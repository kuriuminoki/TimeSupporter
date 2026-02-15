#ifndef CHACACTER_ACTION_H_INCLUDED
#define CHACACTER_ACTION_H_INCLUDED


#include <vector>
#include <string>


class Character;
class Object;
class SoundPlayer;


// キャラクターの状態
enum class CHARACTER_STATE {
	STAND,	// 何もしていない
	DAMAGE,	// ダメージ受け中 着地で解除
	PREJUMP,	// ジャンプ前
	SQUAT,	// しゃがみ状態
	INIT,	// ボスの初期アニメーション
};


/*
* キャラクターを動かすクラス
* Controllerが使用する。それ以外のインスタンスから使われることはない。
*/
class CharacterAction {
protected:

	// 時間計測
	int m_cnt;

	// サウンドプレイヤー
	SoundPlayer* m_soundPlayer_p;

	// キャラの状態
	CHARACTER_STATE m_state;

	// 動かすキャラクター
	Character* m_character_p;

	// 前のフレームのleftDirection
	bool m_prevLeftDirection;

	// キャラのバージョン イベントでrunSpeedの変更した場合に対処するため
	int m_characterVersion;
	int m_characterMoveSpeed;

	// キャラが地面にいる
	bool m_grand;

	// 右肩下がりの坂にいる
	bool m_grandRightSlope;
	
	// 左肩下がりの坂にいる
	bool m_grandLeftSlope;

	// キャラが走っていないなら-1 そうでないなら走ったフレーム数
	int m_runCnt;

	// ジャンプ前の動作
	int m_preJumpCnt;

	// ジャンプのため時間の最大
	const int PRE_JUMP_MAX = 10;

	// 着地モーションの残り時間
	int m_landCnt;

	// 着地モーションの総時間
	const int LAND_TIME = 10;

	// ブーストアニメの残り時間 または受け身状態
	int m_boostCnt;
	int m_boostDone;// 0:none 1:right 2:left
	const int BOOST_TIME = 30;
	const int BOOST_SPEED = 6;

	// ステップ
	int m_stepCnt;
	int m_stepDone;// 0:none 1:right 2:left
	const int STEP_STOP_TIME = 10;
	const int STEP_TIME = 20;
	const int STEP_SPEED = 30;

	// スライディング
	int m_slidingCnt;
	int m_slidingDone;// 0:none 1:right 2:left
	const int SLIDING_SPEED = 30;

	// やられ状態の時間
	const int DAMAGE_TIME = 10;

	// ノックバックなしのキャラならtrue
	bool m_heavy = false;

	// 移動中
	bool m_moveRight;
	bool m_moveLeft;
	bool m_moveUp;
	bool m_moveDown;

	// 速度
	int m_vx; // キャラの横移動の速度（トータル）
	int m_vy; // キャラの縦移動の速度（トータル）
	int m_runVx; // Controllerによる横移動の速度（走りなど。ダメージによる吹っ飛びは除く）
	int m_runVy;

	// 他のキャラと重なっているため次のフレームで位置をずらす
	int m_dx;

	// 移動のロック（オブジェクト等で動けない方向はtrue）
	bool m_rightLock;
	bool m_leftLock;
	bool m_upLock;
	bool m_downLock;

	// 射撃用カウント
	int m_bulletCnt;

	// 斬撃用カウント
	int m_slashCnt;

	// ジャンプ用カウント
	int m_jumpCnt;

	// 攻撃する方向
	bool m_attackLeftDirection;

	int m_damageCnt;

protected:

	// actionから呼び出す
	virtual void bulletAction();
	virtual void slashAction();
	virtual void damageAction();
	virtual void otherAction();
	virtual void moveAction();

	// キャラの画像を変更
	virtual void switchHandle() = 0;

	// 画像のサイズ変更による位置調整
	void afterChangeGraph(int beforeX1, int afterX1, int beforeY1, int afterY1, int beforeX2, int afterX2, int beforeY2, int afterY2);

public:

	static const char* ACTION_NAME;
	virtual const char* getActionName() const { return this->ACTION_NAME; }

	CharacterAction();
	CharacterAction(Character* character, SoundPlayer* soundPlayer_p);
	virtual ~CharacterAction() {}

	// コピー作成
	virtual CharacterAction* createCopy(std::vector<Character*> characters) = 0;

	// コピー作成用
	void setParam(CharacterAction* action);

	// デバッグ
	void debugAction(int x, int y, int color) const;
	virtual void debug(int x, int y, int color) const = 0;

