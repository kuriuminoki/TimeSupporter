#include "Game.h"
#include "World.h"
#include "Define.h"
#include "Sound.h"
#include "Story.h"
#include "Timer.h"
#include "Control.h"
#include "Character.h"
#include "CharacterAction.h"
#include "CharacterController.h"
#include "CharacterLoader.h"
#include "ObjectLoader.h"
#include "Brain.h"
#include "PausePage.h"
#include "DxLib.h"

#include <sstream>
#include <direct.h>

using namespace std;


// エリア0でデバッグするときはtrueにする
const bool TEST_MODE = true;


/*
* キャラのデータ
*/
CharacterData::CharacterData(const char* name) {
	m_version = 1;
	m_name = name;
	m_hp = -1;
	m_skillGage = 0;
	m_invincible = false;
	// id=-1はデータなしを意味する
	m_id = -1;
	m_groupId = -1;
	m_areaNum = -1;
	m_x, m_y = -1;
	m_brainName = "";
	m_targetName = "";
	m_followName = "";
	m_actionName = "";
	m_soundFlag = false;
	m_controllerName = "";
}


void CharacterData::save(FILE* intFp, FILE* strFp) {
	fwrite(&m_version, sizeof(m_version), 1, intFp);
	fwrite(&m_hp, sizeof(m_hp), 1, intFp);
	fwrite(&m_skillGage, sizeof(m_skillGage), 1, intFp);
	fwrite(&m_invincible, sizeof(m_invincible), 1, intFp);
	fwrite(&m_id, sizeof(m_id), 1, intFp);
	fwrite(&m_groupId, sizeof(m_groupId), 1, intFp);
	fwrite(&m_areaNum, sizeof(m_areaNum), 1, intFp);
	fwrite(&m_x, sizeof(m_x), 1, intFp);
	fwrite(&m_y, sizeof(m_y), 1, intFp);
	fwrite(&m_soundFlag, sizeof(m_soundFlag), 1, intFp);

	fprintf(strFp, "%s\n", m_name.c_str());
	fprintf(strFp, "%s\n", m_brainName.c_str());
	fprintf(strFp, "%s\n", m_targetName.c_str());
	fprintf(strFp, "%s\n", m_followName.c_str());
	fprintf(strFp, "%s\n", m_actionName.c_str());
	fprintf(strFp, "%s\n", m_controllerName.c_str());
}


void CharacterData::load(FILE* intFp, FILE* strFp) {
	fread(&m_version, sizeof(m_version), 1, intFp);
	fread(&m_hp, sizeof(m_hp), 1, intFp);
	fread(&m_skillGage, sizeof(m_skillGage), 1, intFp);
	fread(&m_invincible, sizeof(m_invincible), 1, intFp);
	fread(&m_id, sizeof(m_id), 1, intFp);
	fread(&m_groupId, sizeof(m_groupId), 1, intFp);
	fread(&m_areaNum, sizeof(m_areaNum), 1, intFp);
	fread(&m_x, sizeof(m_x), 1, intFp);
	fread(&m_y, sizeof(m_y), 1, intFp);
	fread(&m_soundFlag, sizeof(m_soundFlag), 1, intFp);

	const int N = 256;
	char* find;
	char str[N];
	fgets(str, N, strFp);
	if ((find = strchr(str, '\n')) != nullptr) { *find = '\0'; }
	m_name = str;
	fgets(str, N, strFp);
	if ((find = strchr(str, '\n')) != nullptr) { *find = '\0'; }
	m_brainName = str;
	fgets(str, N, strFp);
	if ((find = strchr(str, '\n')) != nullptr) { *find = '\0'; }
	m_targetName = str;
	fgets(str, N, strFp);
	if ((find = strchr(str, '\n')) != nullptr) { *find = '\0'; }
	m_followName = str;
	fgets(str, N, strFp);
	if ((find = strchr(str, '\n')) != nullptr) { *find = '\0'; }
	m_actionName = str;
	fgets(str, N, strFp);
	if ((find = strchr(str, '\n')) != nullptr) { *find = '\0'; }
	m_controllerName = str;
}


/*
* ドアのデータ
*/
DoorData::DoorData(FILE* intFp, FILE* strFp) {
	load(intFp, strFp);
}
DoorData::DoorData(int x1, int y1, int x2, int y2, int from, int to, const char* fileName) {
	m_x1 = x1;
	m_y1 = y1;
	m_x2 = x2;
	m_y2 = y2;
	m_from = from;
	m_to = to;
	m_fileName = fileName;
}


