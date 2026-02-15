#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

#include <vector>
#include <string>

class SoundPlayer;
class World;
class Story;
class Character;
class BattleOption;


// キャラのセーブデータ
class CharacterData {
private:

	// 名前
	std::string m_name;

	// Infoのバージョン
	int m_version;

	// HP
	int m_hp;

	// スキルゲージ
	int m_skillGage;

	// 無敵ならtrue
	bool m_invincible;

	// ID
	int m_id;

	// GroupID
	int m_groupId;

	// どこのエリアにいるか
	int m_areaNum;

	// 座標
	int m_x, m_y;

	// Brainのクラス名
	std::string m_brainName;

	// 攻撃対象の名前 いないなら空文字
	std::string m_targetName;

	// 追跡対象の名前 いないなら空文字
	std::string m_followName;

	// CharacterActionのクラス名
	std::string m_actionName;

	// 効果音ありか
	bool m_soundFlag;

	// CharacterControllerのクラス名
	std::string m_controllerName;

public:
	CharacterData(const char* name);

	// セーブとロード
	void save(FILE* intFp, FILE* strFp);
	void load(FILE* intFp, FILE* strFp);

	// ゲッタ
	inline const char* name() const { return m_name.c_str(); }
	inline const int version() const { return m_version; }
	inline int hp() const { return m_hp; }
	inline int skillGage() const { return m_skillGage; }
	inline bool invincible() const { return m_invincible; }
	inline int id() const { return m_id; }
	inline int groupId() const { return m_groupId; }
	inline int areaNum() const { return m_areaNum; }
	inline int x() const { return m_x; }
	inline int y() const { return m_y; }
	inline std::string brainName() const { return m_brainName; }
	inline std::string targetName() const { return m_targetName; }
	inline std::string followName() const { return m_followName; }
	inline std::string actionName() const { return m_actionName; }
	inline bool soundFlag() const { return m_soundFlag; }
	inline std::string controllerName() const { return m_controllerName; }

	// セッタ
	inline void setVersion(int version) { m_version = version; }
	inline void setHp(int hp) { m_hp = hp; }
	inline void getSkillGage(int skillGage) { m_skillGage = skillGage; }
	inline void setInvincible(bool invincible) { m_invincible = invincible; }
	inline void setId(int id) { m_id = id; }
	inline void setGroupId(int groupId) { m_groupId = groupId; }
	inline void setAreaNum(int areaNum) { m_areaNum = areaNum; }
	inline void setX(int x) { m_x = x; }
	inline void setY(int y) { m_y = y; }
	inline void setBrainName(const char* brainName) { m_brainName = brainName; }
	inline void setTargetName(const char* targetName) { m_targetName = targetName; }
	inline void setFollowName(const char* followName) { m_followName = followName; }
	inline void setActionName(const char* actionName) { m_actionName = actionName; }
	inline void setSoundFlag(bool soundFlag) { m_soundFlag = soundFlag; }
	inline void setControllerName(const char* controllerName) { m_controllerName = controllerName; }
};


class DoorData {
private:

	// 座標
	int m_x1, m_y1, m_x2, m_y2;

	// どこからどこへのドアか
	int m_from, m_to;

	// 画像のファイル名
	std::string m_fileName;

public:
	DoorData(FILE* intFp, FILE* strFp);
	DoorData(int x1, int y1, int x2, int y2, int from, int to, const char* fileName);

	// セーブとロード
	void save(FILE* intFp, FILE* strFp);
	void load(FILE* intFp, FILE* strFp);

	// ゲッタ
	inline int x1() const { return m_x1; }
	inline int y1() const { return m_y1; }
	inline int x2() const { return m_x2; }
	inline int y2() const { return m_y2; }
	inline int from() const { return m_from; }
	inline int to() const { return m_to; }
	inline const char* fileName() const { return m_fileName.c_str(); }