	// ゲッタ
	inline const Character* getCharacter() const { return m_character_p; }
	inline CHARACTER_STATE getState() const { return m_state; }
	inline bool getGrand() const { return m_grand; }
	inline bool getGrandRightSlope() const { return m_grandRightSlope; }
	inline bool getGrandLeftSlope() const { return m_grandLeftSlope; }
	inline bool getHeavy() const { return m_heavy; }
	inline int getVx() const { return m_vx + m_runVx; }
	inline int getVy() const { return m_vy + m_runVy; }
	inline int getDx() const { return m_dx; }
	inline int getSlashCnt() const { return m_slashCnt; }
	inline int getBulletCnt() const { return m_bulletCnt; }
	inline int getSlidingDone() const { return m_slidingDone; }
	bool getRightLock() const { return m_rightLock; }
	bool getLeftLock() const { return m_leftLock; }
	bool getUpLock() const { return m_upLock; }
	bool getDownLock() const { return m_downLock; }
	const SoundPlayer* getSoundPlayer() const { return m_soundPlayer_p; }
	inline int getPreJumpCnt() const { return m_preJumpCnt; }
	virtual int getPreJumpMax() const { return PRE_JUMP_MAX; }

	// セッタ
	inline void setCnt(int cnt) { m_cnt = cnt; }
	void setState(CHARACTER_STATE state);
	inline void setCharacterVersion(int version) { m_characterVersion = version; }
	inline void setCharacterMoveSpeed(int moveSpeed) { m_characterMoveSpeed = moveSpeed; }
	inline void setSimpleGrand(bool grand) { m_grand = grand; }
	virtual void setGrand(bool grand);
	void setRightLock(bool lock);
	void setLeftLock(bool lock);
	void setUpLock(bool lock);
	void setDownLock(bool lock);
	virtual void setBoost(bool leftDirection);
	void finishBoost();
	void setStep(bool leftDirection);
	void finishStep();
	void setSliding(bool leftDirection);
	void finishSliding();
	inline void setGrandRightSlope(bool grand) { m_grandRightSlope = grand; }
	inline void setGrandLeftSlope(bool grand) { m_grandLeftSlope = grand; }
	inline void setRunCnt(int runCnt) { m_runCnt = runCnt; }
	inline void setPreJumpCnt(int preJumpCnt) { m_preJumpCnt = preJumpCnt; }
	inline void setMoveRight(bool moveRight) { m_moveRight = moveRight; }
	inline void setMoveLeft(bool moveLeft) { m_moveLeft = moveLeft; }
	inline void setMoveUp(bool moveUp) { m_moveUp = moveUp; }
	inline void setMoveDown(bool moveDown) { m_moveDown = moveDown; }
	inline void setVx(int vx) { m_vx = vx; }
	inline void setVy(int vy) { m_vy = vy; }
	inline void setRunVx(int run_vx) { m_runVx = run_vx; }
	inline void setRunVy(int run_vy) { m_runVy = run_vy; }
	inline void setDx(int dx) { m_dx = dx; }
	inline void setBulletCnt(int bulletCnt) { m_bulletCnt = bulletCnt; }
	inline void setSlashCnt(int slashCnt) { m_slashCnt = slashCnt; }
	inline void setJumpCnt(int jumpCnt) { m_jumpCnt = jumpCnt; }
	inline void setAttackLeftDirection(bool attackLeftDirection) { m_attackLeftDirection = attackLeftDirection; }
	inline void setLandCnt(int landCnt) { m_landCnt = landCnt; }
	inline void setBoostCnt(int boostCnt) { m_boostCnt = boostCnt; }
	inline void setBoostDone(int boostDone) { m_boostDone = boostDone; }
	inline void setStepCnt(int stepCnt) { m_stepCnt = stepCnt; }
	inline void setStepDone(int stepDone) { m_stepDone = stepDone; }
	inline void setSlidingCnt(int slidingCnt) { m_slidingCnt = slidingCnt; }
	inline void setSlidingDone(int slidingDone) { m_slidingDone = slidingDone; }
	inline void setDamageCnt(int damageCnt) { m_damageCnt = damageCnt; }
	inline void setHeavy(bool heavy) { m_heavy = heavy; }
	inline void setSoundPlayer(SoundPlayer* soundPlayer) { m_soundPlayer_p = soundPlayer; }

	// 今ダメージを受けていて動けない
	inline bool damageFlag() const { return m_state == CHARACTER_STATE::DAMAGE; }

	// squat==trueならしゃがむ、falseなら立つ
	virtual void setSquat(bool squat);

	// キャラクターのセッタ
	void setCharacterX(int x);
	void setCharacterY(int y);
	void setCharacterLeftDirection(bool leftDirection);
	void setCharacterFreeze(bool freeze);
	void setDeadFlag(bool deadFlag);

	// 行動前の処理 毎フレーム行う
	virtual void init();

	// 物理演算 毎フレーム行う
	virtual void action();

	// 移動 引数は４方向分 キャラによっては斜めに移動できるため。
	virtual void move(bool right, bool left, bool up, bool down) = 0;