void DoorData::save(FILE* intFp, FILE* strFp) {
	fwrite(&m_x1, sizeof(m_x1), 1, intFp);
	fwrite(&m_y1, sizeof(m_y1), 1, intFp);
	fwrite(&m_x2, sizeof(m_x2), 1, intFp);
	fwrite(&m_y2, sizeof(m_y2), 1, intFp);
	fwrite(&m_from, sizeof(m_from), 1, intFp);
	fwrite(&m_to, sizeof(m_to), 1, intFp);

	fprintf(strFp, "%s\n", m_fileName.c_str());
}


void DoorData::load(FILE* intFp, FILE* strFp) {
	fread(&m_x1, sizeof(m_x1), 1, intFp);
	fread(&m_y1, sizeof(m_y1), 1, intFp);
	fread(&m_x2, sizeof(m_x2), 1, intFp);
	fread(&m_y2, sizeof(m_y2), 1, intFp);
	fread(&m_from, sizeof(m_from), 1, intFp);
	fread(&m_to, sizeof(m_to), 1, intFp);

	const int N = 256;
	char* find;
	char str[N];
	fgets(str, N, strFp);
	if ((find = strchr(str, '\n')) != nullptr) { *find = '\0'; }
	m_fileName = str;
}


/*
* クリアしたイベントのリスト
*/
EventData::EventData() {

}
EventData::EventData(FILE* eventFp) {
	load(eventFp);
}


void EventData::save(FILE* eventFp) {
	if (m_clearEvent.size() == 0) {
		// 空のファイルだとロード時にうまくいかないため0を入れて空にならないようにする。
		m_clearEvent.push_back(0);
		m_clearLoop.push_back(0);
	}
	for (unsigned int i = 0; i < m_clearEvent.size(); i++) {
		fwrite(&m_clearEvent[i], sizeof(int), 1, eventFp);
		fwrite(&m_clearLoop[i], sizeof(int), 1, eventFp);
	}
}


void EventData::load(FILE* eventFp) {
	while (feof(eventFp) == 0) {
		int num;
		fread(&num, sizeof(int), 1, eventFp);
		int loop;
		fread(&loop, sizeof(int), 1, eventFp);
		setClearEvent(num, loop);
	}
}


// 特定のイベントをクリアしてるか
bool EventData::checkClearEvent(int eventNum, int loop) {
	for (unsigned int i = 0; i < m_clearEvent.size(); i++) {
		if (m_clearEvent[i] == eventNum && m_clearLoop[i] <= loop) {
			return true;
		}
	}
	return false;
}


//特定のイベントをクリア状態にする
void EventData::setClearEvent(int eventNum, int loop) {
	if (!checkClearEvent(eventNum)) {
		m_clearEvent.push_back(eventNum);
		m_clearLoop.push_back(loop);
	}
}


/*
* ゲームのセーブデータ
*/
// 初期状態のデータを作成
GameData::GameData() {

	m_noticeSaveDone = 0;

	m_exist = false;

	m_soundVolume = 50;

	m_money = 0;

	loadCommon(&m_soundVolume, &GAME_WIDE, &GAME_HEIGHT);

	m_saveFilePath = "";

	m_areaNum = 1;
	m_time = 0;
	m_loop = 1;
	m_latestLoop = 0;

	if (TEST_MODE) {
		m_areaNum = 0;
		m_time = 0;
		m_loop = 1;
	}

	// 主要キャラを設定
	const int mainSum = 15;
	const char* mainCharacters[mainSum] = {
		"ハート",
		"シエスタ",
		"ヒエラルキー",
		"ヴァルキリア",
		"トロイ",
		"メモリー",
		"ユーリ",
		"エム・サディ",
		"フレンチ",
		"アーカイブ",
		"アイギス",
		"コハル",
		"マスカーラ",
		"ヴェルメリア",
		"サン"
	};
	for (int i = 0; i < mainSum; i++) {
		m_characterData.push_back(new CharacterData(mainCharacters[i]));
	}

	m_eventData = new EventData();

}


// ファイルを指定してデータを復元
GameData::GameData(const char* saveFilePath) :
	GameData()
{
	m_saveFilePath = saveFilePath;
	m_exist = load();
}