	// セッタ
	inline void setX1(int x1) { m_x1 = x1; }
	inline void setY1(int y1) { m_y1 = y1; }
	inline void setX2(int x2) { m_x2 = x2; }
	inline void setY2(int y2) { m_y2 = y2; }
	inline void setFrom(int from) { m_from = from; }
	inline void setTo(int to) { m_to = to; }
	inline void setFileName(const char* fileName) { m_fileName = fileName; }
};


/*
* クリアしたイベントのリスト
*/
class EventData {
private:

	// クリアしたイベント番号
	std::vector<int> m_clearEvent;

	// クリアした周
	std::vector<int> m_clearLoop;

public:

	EventData();
	EventData(FILE* eventFp);

	void save(FILE* eventFp);
	void load(FILE* eventFp);

	// 初期化
	void init() { 
		m_clearEvent.clear();
		m_clearLoop.clear();
	}

	// 特定のイベントをクリアしてるか (0 ~ loopまでの周回が対象)
	bool checkClearEvent(int eventNum, int loop = 100);

	//特定のイベントをクリアした
	void setClearEvent(int eventNum, int loop);

};


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

	// キャラのデータ
	std::vector<CharacterData*> m_characterData;

	// ドアのデータ
	std::vector<DoorData*> m_doorData;

	// イベントのデータ
	EventData* m_eventData;

	// 今いるエリア
	int m_areaNum;

	// 今やっているストーリー
	int m_time;

	int m_loop;

	// 最新の未ループ
	int m_latestLoop;

	// 音量
	int m_soundVolume;

	// 所持金
	int m_money;

	// セーブ完了の通知を表示する残り時間
	int m_noticeSaveDone;

public:
	GameData();
	GameData(const char* saveFilePath);
	GameData(const char* saveFilePath, int loop);
	~GameData();

	// セーブ完了の通知を表示する時間
	const int NOTICE_SAVE_DONE_TIME = 300;

	// セーブとロード
	bool save(bool force = false);
	bool load();
	bool saveLoop();
	bool loadLoop(int loop);
	// 全セーブデータ共通
	bool saveCommon(int soundVolume, int gameWide, int gameHeight);
	bool loadCommon(int* soundVolume, int* gameWide, int* gameHeight);

	// ゲッタ
	inline bool getExist() const { return m_exist; }
	inline int getAreaNum() const { return m_areaNum; }
	inline int getTime() const { return m_time; }
	inline int getLoop() const { return m_loop; }
	inline int getSoundVolume() const { return m_soundVolume; }
	inline int getMoney() const { return m_money; }
	inline const char* getSaveFilePath() const { return m_saveFilePath.c_str(); }
	inline int getDoorSum() const { return (int)m_doorData.size(); }
	inline int getFrom(int i) const { return m_doorData[i]->from(); }
	inline int getTo(int i) const { return m_doorData[i]->to(); }
	inline int getLatestLoop() const { return m_latestLoop; }
	inline EventData* getEventData() { return m_eventData; }
	inline int getNoticeSaveDone() const { return m_noticeSaveDone; }
	CharacterData* getCharacterData(std::string characterName);

	// セッタ
	inline void setAreaNum(int areaNum) { m_areaNum = areaNum; }
	inline void setTime(int time) { m_time = time; }
	inline void setLoop(int loop) { m_loop = loop; }
	inline void setSoundVolume(int soundVolume) { m_soundVolume = soundVolume; }
	inline void setNoticeSaveDone(int noticeSaveDone) { m_noticeSaveDone = noticeSaveDone; }

	// セーブデータ削除
	void removeSaveData();

	// 自身のデータをWorldにデータ反映させる
	void asignWorld(World* world, bool playerHpReset = false);

	// Worldのデータを自身に反映させる
	void asignedWorld(const World* world, bool notCharacterPoint);

	// ストーリーが進んだ時にセーブデータを更新する
	void updateStory(Story* story);

	void updateWorldVersion(Story* story);

	// 世界のやり直し
	void resetWorld();

};


