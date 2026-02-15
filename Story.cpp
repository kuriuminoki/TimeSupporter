#include "Story.h"
#include "Game.h"
#include "Event.h"
#include "CsvReader.h"
#include "World.h"
#include "Sound.h"
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


Story::Story(int storyNum, GameData* gameData, SoundPlayer* soundPlayer) {

	m_storyNum = storyNum;

	m_world = new World(-1, m_storyNum + 1, soundPlayer);

	// データを世界に反映
	m_gameData_p = gameData;
	m_gameData_p->asignWorld(m_world);

	// Worldの初期化
	m_world->cameraPointInit();

	m_soundPlayer_p = soundPlayer;

	m_nowEvent = nullptr;
	m_characterLoader = nullptr;
	m_objectLoader = nullptr;

	m_initDark = false;

	m_date = 0;
	m_world->setDate(m_date);

	// eventList.csvをロード
	m_eventList.push_back(new Event(m_storyNum + 1, m_world, soundPlayer));

	// イベントの発火確認
	checkFire();
}

Story::~Story() {
	delete m_world;
	for (unsigned int i = 0; i < m_eventList.size(); i++) {
		delete m_eventList[i];
	}
	if (m_nowEvent != nullptr) {
		delete m_nowEvent;
	}
	delete m_characterLoader;
	delete m_objectLoader;
}

EVENT_RESULT Story::play() {
	m_initDark = false;
	if (m_nowEvent == nullptr) {
		m_world->battle();
		checkFire();
	}
	else {
		// イベント進行中
		EVENT_RESULT result = m_nowEvent->play();
		
		// イベント失敗
		if (result == EVENT_RESULT::FAILURE) {
			return EVENT_RESULT::FAILURE;
		}
		// 成功または失敗したのでイベント終了
		if (result != EVENT_RESULT::NOW) {
			delete m_nowEvent;
			m_nowEvent = nullptr;
			checkFire();
			if (m_nowEvent == nullptr) {
				return EVENT_RESULT::SUCCESS;
			}
		}
	}

	return  EVENT_RESULT::NOW;
}

// イベントの発火確認
void Story::checkFire() {
	for (unsigned int i = 0; i < m_eventList.size(); i++) {
		// 発火確認
		if (m_eventList[i]->fire()) {
			m_nowEvent = m_eventList[i];
			m_eventList[i] = m_eventList.back();
			m_eventList.pop_back();
			i--;
		}
	}
}

// セッタ
void Story::setWorld(World* world) {
	m_world = world;
	m_world->changeCharacterVersion(1);
	m_world->setDate(m_date);
	if (m_nowEvent != nullptr) {
		m_nowEvent->setWorld(m_world);
	}
	for (unsigned int i = 0; i < m_eventList.size(); i++) {
		m_eventList[i]->setWorld(m_world);
	}
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