// ファイルとループを指定してデータを復元
GameData::GameData(const char* saveFilePath, int loop) :
	GameData()
{
	m_saveFilePath = saveFilePath;
	// いったん最新のデータを読み込む
	m_exist = load();

	// 古いループのデータを読み込んで上書き
	loadLoop(loop);
}


GameData::~GameData() {
	for (unsigned int i = 0; i < m_characterData.size(); i++) {
		delete m_characterData[i];
	}
	for (unsigned int i = 0; i < m_doorData.size(); i++) {
		delete m_doorData[i];
	}
	delete m_eventData;
}


CharacterData* GameData::getCharacterData(string characterName) {
	for (unsigned int i = 0; i < m_characterData.size(); i++) {
		if (m_characterData[i]->name()) {
			return m_characterData[i];
		}
	}
	return nullptr;
}


// セーブ forceがfalseなら最新のループ以外のセーブを拒否する
bool GameData::save(bool force) {

	// 今やっているループが最新ならセーブ
	if (m_loop == m_latestLoop || force) {
		FILE* intFp = nullptr, * strFp = nullptr, * eventFp = nullptr;

		if (!saveCommon(m_soundVolume, GAME_WIDE, GAME_HEIGHT)) { return false; }

		string fileName = m_saveFilePath;
		if (fopen_s(&intFp, (fileName + INT_DATA_PATH).c_str(), "wb") != 0 ||
			fopen_s(&strFp, (fileName + STR_DATA_PATH).c_str(), "wb") != 0 ||
			fopen_s(&eventFp, (fileName + EVENT_DATA_PATH).c_str(), "wb") != 0) {
			return false;
		}

		fwrite(&m_areaNum, sizeof(m_areaNum), 1, intFp);
		fwrite(&m_time, sizeof(m_time), 1, intFp);
		fwrite(&m_loop, sizeof(m_loop), 1, intFp);
		fwrite(&m_latestLoop, sizeof(m_latestLoop), 1, intFp);
		fwrite(&m_money, sizeof(m_money), 1, intFp);
		for (unsigned int i = 0; i < m_characterData.size(); i++) {
			m_characterData[i]->save(intFp, strFp);
		}
		unsigned int doorSum = (unsigned int)m_doorData.size();
		fwrite(&doorSum, sizeof(doorSum), 1, intFp);
		for (unsigned int i = 0; i < m_doorData.size(); i++) {
			m_doorData[i]->save(intFp, strFp);
		}
		m_eventData->save(eventFp);

		fclose(intFp);
		fclose(strFp);
		fclose(eventFp);

		// セーブ完了通知の開始 最初だけはしない
		if (m_time > 1) {
			m_noticeSaveDone = NOTICE_SAVE_DONE_TIME;
		}
	}
	return true;
}


bool GameData::load() {
	FILE* intFp = nullptr, * strFp = nullptr, * eventFp = nullptr;

	if (!loadCommon(&m_soundVolume, &GAME_WIDE, &GAME_HEIGHT)) { return false; }

	string fileName = m_saveFilePath;
	if (fopen_s(&intFp, (fileName + INT_DATA_PATH).c_str(), "rb") != 0 ||
		fopen_s(&strFp, (fileName + STR_DATA_PATH).c_str(), "rb") != 0 ||
		fopen_s(&eventFp, (fileName + EVENT_DATA_PATH).c_str(), "rb") != 0) {
		return false;
	}

	fread(&m_areaNum, sizeof(m_areaNum), 1, intFp);
	fread(&m_time, sizeof(m_time), 1, intFp);
	fread(&m_loop, sizeof(m_loop), 1, intFp);
	fread(&m_latestLoop, sizeof(m_latestLoop), 1, intFp);
	fread(&m_money, sizeof(m_money), 1, intFp);
	for (unsigned int i = 0; i < m_characterData.size(); i++) {
		if (feof(intFp) != 0 || feof(strFp) != 0) { break; }
		m_characterData[i]->load(intFp, strFp);
	}
	int doorSum = 0;
	m_doorData.clear();
	fread(&doorSum, sizeof(doorSum), 1, intFp);
	for (int i = 0; i < doorSum; i++) {
		m_doorData.push_back(new DoorData(intFp, strFp));
	}
	m_eventData->init();
	m_eventData->load(eventFp);

	fclose(intFp);
	fclose(strFp);
	fclose(eventFp);
	return true;
}

