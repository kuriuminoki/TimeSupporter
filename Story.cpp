#include "Story.h"
#include "Game.h"
#include "Event.h"
#include "CsvReader.h"
#include "World.h"
#include "Sound.h"
#include "Timer.h"
#include "CharacterLoader.h"
#include "ObjectLoader.h"
#include <sstream>

using namespace std;


vector<string> split(string str, char del) {
	vector<string> result;
	string subStr;

	for (const char c : str) {
		if (c == del) {
			result.push_back(subStr);
			subStr.clear();
		}
		else {
			subStr += c;
		}
	}

	result.push_back(subStr);
	return result;
}


Story::Story(int loop, int time, World* world, SoundPlayer* soundPlayer, EventData* eventData, int worldLifespan, int maxVersion) {
	m_worldEndFlag = false;
	m_timer = new Timer();
	m_timer->setTime(time);
	m_world_p = world;
	m_soundPlayer_p = soundPlayer;
	m_nowEvent = nullptr;
	m_loop = loop;
	m_eventData_p = eventData;

	m_needWorldUpdate = false;
	m_characterLoader = nullptr;
	m_objectLoader = nullptr;

	m_initDark = false;

	m_version = calcVersion(m_timer->getTime(), worldLifespan, maxVersion);
	m_date = calcDate(m_timer->getTime(), worldLifespan);
	if (time == 0) {
		updateWorldVersion();
	}
	m_world_p->setDate(m_date);

	// eventList.csvをロード
	ostringstream oss;
	oss << "data/story/" << "eventList.csv";
	loadEventCsvData(oss.str().c_str(), world, m_soundPlayer_p, worldLifespan / maxVersion);

	// イベントの発火確認
	checkFire();
	m_soundPlayer_p->stopBGM();
}

Story::~Story() {
	delete m_timer;
	for (unsigned int i = 0; i < m_eventList.size(); i++) {
		delete m_eventList[i];
	}
	if (m_nowEvent != nullptr) {
		delete m_nowEvent;
	}
	delete m_characterLoader;
	delete m_objectLoader;
}

// csvファイルを読み込む
void Story::loadEventCsvData(const char* fileName, World* world, SoundPlayer* soundPlayer, int versionTimeSpan) {
	CsvReader csvReader(fileName);
	vector<map<string, string> > data = csvReader.getData();
	for (int i = 0; i < data.size(); i++) {
		int eventNum = stoi(data[i].find("eventNum")->second);
		bool repeat = (bool)stoi(data[i].find("repeat")->second);

		// 前のループまでにクリアしている必要があるイベントの確認
		string preConditions_str = data[i].find("preConditions")->second;
		vector<string> preConditions = split(preConditions_str, '|');
		bool checkConditions = true;
		for (unsigned int j = 0; j < preConditions.size(); j++) {
			if (preConditions[j] != "" && !m_eventData_p->checkClearEvent(stoi(preConditions[j]), m_loop - 1)) {
				checkConditions = false;
			}
		}
		if (checkConditions && (repeat || !m_eventData_p->checkClearEvent(eventNum))) {
			
			// 時間はバージョン単位で指定する。例えばバージョン1と2のちょうど間を表すなら1.5とcsvファイルに記載する。
			int startTime = (int)((stod(data[i].find("startTime")->second) - 1) * versionTimeSpan);
			int endTime = (int)((stod(data[i].find("endTime")->second) - 1) * versionTimeSpan);

			string conditions_str = data[i].find("conditions")->second;
			vector<string> conditions = split(conditions_str, '|');
			vector<int> requireEventNum;
			if (conditions[0] != "") {
				for (unsigned int i = 0; i < conditions.size(); i++) {
					// 既に条件を満たしている（クリアしている）イベントはチェックの必要がないためpush_backしない
					int n = stoi(conditions[i]);
					if (!m_eventData_p->checkClearEvent(n)) {
						requireEventNum.push_back(n);
					}
				}
			}

			m_eventList.push_back(new Event(eventNum, startTime, endTime, requireEventNum, world, soundPlayer, m_version));
		}
	}
}

void Story::loadVersionCsvData(const char* fileName, World* world, SoundPlayer* soundPlayer) {
	delete m_characterLoader;
	delete m_objectLoader;
	m_characterLoader = new CharacterLoader;
	m_objectLoader = new ObjectLoader;
	CsvReader2 csvReader2(fileName);

	// キャラクターを用意
	vector<map<string, string> > characterData = csvReader2.getDomainData("CHARACTER:");
	for (unsigned int i = 0; i < characterData.size(); i++) {
		m_characterLoader->addCharacter(characterData[i]);
	}

	// オブジェクトを用意
	vector<map<string, string> > objectData = csvReader2.getDomainData("OBJECT:");
	for (unsigned int i = 0; i < objectData.size(); i++) {
		m_objectLoader->addObject(objectData[i]);
	}
}

