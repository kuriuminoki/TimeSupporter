#ifndef EVENT_H_INCLUDED
#define EVENT_H_INCLUDED

#include <vector>
#include <string>
#include <map>


class World;
class SoundPlayer;
class CharacterController;
class Character;
class Conversation;
class Movie;


enum class EVENT_RESULT {
	NOW,			// 進行中
	SUCCESS,		// クリア
	FAILURE			// 失敗
};


/*
* イベントの発火条件 Worldに変化を加えない
*/
class EventFire {
protected:

	// イベントが起きるWorld
	const World* m_world_p;

public:
	EventFire(World* world);
	virtual ~EventFire() {}

	virtual bool fire() = 0;

	virtual void setWorld(World* world) { m_world_p = world; }
};


/*
* イベントを構成する要素
*/
class EventElement {
protected:

	// イベントが起きているWorld
	World* m_world_p;

public:
	EventElement(World* world);
	virtual ~EventElement();

	// 初期化
	virtual void init(){}

	// プレイ
	virtual EVENT_RESULT play() = 0;

	// ハートのスキル発動が可能かどうか
	virtual bool skillAble() { return false; }

	// クリア時に前のセーブポイントへ戻る必要があるか
	virtual int needBackPrevSave() { return 0; }

	// セッタ
	virtual void setWorld(World* world) { m_world_p = world; }
};


/*
* イベント
* EventElementを管理する
*/
class Event {
private:

	// 発火後にこれを使ってElement生成
	World* m_world_p;
	SoundPlayer* m_soundPlayer_p;

	// イベント番号
	int m_eventNum;

	int m_startTime;
	int m_endTime;

	// 前提としてクリアが必要なイベントの番号
	std::vector<int> m_requireEventNum;

	// イベントの発火条件
	std::vector<EventFire*> m_eventFire;

	// イベントの要素
	EventElement* m_eventElement;

	// イベントの内容
	std::vector<std::map<std::string, std::string> > m_elementsData;

	// イベントの進捗(EventElementのインデックス)
	int m_nowElement;

	// 前のセーブポイントへ戻る要求 戻るならいくつ戻るか返す(>0)
	int m_backPrevSave;

	// 世界のバージョン
	int m_version;

public:
	Event(int eventNum, int startTime, int endTime, std::vector<int> requireEventNum, World* world, SoundPlayer* soundPlayer, int version);
	~Event();

	// ゲッタ
	inline int getEventNum() const { return m_eventNum; }
	inline int getStartTime() const { return m_startTime; }
	inline int getEndTime() const { return m_endTime; }
	inline const std::vector<int> getRequireEventNum() const { return m_requireEventNum; }
	inline void setVersion(int version) { m_version = version; }

	// 発火
	bool fire();

	// イベント進行
	EVENT_RESULT play();

	// 今ハートのスキル発動可能かどうか
	bool skillAble();

	// Worldを設定しなおす
	void setWorld(World* world);

	int getBackPrevSave() const { return m_backPrevSave; }

	// 前のセーブポイントへ戻ったことを教えてもらう
	void doneBackPrevSave() { m_backPrevSave = 0; }

private:
	void createFire(std::vector<std::string> param, World* world, SoundPlayer* soundPlayer);
	void createElement(std::vector<std::string> param, World* world, SoundPlayer* soundPlayer);
};


/*
* EventFireの派生クラス
*/
// 特定のキャラが指定した座標にいる
class CharacterPointFire :
	public	EventFire
{
private:
	// パラメータ
	std::vector<std::string> m_param;

	// キャラ
	Character* m_character_p;

	// エリア番号
	int m_areaNum;

	// 目標座標
	int m_x, m_y;

	// 座標のずれの許容
	int m_dx, m_dy;

public:
	CharacterPointFire(World* world, std::vector<std::string> param);

	bool fire();

	// 世界を設定しなおす
	void setWorld(World* world);
};

// 特定のキャラが特定のキャラの近くにいる
class CharacterNearFire :
	public	EventFire
{
private:
	// パラメータ
	std::vector<std::string> m_param;

	// キャラ
	Character* m_character_p;

	// 今いるエリア番号　エリア移動を知る用
	int m_areaNum;

	// 子のキャラの近くに行くのが条件
	Character* m_target_p;

	// 座標のずれの許容
	int m_dx, m_dy;

public:
	CharacterNearFire(World* world, std::vector<std::string> param);

	bool fire();

	// 世界を設定しなおす
	void setWorld(World* world);
};

// 勝手に発火
class AutoFire :
	public EventFire
{
public:

	AutoFire(World* world_p) :
		EventFire(world_p)
	{

	}

	bool fire() { return true; }

};

// 発火しない　デバッグ用
class NonFire :
	public EventFire
{
public:

	NonFire(World* world_p) :
		EventFire(world_p)
	{

	}

	bool fire() { return false; }
};


