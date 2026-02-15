#ifndef STORY_H_INCLUDED
#define STORY_H_INCLUDED

#include <string>
#include <vector>

class Event;
class EventData;
class GameData;
class World;
class SoundPlayer;
class CharacterLoader;
class ObjectLoader;

enum class EVENT_RESULT;


// ストーリ－
class Story {
private:

	int m_storyNum;

	GameData* m_gameData_p;

	// 対象の世界
	World* m_world;

	SoundPlayer* m_soundPlayer_p;

	// 時間帯
	int m_date;

	// Storyを画面暗転の状態からスタートならtrue
	bool m_initDark;

	// 進行中のイベント
	Event* m_nowEvent;
	
	// クリア必須イベント
	std::vector<Event*> m_eventList;

	// キャラクターのデータ
	CharacterLoader* m_characterLoader;

	// オブジェクトのデータ
	ObjectLoader* m_objectLoader;

public:
	Story(int storyNum, GameData* gameData, SoundPlayer* soundPlayer);
	~Story();

	void debug(int x, int y, int color);

	EVENT_RESULT play();

	// イベントの発火確認
	void checkFire();

	// ゲッタ
	inline int getStoryNum() const { return m_storyNum; }
	inline int getDate() const { return m_date; }
	inline bool getInitDark() const { return m_initDark; }
	inline CharacterLoader* getCharacterLoader() const { return m_characterLoader; }
	inline ObjectLoader* getObjectLoader() const { return m_objectLoader; }
	inline const World* getWorld() const { return m_world; }

	// セッタ
	void setWorld(World* world);

	// 今イベント中か
	bool eventNow() const { return m_nowEvent != nullptr; }

	// 前のセーブポイントへ戻ったことを教えてもらう
	void doneBackPrevSave();

private:
	int calcVersion(int time, int worldLifespan, int maxVersion);
	int calcDate(int time, int worldLifespan);

};


#endif