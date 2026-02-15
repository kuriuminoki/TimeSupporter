#ifndef BRAIN_H_INCLUDED
#define BRAIN_H_INCLUDED

#include <vector>
#include <string>


class Character;
class CharacterAction;
class Camera;


// Controllerに命令するクラス（キーボード＆マウスやＡＩ）
class Brain {
protected:
	// 今のキャラの状態を考慮して射撃の目標座標や次の行動を決めるため必要
	const CharacterAction* m_characterAction_p;

public:
	static const char* BRAIN_NAME;
	virtual const char* getBrainName() const { return this->BRAIN_NAME; }

	Brain();
	virtual ~Brain(){}

	virtual Brain* createCopy(std::vector<Character*> characters, const Camera* camera) = 0;

	virtual void debug(int x, int y, int color) const = 0;

	// 話しかけたり扉入ったり
	virtual bool actionOrder() { return false; }

	// セッタ
	virtual void setGx(int gx) { }
	virtual void setGy(int gy) { }
	virtual void setCharacterAction(const CharacterAction* characterAction) = 0;

	// 遠距離攻撃の目標座標
	virtual void bulletTargetPoint(int& x, int& y) = 0;

	// 近距離攻撃の目標座標
	virtual void slashTargetPoint(int& x, int& y) = 0;

	// 移動（上下左右の入力）
	virtual void moveOrder(int& right, int& left, int& up, int& down) = 0;

	// ジャンプの制御
	virtual int jumpOrder() = 0;

	// しゃがみの制御
	virtual int squatOrder() = 0;

	// 近距離攻撃
	virtual int slashOrder() = 0;

	// 遠距離攻撃
	virtual int bulletOrder() = 0;

	// 目標地点へ移動するだけ 達成済みならtrueで何もしない
	virtual bool moveGoalOrder(int& right, int& left, int& up, int& down, int& jump) { return true; }

	virtual void setGoalToTarget() = 0;

	// 斬撃攻撃を振る最大距離
	virtual int getSlashReach() const { return 0; }

	// 攻撃対象を決める(AIクラスでオーバライドする。)
	virtual void searchTarget(const Character* character) { }

	// 攻撃対象を変更する必要があるならtrueでアピールする(AIクラスでオーバライドする)。
	virtual bool needSearchTarget() const { return false; }

	// 追跡対象を決める(AIクラスでオーバライドする。)
	virtual void searchFollow(const Character* character) { }

	// 追跡対象を変更する必要があるならtrueでアピールする(AIクラスでオーバライドする)。
	virtual bool needSearchFollow() const { return false; }

	// 攻撃対象や追跡対象の情報を取得
	virtual int getTargetId() const { return -1; }
	virtual const char* getTargetName() const { return ""; }
	virtual int getFollowId() const { return -1; }
	virtual const char* getFollowName() const { return ""; }
	virtual const Character* getFollow() const { return nullptr; }

	virtual void setFollow(const Character* character){  }
	virtual void setTarget(const Character* character) {  }

	// 追跡対象の近くにいるか判定
	virtual bool checkAlreadyFollow() { return true; }

};


// クラス名からBrainを作成する関数
Brain* createBrain(const std::string brainName, const Camera* camera_p);


/*
* キーボードでキャラの操作を命令するクラス
*/
class KeyboardBrain :
	public Brain
{
private:
	// カメラ
	const Camera* m_camera_p;

public:
	static const char* BRAIN_NAME;
	const char* getBrainName() const { return this->BRAIN_NAME; }

	KeyboardBrain(const Camera* camera);

	Brain* createCopy(std::vector<Character*> characters, const Camera* camera) { return new KeyboardBrain(camera); }

	void debug(int x, int y, int color) const;

	// セッタ
	inline void setCharacterAction(const CharacterAction* characterAction) { m_characterAction_p = characterAction; }

	void bulletTargetPoint(int& x, int& y);
	void slashTargetPoint(int& x, int& y);
	bool actionOrder();
	void moveOrder(int& right, int& left, int& up, int& down);
	int jumpOrder();
	int squatOrder();
	int slashOrder();
	int bulletOrder();
	void setGoalToTarget() {}
};


/*
* 全く動かないAI
*/
class Freeze :
	public Brain
{
public:
	static const char* BRAIN_NAME;
	const char* getBrainName() const { return this->BRAIN_NAME; }

	Freeze() { }

	Brain* createCopy(std::vector<Character*> characters, const Camera* camera) { return new Freeze(); }

	void debug(int x, int y, int color) const { }

	// セッタ
	void setCharacterAction(const CharacterAction* characterAction) { m_characterAction_p = characterAction; }

	bool actionOrder() { return false; }
	void bulletTargetPoint(int& x, int& y) {  }
	void slashTargetPoint(int& x, int& y) {  }
	void moveOrder(int& right, int& left, int& up, int& down) { right = 0; left = 0; up = 0; down = 0; }
	int jumpOrder() { return 0; }
	int squatOrder() { return 0; }
	int slashOrder() { return 0; }
	int bulletOrder() { return 0; }
	void setGoalToTarget() {}
};