/*
* EventElementの派生クラス
*/
// エリア移動を禁止する
class LockAreaEvent :
	public EventElement
{
private:

	// 禁止ならtrue
	bool m_lock;

public:
	LockAreaEvent(World* world, std::vector<std::string> param);

	// プレイ
	EVENT_RESULT play();

};


// 操作キャラ変更を禁止する
class LockControlCharacterEvent :
	public EventElement
{
private:

	// 禁止ならtrue
	bool m_lock;

public:
	LockControlCharacterEvent(World* world, std::vector<std::string> param);

	// プレイ
	EVENT_RESULT play();

};


// キャラを無敵にする
class InvincibleEvent :
	public EventElement
{
private:

	// パラメータ
	std::vector<std::string> m_param;

	// trueなら無敵にする
	bool m_invincible;

	// 対象のキャラ
	Character* m_character_p;

public:
	InvincibleEvent(World* world, std::vector<std::string> param);

	// プレイ
	EVENT_RESULT play();

	// 世界を設定しなおす
	void setWorld(World* world);
};

// キャラの目標地点を設定を変える
class SetGoalPointEvent :
	public EventElement
{
private:

	// パラメータ
	std::vector<std::string> m_param;

	// 目標地点
	int m_gx, m_gy;

	// 対象のキャラ
	CharacterController* m_controller_p;

public:
	SetGoalPointEvent(World* world, std::vector<std::string> param);

	// プレイ
	EVENT_RESULT play();

	// 世界を設定しなおす
	void setWorld(World* world);
};

// 全キャラが目標地点へ移動するまで待機
class MoveGoalEvent :
	public EventElement
{
private:

	// パラメータ
	std::vector<std::string> m_param;

public:
	MoveGoalEvent(World* world, std::vector<std::string> param);

	// プレイ
	EVENT_RESULT play();

};

// キャラのAIを変える
class ChangeActionEvent :
	public EventElement
{
private:

	// パラメータ
	std::vector<std::string> m_param;

	// Actionのクラス名
	std::string m_actionName;

	// 対象のキャラ
	Character* m_character_p;

public:
	ChangeActionEvent(World* world, std::vector<std::string> param);

	// プレイ
	EVENT_RESULT play();

	// 世界を設定しなおす
	void setWorld(World* world);
};

// キャラのAIを変える
class ChangeBrainEvent :
	public EventElement
{
private:

	// パラメータ
	std::vector<std::string> m_param;

	// Brainのクラス名
	std::string m_brainName;

	// 対象のキャラ
	CharacterController* m_controller_p;

public:
	ChangeBrainEvent(World* world, std::vector<std::string> param);

	// プレイ
	EVENT_RESULT play();

	// 世界を設定しなおす
	void setWorld(World* world);
};

// キャラのGroupIDを変える
class ChangeGroupEvent :
	public EventElement
{
private:

	// パラメータ
	std::vector<std::string> m_param;

	int m_groupId;

	// 対象のキャラ
	Character* m_character_p;

public:
	ChangeGroupEvent(World* world, std::vector<std::string> param);

	// プレイ
	EVENT_RESULT play();

	// 世界を設定しなおす
	void setWorld(World* world);
};

// キャラのversionを変える
class ChangeInfoVersionEvent :
	public EventElement
{
private:

	// パラメータ
	std::vector<std::string> m_param;

	int m_version;

	// 対象のキャラ
	Character* m_character_p;

public:
	ChangeInfoVersionEvent(World* world, std::vector<std::string> param);

	// プレイ
	EVENT_RESULT play();

	// 世界を設定しなおす
	void setWorld(World* world);
};

// キャラのbossFlagを変える
class ChangeBossFlagEvent :
	public EventElement
{
private:

	// パラメータ
	std::vector<std::string> m_param;

	bool m_bossFlag;

	// 対象のキャラ
	Character* m_character_p;

public:
	ChangeBossFlagEvent(World* world, std::vector<std::string> param);

	// プレイ
	EVENT_RESULT play();

	// 世界を設定しなおす
	void setWorld(World* world);
};

// キャラの座標を変える
class ChangeCharacterPointEvent :
	public EventElement
{
private:

	// パラメータ
	std::vector<std::string> m_param;

	int m_x, m_y;

	// 対象のキャラ
	Character* m_character_p;

public:
	ChangeCharacterPointEvent(World* world, std::vector<std::string> param);

	// プレイ
	EVENT_RESULT play();

	// 世界を設定しなおす
	void setWorld(World* world);
};

