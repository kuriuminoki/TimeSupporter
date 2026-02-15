#ifndef CHACACTER_CONTROLLER_H_INCLUDED
#define CHACACTER_CONTROLLER_H_INCLUDED


#include <vector>
#include <string>


class Character;
class CharacterAction;
class Object;
class Camera;
class Brain;
class ControllerRecorder;
class SoundPlayer;


/*
* コントローラの基底クラス（キャラクターを操作するクラス）
*/
class CharacterController {
protected:
	// 複製ならtrue Recorderをデリートしないため
	bool m_duplicationFlag;

	// こいつが操作を命令してくる Controllerがデリートする
	Brain* m_brain;

	// 操作対象 Controllerがデリートする
	CharacterAction* m_characterAction;

	// 操作の記録 使わないならnullptr
	ControllerRecorder* m_stickRecorder;
	ControllerRecorder* m_jumpRecorder;
	ControllerRecorder* m_squatRecorder;
	ControllerRecorder* m_slashRecorder;
	ControllerRecorder* m_bulletRecorder;

	// ダメージの記録 変化したらそれ以降のレコードを削除する
	ControllerRecorder* m_damageRecorder;

	// 操作対象に攻撃を与えたObjectのID(2重で攻撃を与えない用)
	std::vector<int> m_damagedObjectIds;

public:
	static const char* CONTROLLER_NAME;
	virtual const char* getControllerName() const { return this->CONTROLLER_NAME; }

	CharacterController();
	CharacterController(Brain* brain, CharacterAction* characterAction);
	~CharacterController();

	virtual CharacterController* createCopy(std::vector<Character*> characters, const Camera* camera) = 0;

	// デバッグ
	void debugController(int x, int y, int color) const;
	virtual void debug(int x, int y, int color) const = 0;

	// ゲッタ
	inline const CharacterAction* getAction() const { return m_characterAction; }
	inline const Brain* getBrain() const { return m_brain; }
	inline const ControllerRecorder* getStickRecorder() const { return m_stickRecorder; }
	inline const ControllerRecorder* getJumpRecorder() const { return m_jumpRecorder; }
	inline const ControllerRecorder* getSquatRecorder() const { return m_squatRecorder; }
	inline const ControllerRecorder* getSlashRecorder() const { return m_slashRecorder; }
	inline const ControllerRecorder* getBulletRecorder() const { return m_bulletRecorder; }
	inline const ControllerRecorder* getDamageRecorder() const { return m_damageRecorder; }

	// セッタ
	void setAction(CharacterAction* action);
	void setBrain(Brain* brain);
	void setStickRecorder(ControllerRecorder* recorder);
	void setJumpRecorder(ControllerRecorder* recorder);
	void setSquatRecorder(ControllerRecorder* recorder);
	void setSlashRecorder(ControllerRecorder* recorder);
	void setBulletRecorder(ControllerRecorder* recorder);
	void setDamageRecorder(ControllerRecorder* recorder);
	void setTarget(Character* character);
	void setDuplicationFlag(bool flag) { m_duplicationFlag = flag; }
	inline void setDamagedObjectIds(std::vector<int>& list) { for (unsigned int i = 0; i < list.size(); i++) { m_damagedObjectIds.push_back(list[i]); } }

	// レコーダを初期化
	void initRecorder();
	// レコードをやめる
	void eraseRecorder();

	// 話しかけたり扉に入ったりするボタンがtrueか
	virtual bool getActionKey() const;

	// アクションのセッタ
	void setCharacterGrand(bool grand);
	void setCharacterGrandRightSlope(bool grand);
	void setCharacterGrandLeftSlope(bool grand);
	void setActionRightLock(bool lock);
	void setActionLeftLock(bool lock);
	void setActionUpLock(bool lock);
	void setActionDownLock(bool lock);
	void setActionSound(SoundPlayer* soundPlayer);
	void addActionDx(int value);
	void setCharacterDeadFlag(bool deadFlag);

	// キャラクターのセッタ
	void setCharacterX(int x);
	void setCharacterY(int y);
	void setCharacterFreeze(bool freeze);

	// 行動前の処理 毎フレーム行う
	void init();

	// 攻撃対象を変更
	void searchTargetCandidate(const Character* character);

	// 追跡対象を変更
	void searchFollowCandidate(const Character* character);

	// 攻撃対象を強制変更
	void setBrainTarget(const Character* character);

	// 追跡対象を強制変更
	void setBrainFollow(const Character* character);

	// 操作や当たり判定の結果を反映（実際にキャラを動かす）毎フレーム行う
	void action();

	// キャラの操作
	virtual void control() = 0;

	// 射撃攻撃
	virtual std::vector<Object*>* bulletAttack() = 0;

	// 斬撃攻撃
	virtual std::vector<Object*>* slashAttack() = 0;

	// ダメージ
	virtual void damage(int vx, int vy, int damageValue) = 0;

	// BrainがFreezeならプレイヤーの方向を向かせる allがtrueならFreeze以外の全キャラが対象
	void setPlayerDirection(Character* player_p, bool all = false);

	// AIの目標地点を設定
	void setGoal(int gx, int gy);

	// 目標地点へ移動するだけ
	bool moveGoal();

	// キャラのストップ時間を消費
	void consumeStopCnt();

	// キャラをストップ
	void stopCharacter(int cnt);

	// 攻撃を受けたことがあるIDかチェックし、ないならpushしてfalseを返す
	bool checkAndPushDamagedObjectId(int id);
};


CharacterController* createController(const std::string controllerName, Brain* brain, CharacterAction* action);


/*
* 普通のコントローラ
*/
class NormalController :
	public CharacterController {

private:
	// ジャンプキーを長押しする最大時間
	const int JUMP_KEY_LONG = 10;
public:
	static const char* CONTROLLER_NAME;
	const char* getControllerName() const { return this->CONTROLLER_NAME; }

	NormalController(Brain* brain, CharacterAction* characterAction);

	CharacterController* createCopy(std::vector<Character*> characters, const Camera* camera);

	void debug(int x, int y, int color) const;

	// キャラの移動やしゃがみ(;攻撃以外の)操作 毎フレーム行う
	void control();

	// 射撃攻撃
	std::vector<Object*>* bulletAttack();

	// 斬撃攻撃
	std::vector<Object*>* slashAttack();

	// ダメ―ジ
	void damage(int vx, int vy, int damageValue);
};

#endif
