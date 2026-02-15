#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

#include <vector>
#include <string>

class SoundPlayer;
class World;
class SelectStagePage;
class Story;
class Character;
class BattleOption;


// セーブデータ
class GameData {
private:
	const char* INT_DATA_PATH = "intDataNew.dat";
	const char* STR_DATA_PATH = "strDataNew.dat";
	const char* EVENT_DATA_PATH = "eventDataNew.dat";

	// セーブする場所
	std::string m_saveFilePath;

	// セーブデータが存在するか
	bool m_exist;

	// クリアしたステージ数
	int m_completeStageSum;

	// 音量
	int m_soundVolume;

	// 所持金
	int m_money;

	// セーブ完了の通知を表示する残り時間
	int m_noticeSaveDone;

public:
	GameData();
	GameData(const char* saveFilePath);
	~GameData();

	// セーブ完了の通知を表示する時間
	const int NOTICE_SAVE_DONE_TIME = 300;

	// セーブとロード
	bool save();
	bool load();
	// 全セーブデータ共通
	bool saveCommon(int soundVolume, int gameWide, int gameHeight);
	bool loadCommon(int* soundVolume, int* gameWide, int* gameHeight);

	// ゲッタ
	inline bool getExist() const { return m_exist; }
	inline int getCompleteStageSum() const { return m_completeStageSum; }
	inline int getSoundVolume() const { return m_soundVolume; }
	inline int getMoney() const { return m_money; }
	inline const char* getSaveFilePath() const { return m_saveFilePath.c_str(); }
	inline int getNoticeSaveDone() const { return m_noticeSaveDone; }

	// セッタ
	inline void setCompleteStageSum(int completeStageSum) { m_completeStageSum = completeStageSum; }
	inline void setSoundVolume(int soundVolume) { m_soundVolume = soundVolume; }
	inline void setNoticeSaveDone(int noticeSaveDone) { m_noticeSaveDone = noticeSaveDone; }

	// セーブデータ削除
	void removeSaveData();

	// 自身のデータをWorldにデータ反映させる
	void asignWorld(World* world);

	// Worldのデータを自身に反映させる
	void asignedWorld(const World* world);

	// ストーリーが進んだ時にセーブデータを更新する
	void updateStory(Story* story);

};


class Game {
private:

	GameData* m_gameData;

	// サウンドプレイヤー
	SoundPlayer* m_soundPlayer;

	// ゲームオーバーの表示
	int m_gameoverCnt;

	// カーソルの座標
	int m_handX, m_handY;

	// ストーリー
	Story* m_story;

	// 一時停止画面
	BattleOption* m_battleOption;

	// 一時停止画面音
	int m_pauseSound;

	// ゲームの再起動（タイトルへ戻る）を要求
	bool m_rebootFlag;

	SelectStagePage* m_selectStagePage;

public:
	Game(const char* saveFilePath = "savedata/test/");
	~Game();

	// ゲッタ
	SelectStagePage* const getSelectStagePage() const { return m_selectStagePage; }
	Story* const getStory() const { return m_story; }
	BattleOption* getGamePause() const { return m_battleOption; }
	bool getRebootFlag() const { return m_rebootFlag; }
	inline int getGameoverCnt() const { return m_gameoverCnt; }
	inline const GameData* getGameData() const { return m_gameData; }

	// デバッグ
	void debug(int x, int y, int color) const;

	// ゲームをプレイする
	bool play();

	// 描画していいならtrue
	bool ableDraw();

};

#endif