// ハートのスキル
class HeartSkill {
public:

	// 何秒間か
	const int DUPLICATION_TIME = 300;

private:
	// 複製の数
	int m_loopNum;

	// 今何ループ目か
	int m_loopNow;

	// 元の世界
	World* m_world_p;

	// 複製
	World* m_duplicationWorld;

	// DUPLICATION_TIMEまでカウントする
	int m_cnt;

	// 複製のキャラID スキル終了時に消すため
	std::vector<int> m_duplicationId;

	// サウンドプレイヤー(bgm用)
	SoundPlayer* m_soundPlayer;

	// サウンドプレイヤー
	SoundPlayer* m_soundPlayer_p;

	// 効果音
	int m_sound;

public:
	HeartSkill(int loopNum, World* world, SoundPlayer* soundPlayer);
	~HeartSkill();

	// ゲッタ
	inline int getLoopNum() const { return m_loopNum; }
	inline int getLoopNow() const { return m_loopNow; }
	inline World* getWorld() const { return m_loopNow < m_loopNum ? m_duplicationWorld : m_world_p; }
	inline int getOriginalCnt() const { return m_cnt; }
	inline double getCnt() const { return ((double)DUPLICATION_TIME / 60.0) - ((double)m_cnt / 60.0); }

	// スキル進行中 スキル終了時にtrue
	bool play();

	// 戦わせる（操作記録をするという言い方が正しい）
	void battle();

	// 操作記録が終わったかどうかの判定
	bool finishRecordFlag();

	void controlBGM(bool on, int soundVolume = -1);

private:
	// 世界のコピーを作る コピーの変更はオリジナルに影響しない
	World* createDuplicationWorld(const World* world);

	void createDuplicationHeart();
};


class Game {
public:
	// 世界が終わるまでの時間 X分 * 60 * FPS 36000だと10min debug時1200とか
	const int WORLD_LIFESPAN = 36000;

	const int MAX_VERSION = 6;

	// 複製の最大数
	const int MAX_SKILL = 6;

private:

	GameData* m_gameData;

	// サウンドプレイヤー
	SoundPlayer* m_soundPlayer;

	// 世界
	World* m_world;

	// ゲームオーバーの表示
	int m_gameoverCnt;

	// ストーリー
	Story* m_story;

	// スキル
	HeartSkill* m_skill;

	// 一時停止画面
	BattleOption* m_battleOption;

	// 時間の進む速度
	int m_timeSpeed;
	const int DEFAULT_TIME_SPEED = 1;
	const int QUICK_TIME_SPEED = 10;

	// 一時停止画面音
	int m_pauseSound;

	// ゲームの再起動（タイトルへ戻る）を要求
	bool m_rebootFlag;

public:
	Game(const char* saveFilePath = "savedata/test/", int loop = -1);
	~Game();

	// ゲッタ
	Story* const getStory() const { return m_story; }
	World* getWorld() const { return m_world; }
	HeartSkill* getSkill() const { return m_skill; }
	BattleOption* getGamePause() const { return m_battleOption; }
	bool getRebootFlag() const { return m_rebootFlag; }
	inline int getGameoverCnt() const { return m_gameoverCnt; }
	inline const GameData* getGameData() const { return m_gameData; }

	// デバッグ
	void debug(int x, int y, int color) const;

	// ゲームをプレイする
	bool play();

	// セーブデータをロード（前のセーブポイントへ戻る）
	void backPrevSave();

	// 描画していいならtrue
	bool ableDraw();

	// スキル発動できるところまでストーリーが進んでいるか
	bool afterSkillUsableLoop() const;

	bool quickModeNow() const { return !(m_timeSpeed == DEFAULT_TIME_SPEED); }

private:

	// スキルの終了
	void endSkill();

	bool skillUsable();

};

#endif