// バックアップを取る（ループ巻き戻し機能用）
bool GameData::saveLoop() {

	// 一時的にパスを savedata/<セーブデータ番号>/loop/<m_loop>/ にする
	string filePath = m_saveFilePath;
	ostringstream oss;
	oss << m_saveFilePath << "loop/" << m_loop;
	_mkdir(oss.str().c_str());
	m_saveFilePath = oss.str() + "/";
	save(true);
	// パスをもとに戻す
	m_saveFilePath = filePath;

	return true;
}


// ループを指定してロード、latestLoopだけは変わらない
bool GameData::loadLoop(int loop) {
	int latestLoop = m_latestLoop;
	string filePath = m_saveFilePath;
	ostringstream oss;
	oss << m_saveFilePath << "loop/" << loop << "/";
	m_saveFilePath = oss.str();
	bool flag = load();
	m_saveFilePath = filePath;
	m_latestLoop = latestLoop;
	return flag;
}


// 全セーブデータ共通で1つだけ記録するデータ
bool GameData::saveCommon(int soundVolume, int gameWide, int gameHeight) {

	FILE* commonFp = nullptr;
	if (fopen_s(&commonFp, "savedata/commonData.dat", "wb") != 0) {
		return false;
	}
	fwrite(&soundVolume, sizeof(soundVolume), 1, commonFp);
	fwrite(&gameWide, sizeof(gameWide), 1, commonFp);
	fwrite(&gameHeight, sizeof(gameHeight), 1, commonFp);
	fclose(commonFp);
	return true;
}

// 全セーブデータ共通の項目だけをロード
bool GameData::loadCommon(int* soundVolume, int* gameWide, int* gameHeight) {

	FILE* commonFp = nullptr;
	if (fopen_s(&commonFp, "savedata/commonData.dat", "rb") != 0) {
		return false;
	}
	fread(soundVolume, sizeof(*soundVolume), 1, commonFp);
	fread(gameWide, sizeof(*gameWide), 1, commonFp);
	fread(gameHeight, sizeof(*gameHeight), 1, commonFp);
	fclose(commonFp);
	return true;
}


// セーブデータ削除
void GameData::removeSaveData() {
	string fileName = m_saveFilePath;
	remove((fileName + INT_DATA_PATH).c_str());
	remove((fileName + STR_DATA_PATH).c_str());
	remove((fileName + EVENT_DATA_PATH).c_str());
}


// 自身のデータをWorldにデータ反映させる
void GameData::asignWorld(World* world, bool playerHpReset) {
	size_t size = m_characterData.size();
	for (unsigned int i = 0; i < size; i++) {
		world->asignedCharacterData(m_characterData[i]->name(), m_characterData[i]);
	}
	size = m_doorData.size();
	for (unsigned int i = 0; i < size; i++) {
		world->asignedDoorData(m_doorData[i]);
	}
	if (playerHpReset) {
		world->playerHpReset();
	}
	world->setMoney(m_money);
}


// Worldのデータを自身に反映させる
void GameData::asignedWorld(const World* world, bool notCharacterPoint) {
	size_t size = m_characterData.size();
	for (unsigned int i = 0; i < size; i++) {
		world->asignCharacterData(m_characterData[i]->name(), m_characterData[i], m_areaNum, notCharacterPoint);
	}
	world->asignDoorData(m_doorData, m_areaNum);
	m_money = world->getMoney();
}


// ストーリーが進んだ時にセーブデータを更新する エリア外（World以外）も考慮する
void GameData::updateStory(Story* story) {
	m_areaNum = story->getWorld()->getAreaNum();
	m_time = story->getTimer()->getTime();
	m_loop = story->getLoop();
	m_latestLoop = max(m_latestLoop, m_loop);
	m_soundVolume = story->getWorld()->getSoundPlayer()->getVolume();
	m_money = story->getWorld()->getMoney();
}


void GameData::updateWorldVersion(Story* story) {
	// Storyによって変更・新登場されたキャラ情報を取得
	CharacterLoader* characterLoader = story->getCharacterLoader();
	size_t size = m_characterData.size();
	for (unsigned int i = 0; i < size; i++) {
		characterLoader->saveCharacterData(m_characterData[i]);
	}
	// ドアの情報も取得
	ObjectLoader* objectLoader = story->getObjectLoader();
	objectLoader->saveDoorData(m_doorData);
	story->doneWorldUpdate();
}