/*
*  普通に敵と戦うよう命令するＡＩのクラス
*/
class NormalAI :
	public Brain
{
protected:
	// 移動用
	int m_rightKey, m_leftKey, m_upKey, m_downKey;

	// ジャンプの長さ
	int m_jumpCnt;

	// しゃがむ長さ
	int m_squatCnt;

	// 攻撃対象を認知する距離
	const int TARGET_DISTANCE = 2000;

	// 攻撃対象
	const Character* m_target_p;

	// 射撃の精度
	const int BULLET_ERROR = 400;

	// 特に意味のない移動をする確率
	const int MOVE_RAND = 59;

	// 移動目標
	int m_gx, m_gy;

	// 移動目標達成とみなす誤差 ±GX_ERROR
	const int GX_ERROR = 100;
	const int GY_ERROR = 50;

	// 移動時間
	int m_moveCnt;

	// 10フレーム前でのX座標
	int m_prevX;

	// 移動を諦めるまでの時間
	const int GIVE_UP_MOVE_CNT = 180;

public:
	static const char* BRAIN_NAME;
	const char* getBrainName() const { return this->BRAIN_NAME; }

	NormalAI();

	Brain* createCopy(std::vector<Character*> characters, const Camera* camera);
	void setParam(NormalAI* brain);

	void debug(int x, int y, int color) const;

	// セッタ
	void setRightKey(int rightKey) { m_rightKey = rightKey; }
	void setLeftKey(int leftKey) { m_leftKey = leftKey; }
	void setUpKey(int upKey) { m_upKey = upKey; }
	void setDownKey(int downKey) { m_downKey = downKey; }
	void setJumpCnt(int cnt) { m_jumpCnt = cnt; }
	void setSquatCnt(int cnt) { m_squatCnt = cnt; }
	void setGx(int gx) { m_gx = gx; }
	void setGy(int gy) { m_gy = gy; }
	void setMoveCnt(int cnt) { m_moveCnt = cnt; }
	void setPrevX(int prevX) { m_prevX = prevX; }
	void setTarget(const Character* character) { m_target_p = character; }
	void setCharacterAction(const CharacterAction* characterAction);

	void bulletTargetPoint(int& x, int& y);
	void slashTargetPoint(int& x, int& y);
	void moveOrder(int& right, int& left, int& up, int& down);
	void moveUpDownOrder(int x, int y, bool& tryFlag);
	int jumpOrder();
	int squatOrder();
	int slashOrder();
	int bulletOrder();
	void setGoalToTarget();

	// 斬撃攻撃を振る最大距離
	int getSlashReach() const { return 800; }

	// 攻撃対象を決める(targetのままか、characterに変更するか)
	void searchTarget(const Character* character);

	// 攻撃対象を変更する必要があるならtrueでアピールする。
	bool needSearchTarget() const;

	// 攻撃対象の情報を取得（オーバーライド）
	int getTargetId() const;
	const char* getTargetName() const;

	// 目標地点へ移動するだけ 達成済みならtrueで何もしない
	bool moveGoalOrder(int& right, int& left, int& up, int& down, int& jump);

protected:
	// スティック操作
	void stickOrder(int& right, int& left, int& up, int& down);
};


/*
* 仲間についていきつつ戦うNormalAI
*/
class FollowNormalAI :
	public NormalAI
{
protected:
	// ついていくキャラ
	const Character* m_follow_p;

	// 追跡対象の近くにいるとみなす誤差 ±GX_ERROR
	const int FOLLOW_X_ERROR = 600;

public:
	static const char* BRAIN_NAME;
	const char* getBrainName() const { return this->BRAIN_NAME; }

	FollowNormalAI();

	Brain* createCopy(std::vector<Character*> characters, const Camera* camera);

	void debug(int x, int y, int color) const;

	// 追跡対象の情報を取得（オーバーライド）
	int getFollowId() const;
	const char* getFollowName() const;
	const Character* getFollow() const;

	// 追跡対象をセット
	void setFollow(const Character* character);

	// 移動の目標地点設定
	void moveOrder(int& right, int& left, int& up, int& down);

	// 追跡対象を決める(AIクラスでオーバライドする。)
	void searchFollow(const Character* character);

	// 追跡対象を変更する必要があるならtrueでアピールする(AIクラスでオーバライドする)。
	bool needSearchFollow() const;

	// 追跡対象の近くにいるか判定
	bool checkAlreadyFollow();
};


/*
* ParabolaBulletを使うAI
*/
class ParabolaAI :
	public NormalAI
{
public:
	static const char* BRAIN_NAME;
	const char* getBrainName() const { return this->BRAIN_NAME; }

	ParabolaAI();

	Brain* createCopy(std::vector<Character*> characters, const Camera* camera);

	void bulletTargetPoint(int& x, int& y);
};