bool Story::play(int worldLifespan, int maxVersion, int timeSpeed) {
	m_initDark = false;
	if (m_nowEvent == nullptr) {
		if (m_timer->getTime() >= worldLifespan) {
			m_timer->setTime(worldLifespan);
			// 世界ループのイベント(9999)をセット
			m_eventList.push_back(new Event(9999, 0, 99999999, vector<int>(), m_world_p, m_soundPlayer_p, m_version));
			m_worldEndFlag = true;
			m_loop++;
		}
		m_timer->advanceTime(timeSpeed);
		m_world_p->battle();
		checkFire();
		int newVersion = calcVersion(m_timer->getTime(), worldLifespan, maxVersion);
		int newDate = calcDate(m_timer->getTime(), worldLifespan);
		if (m_version < newVersion) {
			// 新バージョンをロードし世界を更新
			if (m_version < maxVersion) {
				m_version++;
				updateWorldVersion();
			}
		}
		if (m_date < newDate) {
			m_date = min(2, m_date + 1);
			m_world_p->setDate(m_date);
		}
	}
	else {
		// イベント進行中
		EVENT_RESULT result = m_nowEvent->play();
		
		// イベントクリア
		if (result == EVENT_RESULT::SUCCESS) {
			m_eventData_p->setClearEvent(m_nowEvent->getEventNum(), m_loop);
		}
		// イベント失敗
		if (result == EVENT_RESULT::FAILURE) {
			// mustのイベントならタイトルへ戻る
		}
		// 成功または失敗したのでイベント終了
		if (result != EVENT_RESULT::NOW) {
			delete m_nowEvent;
			m_nowEvent = nullptr;
			return true;
		}
	}

	return false;
}

// イベントの発火確認
void Story::checkFire() {
	for (unsigned int i = 0; i < m_eventList.size(); i++) {
		// タイミングの条件確認
		if (!(m_timer->getTime() >= m_eventList[i]->getStartTime() && m_timer->getTime() < m_eventList[i]->getEndTime())) {
			continue;
		}
		// 依存イベントの条件確認
		const vector<int> requireEventNum = m_eventList[i]->getRequireEventNum();
		bool requireEventComplete = true;
		for (unsigned int i = 0; i < requireEventNum.size(); i++) {
			if (!m_eventData_p->checkClearEvent(requireEventNum[i])) {
				requireEventComplete = false;
				break;
			}
		}
		// 発火確認
		if (requireEventComplete && m_eventList[i]->fire()) {
			m_nowEvent = m_eventList[i];
			m_eventList[i] = m_eventList.back();
			m_eventList.pop_back();
			i--;
		}
	}
}

// ハートのスキル発動が可能かどうか
bool Story::skillAble() {
	if (m_nowEvent == nullptr) {
		return true;
	}
	return m_nowEvent->skillAble();
}

void Story::updateWorldVersion() {
	m_needWorldUpdate = true;
	ostringstream oss;
	oss << "data/story/template/version" << m_version << ".csv";
	loadVersionCsvData(oss.str().c_str(), m_world_p, m_soundPlayer_p);
	for (unsigned int i = 0; i < m_eventList.size(); i++) {
		m_eventList[i]->setVersion(m_version);
	}
	m_world_p->addCharacter(getCharacterLoader());
	m_world_p->addObject(getObjectLoader());
}

void Story::loopInit() {
	m_version = 1;
	m_timer->setTime(0);
	m_date = 0;
}

// セッタ
void Story::setWorld(World* world) {
	m_world_p = world;
	m_world_p->changeCharacterVersion(m_version);
	m_world_p->setDate(m_date);
	if (m_nowEvent != nullptr) {
		m_nowEvent->setWorld(m_world_p);
	}
	for (unsigned int i = 0; i < m_eventList.size(); i++) {
		m_eventList[i]->setWorld(m_world_p);
	}
}

// 前のセーブポイントへ戻る必要があるか 戻るならいくつ分戻るか返す(>0)
int Story::getBackPrevSave() const {
	return m_nowEvent != nullptr ? m_nowEvent->getBackPrevSave() : false;
}

// 前のセーブポイントへ戻ったことを教えてもらう
void Story::doneBackPrevSave() {
	m_nowEvent->doneBackPrevSave();
}


int Story::calcVersion(int time, int worldLifespan, int maxVersion) {
	return 1 + time / (worldLifespan / maxVersion);
}


int Story::calcDate(int time, int worldLifespan) {
	return time / (worldLifespan / 3); // 朝昼晩の3つ
}