// 世界のやり直し
void GameData::resetWorld() {
	for (unsigned int i = 0; i < m_characterData.size(); i++) {
		m_characterData[i]->setId(-1);
		m_characterData[i]->setAreaNum(-1);
	}
	m_doorData.clear();
	m_money = 0;
}



/*
* ゲーム本体
*/
Game::Game(const char* saveFilePath, int loop) {

	m_timeSpeed = DEFAULT_TIME_SPEED;

	if (loop == -1) { // ループ指定なし、最新のループ
		m_gameData = new GameData(saveFilePath);
	}
	else { // ループ指定あり
		m_gameData = new GameData(saveFilePath, loop);
	}

	m_soundPlayer = new SoundPlayer();
	m_soundPlayer->setVolume(m_gameData->getSoundVolume());

	m_world = new World(-1, m_gameData->getAreaNum(), m_soundPlayer);
	m_world->setMoney(m_gameData->getMoney());
	m_soundPlayer->stopBGM();

	m_story = new Story(m_gameData->getLoop(), m_gameData->getTime(), m_world, m_soundPlayer, m_gameData->getEventData(), WORLD_LIFESPAN, MAX_VERSION);
	m_world->changeCharacterVersion(m_story->getVersion());
	m_world->setDate(m_story->getDate());

	// セーブデータに上書き
	if (!TEST_MODE) {
		m_gameData->updateStory(m_story);
	}

	// データを世界に反映
	m_gameData->asignWorld(m_world, false);

	m_world->cameraPointInit();

	// 一時停止関連
	m_battleOption = nullptr;
	m_pauseSound = LoadSoundMem("sound/system/pause.wav");

	// ゲームの再起動（タイトルへ戻る）を要求するFlag
	m_rebootFlag = false;

	// はじめからプレイする場合初期データをセーブ
	if (!m_gameData->getExist()) {
		m_gameData->save();
		m_gameData->saveLoop();
	}

	m_gameoverCnt = 0;
}


Game::~Game() {
	delete m_gameData;
	delete m_world;
	delete m_story;
	if (m_battleOption != nullptr) {
		delete m_battleOption;
	}
	DeleteSoundMem(m_pauseSound);
	delete m_soundPlayer;
}