	// ジャンプ rate%の力で飛び上がる。
	virtual void jump(int rate) = 0;

	// 射撃攻撃
	virtual std::vector<Object*>* bulletAttack(int gx, int gy) = 0;

	// 斬撃攻撃
	virtual std::vector<Object*>* slashAttack(int gx, int gy) = 0;

	// スライディング攻撃
	virtual std::vector<Object*>* slidingAttack() { return nullptr; }

	// ダメージ 必要に応じてオーバーライド
	virtual void damage(int vx, int vy, int damageValue);

	// 射撃開始の処理 必要に応じてオーバーライド
	virtual void startBullet();

	// 射撃終了の処理 必要に応じてオーバーライド
	virtual void finishBullet();

	// 斬撃開始の処理 必要に応じてオーバーライド
	virtual void startSlash();

	// 斬撃終了の処理 必要に応じてオーバーライド
	virtual void finishSlash();

	// 今無敵時間じゃない
	virtual bool ableDamage() const;

	// 今攻撃可能状態
	virtual bool ableAttack() const;

	// 今歩ける状態
	virtual bool ableWalk() const;

	// 方向転換可能 FreezeAI用
	virtual bool ableChangeDirection() const;

	// 歩き始める
	void startMoveLeft();
	void startMoveRight();
	void startMoveUp();
	void startMoveDown();

	// 歩くのをやめる
	void stopMoveLeft();
	void stopMoveRight();
	void stopMoveUp();
	void stopMoveDown();

	// キャラのストップ時間を消費
	void consumeStopCnt();

	// キャラをストップ
	void stopCharacter(int cnt);

};


// クラス名からCharacterActionを作成する関数
CharacterAction* createAction(const std::string actionName, Character* character, SoundPlayer* soundPlayer_p);


/*
* 空を飛ばない普通の棒人間
*/
class StickAction :
	public CharacterAction 
{
private:
	// 重力加速度
	const int G = 1;

protected:
	// 横へ歩く 引数は右と左の２つ
	void walk(bool right, bool left);

	// キャラの画像を状態(state)に応じて変更
	void switchHandle();

public:
	static const char* ACTION_NAME;
	const char* getActionName() const { return this->ACTION_NAME; }

	StickAction(Character* character, SoundPlayer* soundPlayer_p);

	CharacterAction* createCopy(std::vector<Character*> characters);

	void debug(int x, int y, int color) const;

	// 物理演算 毎フレーム行う
	void action();

	// 移動 引数は４方向分
	void move(bool right, bool left, bool up, bool down);

	// ジャンプ cntフレーム目
	void jump(int cnt);

	// 射撃攻撃
	std::vector<Object*>* bulletAttack(int gx, int gy);

	// 斬撃攻撃
	std::vector<Object*>* slashAttack(int gx, int gy);

	// スライディング攻撃
	std::vector<Object*>* slidingAttack();
};


/*
* ヴァルキリア用Action
*/
class ValkiriaAction :
	public StickAction
{
private:
	// ジャンプのため時間の最大
	const int PRE_JUMP_MAX = 30;

	// 斬撃攻撃による移動速度
	const int SLASH_MOVE_SPEED = 25;

	bool m_slashNow;

public:
	static const char* ACTION_NAME;
	const char* getActionName() const { return this->ACTION_NAME; }

	ValkiriaAction(Character* character, SoundPlayer* soundPlayer_p);

	CharacterAction* createCopy(std::vector<Character*> characters);

	void debug(int x, int y, int color) const;

	int getPreJumpMax() const { return PRE_JUMP_MAX; }

	inline void setSlashNow(bool slashNow) { m_slashNow = slashNow; }
	void setGrand(bool grand);

	// 斬撃開始の処理
	void startSlash();

	// 斬撃終了の処理
	void finishSlash();

	// ダメージ
	void damage(int vx, int vy, int damageValue);
};


/*
* 空を飛ぶキャラ
*/
class FlightAction :
	public CharacterAction
{
protected:

	void damageAction();
	void otherAction();

	// キャラの画像を状態(state)に応じて変更
	void switchHandle();

	void walk(bool right, bool left, bool up, bool down);

public:
	static const char* ACTION_NAME;
	const char* getActionName() const { return this->ACTION_NAME; }

	FlightAction(Character* character, SoundPlayer* soundPlayer_p);

	CharacterAction* createCopy(std::vector<Character*> characters);

	void debug(int x, int y, int color) const;

	void setSquat(bool squat) {}

	void action();

	// 移動 引数は４方向分
	void move(bool right, bool left, bool up, bool down);

	// ジャンプ cntフレーム目
	void jump(int cnt);

	void setBoost(bool leftDirection);

	// 射撃攻撃
	std::vector<Object*>* bulletAttack(int gx, int gy);

	// 斬撃攻撃
	std::vector<Object*>* slashAttack(int gx, int gy);

protected:
	void flightAction();

};