/*
* ParabolaBulletを使うAI
*/
class FollowParabolaAI :
	public FollowNormalAI
{
public:
	static const char* BRAIN_NAME;
	const char* getBrainName() const { return this->BRAIN_NAME; }

	FollowParabolaAI();

	Brain* createCopy(std::vector<Character*> characters, const Camera* camera);

	void bulletTargetPoint(int& x, int& y);
};


/*
* ヴァルキリア用AI 斬撃の間合いやハートとの距離に気を付ける
*/
class ValkiriaAI :
	public FollowNormalAI
{
public:
	static const char* BRAIN_NAME;
	const char* getBrainName() const { return this->BRAIN_NAME; }

	ValkiriaAI();

	Brain* createCopy(std::vector<Character*> characters, const Camera* camera);

	int slashOrder();
	int bulletOrder() { return 0; }
	void moveOrder(int& right, int& left, int& up, int& down);
	int jumpOrder();

	// 斬撃攻撃を振る最大距離
	int getSlashReach() const { return 1200; }

	// 攻撃対象を変更する必要があるならtrueでアピールする。
	bool needSearchTarget() const;

};


/*
* 空を飛ぶキャラ用のAI
*/
class FlightAI :
	public NormalAI
{
protected:
	// 壁にぶつかったとき、trueにして上か下へ移動する。trueのとき天井や床にぶつかっていたら逆へ移動
	bool m_try;
public:
	static const char* BRAIN_NAME;
	const char* getBrainName() const { return this->BRAIN_NAME; }

	FlightAI();

	Brain* createCopy(std::vector<Character*> characters, const Camera* camera);

	void moveOrder(int& right, int& left, int& up, int& down);

	// 目標地点へ移動するだけ 達成済みならtrueで何もしない
	bool moveGoalOrder(int& right, int& left, int& up, int& down, int& jump);

	void setGoalToTarget();
};


/*
* 空を飛ぶ追跡キャラAI
*/
class FollowFlightAI :
	public FollowNormalAI
{
private:
	// 壁にぶつかったとき、trueにして上か下へ移動する。trueのとき天井や床にぶつかっていたら逆へ移動
	bool m_try;
public:
	static const char* BRAIN_NAME;
	const char* getBrainName() const { return this->BRAIN_NAME; }

	FollowFlightAI();

	Brain* createCopy(std::vector<Character*> characters, const Camera* camera);

	void moveOrder(int& right, int& left, int& up, int& down);

	// 目標地点へ移動するだけ 達成済みならtrueで何もしない
	bool moveGoalOrder(int& right, int& left, int& up, int& down, int& jump);
};


/*
* 棒立ちで射撃するヒエラルキー
*/
class HierarchyAI :
	public NormalAI
{
public:

	static const char* BRAIN_NAME;
	const char* getBrainName() const { return this->BRAIN_NAME; }

	HierarchyAI();

	Brain* createCopy(std::vector<Character*> characters, const Camera* camera);

	int bulletOrder();
	void bulletTargetPoint(int& x, int& y);

	void moveOrder(int& right, int& left, int& up, int& down) { return; }
	int jumpOrder() { return 0; }
	int squatOrder() { m_squatCnt = 0; return 0; }

};


/*
* フレンチ用AI
*/
class FrenchAI :
	public NormalAI
{
public:
	static const char* BRAIN_NAME;
	const char* getBrainName() const { return this->BRAIN_NAME; }

	FrenchAI();

	Brain* createCopy(std::vector<Character*> characters, const Camera* camera);

	int slashOrder();
	int bulletOrder() { return 0; }
	void moveOrder(int& right, int& left, int& up, int& down);
	int jumpOrder() { return 0; }
	int squatOrder() { m_squatCnt = 0; return 0; }

	// 斬撃攻撃を振る最大距離
	int getSlashReach() const { return 1500; }
};


/*
* カテゴリーZ用AI
*/
class CategoryZAI :
	public FrenchAI
{
public:
	static const char* BRAIN_NAME;
	const char* getBrainName() const { return this->BRAIN_NAME; }

	CategoryZAI();

	Brain* createCopy(std::vector<Character*> characters, const Camera* camera);

	// 斬撃攻撃を振る最大距離
	int getSlashReach() const { return 500; }
};


/*
* Boss1: サン
*/
class SunAI :
	public FlightAI
{
private:

public:
	static const char* BRAIN_NAME;
	const char* getBrainName() const { return this->BRAIN_NAME; }

	SunAI();

	Brain* createCopy(std::vector<Character*> characters, const Camera* camera);

	// 移動（上下左右の入力）
	void moveOrder(int& right, int& left, int& up, int& down);

	// ジャンプの制御
	int jumpOrder();

	// しゃがみの制御
	int squatOrder();

	// 近距離攻撃
	int slashOrder();

	// 遠距離攻撃
	int bulletOrder();

	void bulletTargetPoint(int& x, int& y);

	void setGoalToTarget();

};


#endif