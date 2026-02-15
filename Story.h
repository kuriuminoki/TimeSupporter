#ifndef STORY_H_INCLUDED
#define STORY_H_INCLUDED

#include <string>
#include <vector>

class Event;
class EventData;
class World;
class SoundPlayer;
class Timer;
class CharacterLoader;
class ObjectLoader;


// ストーリ－
class Story {
private:

	// 経過時間（ループの度リセット）
	Timer* m_timer;

	// 何週目の世界か
	int m_loop;

	// 対象の世界
	World* m_world_p;

	SoundPlayer* m_soundPlayer_p;

	// 時間帯
	int m_date;

	// Emote Onlineのversion
	int m_version;

	// versionアップをGameに知らせる
	bool m_needWorldUpdate;

	// Storyを画面暗転の状態からスタートならtrue
	bool m_initDark;

	// 世界の寿命が来たらtrue
	bool m_worldEndFlag;

	// 進行中のイベント
	Event* m_nowEvent;
	
	// クリア必須イベント
	std::vector<Event*> m_eventList;

	// イベントのクリア状況 Gameクラスからもらう
	EventData* m_eventData_p;

	// キャラクターのデータ
	CharacterLoader* m_characterLoader;

	// オブジェクトのデータ
	ObjectLoader* m_objectLoader;

public:
	Story(int loop, int time, World* world, SoundPlayer* soundPlayer, EventData* eventData, int worldLifespan, int maxVersion);
	~Story();

	// csvファイルを読み込む
	void loadEventCsvData(const char* fileName, World* world, SoundPlayer* soundPlayer, int versionTimeSpan);
	void loadVersionCsvData(const char* fileName, World* world, SoundPlayer* soundPlayer);

	void debug(int x, int y, int color);

	bool play(int worldLifespan, int maxVersion, int timeSpeed);

	// イベントの発火確認
	void checkFire();

	// ハートのスキル発動が可能かどうか
	bool skillAble();

	void updateWorldVersion();

	// ループ実行時の初期化（実行直後の1Fの描画のために使う。）
	void loopInit();

	// ゲッタ
	inline const Timer* getTimer() const { return m_timer; }
	inline int getDate() const { return m_date; }
	inline int getVersion() const { return m_version; }
	inline bool getNeedWorldUpdate() const { return m_needWorldUpdate; }
	inline bool getInitDark() const { return m_initDark; }
	inline CharacterLoader* getCharacterLoader() const { return m_characterLoader; }
	inline ObjectLoader* getObjectLoader() const { return m_objectLoader; }
	inline const World* getWorld() const { return m_world_p; }
	int getBackPrevSave() const;
	int getLoop() const { return m_loop; }

	// セッタ
	void setWorld(World* world);
	inline void doneWorldUpdate() { m_needWorldUpdate = false; }

	// 今イベント中か
	bool eventNow() const { return m_nowEvent != nullptr; }

	// 前のセーブポイントへ戻ったことを教えてもらう
	void doneBackPrevSave();

private:
	int calcVersion(int time, int worldLifespan, int maxVersion);
	int calcDate(int time, int worldLifespan);

};


#endif