bool Game::play() {

	// ゲームオーバー
	if (m_gameoverCnt > 0) {
		m_gameoverCnt++;
		if (m_gameoverCnt == 120) {
			m_rebootFlag = true;
		}
		return false;
	}

	// 一時停止
	if (controlQ() == 1) {
		if (m_battleOption == nullptr) {
			if (m_timeSpeed == DEFAULT_TIME_SPEED) {
				m_battleOption = new BattleOption(m_soundPlayer);
				m_soundPlayer->stopBGM();
			}
			else {
				// 速度アップモード中なら一時停止せずモード解除だけ
				m_timeSpeed = DEFAULT_TIME_SPEED;
			}
		}
		else {
			// 音量の変更があるかもしれないのでセーブ
			m_gameData->saveCommon(m_battleOption->getNewSoundVolume(), GAME_WIDE, GAME_HEIGHT);
			delete m_battleOption;
			m_battleOption = nullptr;
			m_soundPlayer->playBGM();
		}
		m_soundPlayer->pushSoundQueue(m_pauseSound);
	}
	if (m_battleOption != nullptr) {
		m_battleOption->play();
		if (m_battleOption->getTitleFlag()) {
			// 音量の変更があるかもしれないのでセーブしタイトルへ戻る
			m_gameData->saveCommon(m_battleOption->getNewSoundVolume(), GAME_WIDE, GAME_HEIGHT);
			m_rebootFlag = true;
		}
		if (m_battleOption->getQuickFlag()) {
			if (!m_story->eventNow()) {
				// 速度アップモードを開始
				m_timeSpeed = QUICK_TIME_SPEED;
			}
			delete m_battleOption;
			m_battleOption = nullptr;
			m_soundPlayer->playBGM();
		}
		m_soundPlayer->play();
		return false;
	}

	if (TEST_MODE) {
		m_world->battle();
		if (m_world->getWorldFreezeTime() == 0) {
			m_soundPlayer->play();
		}
		return false;
	}
	// ストーリー進行
	else if (!TEST_MODE && m_story->play(WORLD_LIFESPAN, MAX_VERSION, m_timeSpeed)) {
		m_timeSpeed = DEFAULT_TIME_SPEED;
		// 次のループへ移行の場合Storyを作り直す
		if (m_story->getLoop() > m_gameData->getLoop()) {
			int nextLoopNum = m_story->getLoop();
			delete m_story;
			m_story = new Story(nextLoopNum, 0, m_world, m_soundPlayer, m_gameData->getEventData(), WORLD_LIFESPAN, MAX_VERSION);
			m_gameData->updateStory(m_story);
			// ループ直後の状態をバックアップ
			m_gameData->saveLoop();
		}
		else { // イベントクリア
			m_gameData->updateStory(m_story);
			m_gameData->asignedWorld(m_world, false);
			if (m_gameData->getLoop() < m_gameData->getLatestLoop()) {
				m_gameData->saveLoop();// 過去のデータ(バックアップ)をプレイ中ならそのデータを更新
			}
		}
		// セーブ (バックアップは更新されない)
		m_gameData->save();
	}

	if (m_story->eventNow()) {
		m_timeSpeed = DEFAULT_TIME_SPEED;
	}

	if (m_story->getNeedWorldUpdate()) {
		m_gameData->updateWorldVersion(m_story);
	}

	// セーブ完了通知の処理
	m_gameData->setNoticeSaveDone(max(0, m_gameData->getNoticeSaveDone() - 1));

	if (m_world->getWorldFreezeTime() == 0) {
		m_soundPlayer->play();
	}

	// 前のセーブポイントへ戻ることが要求された TODO: 削除
	int prevLoop = m_story->getBackPrevSave();
	if (prevLoop > 0) {
		backPrevSave();
		m_story->doneBackPrevSave();
		return true;
	}
	// ゲームオーバー
	else if (m_world->playerDead() && m_world->getBrightValue() == 0) {
		// storyからハートがやられたことを伝えられたらタイトルへ戻る
		// やられるのがイベントの成功条件なら前のif文(m_story->getBackPrevSaveFlag())にひっかかるはず
		m_gameoverCnt++;
	}
	// エリア移動
	else if (m_world->getBrightValue() == 0 && CheckSoundMem(m_world->getDoorSound()) == 0) {
		m_world->changePlayer(m_world->getCharacterWithName("ハート"));
		int fromAreaNum = m_world->getAreaNum();
		int toAreaNum = m_world->getNextAreaNum();
		m_gameData->asignedWorld(m_world, false);
		bool resetBgmFlag = m_world->getResetBgmFlag();
		bool blindFlag = m_world->getBlindFlag();
		delete m_world;
		m_world = new World(fromAreaNum, toAreaNum, m_soundPlayer);
		m_world->setBlindFlag(blindFlag);
		if (resetBgmFlag) { m_soundPlayer->stopBGM(); }
		m_gameData->asignWorld(m_world);
		if (m_story->getLoop() == m_gameData->getLoop()) {
			m_world->setPlayerOnDoor(fromAreaNum);
		}
		else { // ループが起きたことによるエリア移動の時は初期化などが必要
			m_world->cameraPointInit();
			m_world->setBlindFlag(false);
			m_world->clearCharacter();
			m_story->loopInit();
			m_gameData->resetWorld();
			m_gameData->asignWorld(m_world, true);
			m_soundPlayer->stopBGM();
		}
		m_story->setWorld(m_world);
		m_gameData->setAreaNum(toAreaNum);
		m_story->checkFire(); // これがないとエリア移動した瞬間に始まるイベントのFireが1F遅れる
		m_world->playBGM(); // エリア移動した瞬間にイベントが始まると無音になるのを防ぐ
		return true;
	}

	return false;
}


// セーブデータをロード（前のセーブポイントへ戻る） TODO: 削除
void Game::backPrevSave() {
	m_gameData->asignedWorld(m_world, true);
	// これまでのWorldを削除
	delete m_world;
	// 前のセーブデータをロード
	GameData prevData(m_gameData->getSaveFilePath(), m_gameData->getLoop());
	// 以前のAreaNumでロード
	m_world = new World(-1, prevData.getAreaNum(), m_soundPlayer);
	m_gameData->asignWorld(m_world, true);
	m_world->setPlayerPoint(prevData.getCharacterData("ハート"));
	m_world->setPlayerFollowerPoint();
	m_story->setWorld(m_world);
}


// 描画していいならtrue
bool Game::ableDraw() {
	return !m_story->getInitDark();
}


// スキル発動できるところまでストーリーが進んでいるか
bool Game::afterSkillUsableLoop() const {
	return m_gameData->getLoop() > 1;
}