// キャラの向きを変える
class ChangeCharacterDirectionEvent :
	public EventElement
{
private:

	// パラメータ
	std::vector<std::string> m_param;

	// 左を向くならtrue
	bool m_leftDirection;

	// 子のキャラがいる方向へ向く いないならnullptr
	Character* m_targetCharacter_p;

	// 対象のキャラ
	Character* m_character_p;

public:
	ChangeCharacterDirectionEvent(World* world, std::vector<std::string> param);

	// プレイ
	EVENT_RESULT play();

	// 世界を設定しなおす
	void setWorld(World* world);
};

// 特定のキャラのHPが0になるまで戦う
class DeadCharacterEvent :
	public EventElement
{
private:

	// パラメータ
	std::vector<std::string> m_param;

	// 対象のキャラ
	Character* m_character_p;

public:
	DeadCharacterEvent(World* world, std::vector<std::string> param);

	// プレイ
	EVENT_RESULT play();

	// 世界を設定しなおす
	void setWorld(World* world);

	// ハートのスキル発動が可能かどうか
	bool skillAble() { return true; }
};

// 特定のグループが全滅するまで戦う
class DeadGroupEvent :
	public EventElement
{
private:

	// 対象のグループ
	int m_groupId;

	// エリア番号
	int m_areaNum;

public:
	DeadGroupEvent(World* world, std::vector<std::string> param);

	// プレイ
	EVENT_RESULT play();

	// ハートのスキル発動が可能かどうか
	bool skillAble() { return true; }
};

// 会話イベント
class TalkEvent :
	public EventElement
{
private:

	Conversation* m_conversation;

public:
	TalkEvent(World* world, SoundPlayer* soundPlayer, std::vector<std::string> param);
	~TalkEvent();

	// プレイ
	EVENT_RESULT play();

	// セッタ
	void setWorld(World* world);
};

// ムービーイベント
class MovieEvent :
	public EventElement
{
private:

	Movie* m_movie;

public:
	MovieEvent(World* world, SoundPlayer* soundPlayer, std::vector<std::string> param);
	~MovieEvent();

	// 初期化
	void init();

	// プレイ
	EVENT_RESULT play();
};

// 特定のエリアでプレイヤーがやられるイベント
class PlayerDeadEvent :
	public EventElement
{
private:
	
	int m_areaNum;

	int m_backPrevSave;

public:
	PlayerDeadEvent(World* world, std::vector<std::string> param);

	// プレイ
	EVENT_RESULT play();

	// クリア時に前のセーブポイントへ戻る必要があるか
	int needBackPrevSave() { return m_backPrevSave; }

	// ハートのスキル発動が可能かどうか
	bool skillAble() { return true; }
};

// 特定のエリアへ強制的に移動する
class MoveAreaEvent :
	public EventElement
{
private:
	
	int m_areaNum;

public:
	MoveAreaEvent(World* world, std::vector<std::string> param);

	// プレイ
	EVENT_RESULT play();
};

// 世界の描画をする・しない
class BlindWorldEvent :
	public EventElement
{
private:

	int m_flag;

public:
	BlindWorldEvent(World* world, std::vector<std::string> param);

	// プレイ
	EVENT_RESULT play();
};

class ChangeControlCharacterEvent :
	public EventElement
{
private:

	// 変更先のキャラ名
	std::string m_name;

public:

	ChangeControlCharacterEvent(World* world, std::vector<std::string> param);

	// プレイ
	EVENT_RESULT play();
};

// キャラの追加
class PushCharacterEvent :
	public EventElement
{
private:

	// 追加するキャラの情報
	std::string m_name;
	int m_x, m_y;
	bool m_sound;
	int m_groupId;
	std::string m_action;
	std::string m_brain;
	std::string m_controller;
	int m_version;

public:
	PushCharacterEvent(World* world, std::vector<std::string> param, int version);

	// プレイ
	EVENT_RESULT play();
};

// 待機
class WaitEvent :
	public EventElement
{
	int m_cnt;
	int m_time;
public:
	WaitEvent(World* world, std::vector<std::string> param);

	// プレイ
	EVENT_RESULT play();
};

// スキル発動まで戦闘を続けるイベント
class WaitSkillEvent :
	public EventElement
{
private:

	bool m_skillFlag;

public:
	WaitSkillEvent(World* world, std::vector<std::string> param);

	// プレイ
	EVENT_RESULT play();

	// ハートのスキル発動が可能かどうか
	bool skillAble() { return true; }
};

class SetBgmEvent :
	public EventElement
{
	std::string m_filePath;

public:

	SetBgmEvent(World* world, std::vector<std::string> param);

	// プレイ
	EVENT_RESULT play();
};

// 永遠にbattle テスト用
class BattleForever :
	public EventElement
{
public:
	BattleForever(World* world, std::vector<std::string> param);

	// プレイ
	EVENT_RESULT play();

	// ハートのスキル発動が可能かどうか
	bool skillAble() { return true; }
};

#endif