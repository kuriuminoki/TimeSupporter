#include "Game.h"
#include "World.h"
#include "Define.h"
#include "Event.h"
#include "Sound.h"
#include "Story.h"
#include "Control.h"
#include "Character.h"
#include "CharacterAction.h"
#include "CharacterController.h"
#include "CharacterLoader.h"
#include "ObjectLoader.h"
#include "TimeSupporter.h"
#include "Brain.h"
#include "PausePage.h"
#include "DxLib.h"

#include <sstream>
#include <direct.h>

using namespace std;


// エリア0でデバッグするときはtrueにする
const bool TEST_MODE = true;


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

	m_completeStageSum = 0;

}


// ファイルを指定してデータを復元
GameData::GameData(const char* saveFilePath) :
	GameData()
{
	m_saveFilePath = saveFilePath;
	m_exist = load();
}


GameData::~GameData() {

}



// セーブ forceがfalseなら最新のループ以外のセーブを拒否する
bool GameData::save() {

	FILE* intFp = nullptr;

	if (!saveCommon(m_soundVolume, GAME_WIDE, GAME_HEIGHT)) { return false; }

	string fileName = m_saveFilePath;
	if (fopen_s(&intFp, (fileName + INT_DATA_PATH).c_str(), "wb") != 0) {
		return false;
	}

	fwrite(&m_completeStageSum, sizeof(m_completeStageSum), 1, intFp);
	fwrite(&m_money, sizeof(m_money), 1, intFp);

	fclose(intFp);

	// セーブ完了通知の開始
	m_noticeSaveDone = NOTICE_SAVE_DONE_TIME;
	return true;
}


bool GameData::load() {
	FILE* intFp = nullptr;

	if (!loadCommon(&m_soundVolume, &GAME_WIDE, &GAME_HEIGHT)) { return false; }

	string fileName = m_saveFilePath;
	if (fopen_s(&intFp, (fileName + INT_DATA_PATH).c_str(), "rb") != 0) {
		return false;
	}

	fread(&m_completeStageSum, sizeof(m_completeStageSum), 1, intFp);
	fread(&m_money, sizeof(m_money), 1, intFp);

	fclose(intFp);
	return true;
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
void GameData::asignWorld(World* world) {
	world->setMoney(m_money);
}


// Worldのデータを自身に反映させる
void GameData::asignedWorld(const World* world) {
	m_money = world->getMoney();
}


// ストーリーが進んだ時にセーブデータを更新する エリア外（World以外）も考慮する
void GameData::updateStory(Story* story) {
	m_soundVolume = story->getWorld()->getSoundPlayer()->getVolume();
	m_money = story->getWorld()->getMoney();
}



/*
* ゲーム本体
*/
Game::Game(const char* saveFilePath) {

	m_gameData = new GameData(saveFilePath);

	m_soundPlayer = new SoundPlayer();
	m_soundPlayer->setVolume(m_gameData->getSoundVolume());

	m_soundPlayer->stopBGM();

	m_story = nullptr;

	if (TEST_MODE) {
		m_gameData->setCompleteStageSum(28);
	}
	m_selectStagePage = new SelectStagePage(m_gameData->getCompleteStageSum());

	// 一時停止関連
	m_battleOption = nullptr;
	m_pauseSound = LoadSoundMem("sound/system/pause.wav");

	// ゲームの再起動（タイトルへ戻る）を要求するFlag
	m_rebootFlag = false;

	// はじめからプレイする場合初期データをセーブ
	if (!m_gameData->getExist()) {
		m_gameData->save();
	}

	m_gameoverCnt = 0;
}


Game::~Game() {
	delete m_gameData;
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
			m_battleOption = new BattleOption(m_soundPlayer);
			m_soundPlayer->stopBGM();
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
		m_soundPlayer->play();
		return false;
	}
	if (m_story == nullptr) {
		GetMousePoint(&m_handX, &m_handY);
		if (m_selectStagePage->play(m_handX, m_handY)) {
			int targetStoryNum = m_selectStagePage->getFocusStage();
			if (TEST_MODE) {
				targetStoryNum = 0;
			}
			m_story = new Story(targetStoryNum, m_gameData, m_soundPlayer);
		}
	}
	else {
		// ストーリー進行
		EVENT_RESULT result = m_story->play();
		if (result == EVENT_RESULT::SUCCESS) {
			if (m_story->getStoryNum() > m_gameData->getCompleteStageSum()) {
				m_gameData->setCompleteStageSum(m_story->getStoryNum());
				m_selectStagePage->setCompleteStageSum(m_gameData->getCompleteStageSum());
			}
			m_gameData->updateStory(m_story);
			// セーブ (バックアップは更新されない)
			m_gameData->save();
			delete m_story;
		}
	}

	// セーブ完了通知の処理
	m_gameData->setNoticeSaveDone(max(0, m_gameData->getNoticeSaveDone() - 1));

	m_soundPlayer->play();

	return false;
}


// 描画していいならtrue
bool Game::ableDraw() {
	return m_story == nullptr || !m_story->getInitDark();
}