/*
* コハル用Action
*/
class KoharuAction :
	public StickAction
{
private:

	// 射撃攻撃による移動速度
	const int BULLET_MOVE_SPEED = 2;

public:

	static const char* ACTION_NAME;
	const char* getActionName() const { return this->ACTION_NAME; }

	KoharuAction(Character* character, SoundPlayer* soundPlayer_p);

	CharacterAction* createCopy(std::vector<Character*> characters);

	void debug(int x, int y, int color) const;

	// 射撃攻撃
	std::vector<Object*>* bulletAttack(int gx, int gy);

	void startBullet();

	void finishBullet();

	bool ableAttack() const;

	bool ableWalk() const;

};


/*
* 行動開始前のBoss
*/
class BossFreezeAction :
	public CharacterAction
{
protected:

	// キャラの画像を変更
	void switchHandle();

public:

	static const char* ACTION_NAME;
	virtual const char* getActionName() const { return this->ACTION_NAME; }

	BossFreezeAction(Character* character, SoundPlayer* soundPlayer_p);

	// コピー作成
	CharacterAction* createCopy(std::vector<Character*> characters);

	// デバッグ
	void debug(int x, int y, int color) const { }
	// 行動前の処理 毎フレーム行う
	void init() { }

	// 物理演算 毎フレーム行う
	void action() { switchHandle(); }

	// 移動 引数は４方向分 キャラによっては斜めに移動できるため。
	void move(bool right, bool left, bool up, bool down) { }

	// ジャンプ rate%の力で飛び上がる。
	void jump(int rate) { }

	// 射撃攻撃
	std::vector<Object*>* bulletAttack(int gx, int gy) { return nullptr; }

	// 斬撃攻撃
	std::vector<Object*>* slashAttack(int gx, int gy) { return nullptr; }

	// ダメージ 必要に応じてオーバーライド
	void damage(int vx, int vy, int damageValue) { }

	// 射撃開始の処理 必要に応じてオーバーライド
	void startBullet() { }

	// 射撃終了の処理 必要に応じてオーバーライド
	void finishBullet() { }

	// 斬撃開始の処理 必要に応じてオーバーライド
	void startSlash() { }

	// 斬撃終了の処理 必要に応じてオーバーライド
	void finishSlash() { }

	// 今無敵時間じゃない
	bool ableDamage() const { return false; }

	// 今攻撃可能状態
	bool ableAttack() const { return false; }

	// 今歩ける状態
	bool ableWalk() const { return false; }

	// 方向転換可能 FreezeAI用
	bool ableChangeDirection() const { return false; }

};


/*
* Boss1: サン
*/
class SunAction :
	public FlightAction
{
private:

	// ボスの初期アニメーションカウント用
	int m_initCnt;
	const int NOT_HIDE_CNT = 80;

	// 初期体力
	int m_initHp;

	// 無敵状態
	bool m_hideFlag;

	int m_startAnimeCnt;

public:
	static const char* ACTION_NAME;
	const char* getActionName() const { return this->ACTION_NAME; }

	SunAction(Character* character, SoundPlayer* soundPlayer_p, bool duplicationFlag);

	CharacterAction* createCopy(std::vector<Character*> characters);

	// セッタ
	inline void setInitCnt(int initCnt) { m_initCnt = initCnt; }
	inline void setInitHp(int initHp) { m_initHp = initHp; }
	inline void setHideFlag(int hideFlag) { m_hideFlag = hideFlag; }
	inline void setStartAnimeCnt(int startAnimeCnt) { m_startAnimeCnt = startAnimeCnt; }

	void action();

protected:

	// 状態に応じて画像セット
	void switchHandle();

};


/*
* Boss2: アーカイブ
*/
class ArchiveAction :
	public StickAction
{
private:

	// 最初のHP表示アニメ用 falseの間は動かない
	bool m_initCompFlag;

	// 初期体力
	int m_initHp;

	// 斬撃攻撃による移動速度
	const int SLASH_MOVE_SPEED = 80;

	int m_slashVx;

public:
	static const char* ACTION_NAME;
	const char* getActionName() const { return this->ACTION_NAME; }

	ArchiveAction(Character* character, SoundPlayer* soundPlayer_p, bool duplicationFlag);

	CharacterAction* createCopy(std::vector<Character*> characters);

	// セッタ
	inline void setInitCompFlag(bool initCompFlag) { m_initCompFlag = initCompFlag; }
	inline void setInitHp(int initHp) { m_initHp = initHp; }
	inline void setSlashVx(int slashVx) { m_slashVx = slashVx; }

	void action();

	void startSlash();
	void finishSlash();

	bool ableDamage() const;
};


#endif