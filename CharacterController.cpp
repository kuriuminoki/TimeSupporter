#include "CharacterController.h"
#include "CharacterAction.h"
#include "Character.h"
#include "Camera.h"
#include "Control.h"
#include "Brain.h"
#include "Sound.h"
#include "Define.h"
#include "DxLib.h"
#include <algorithm>


using namespace std;


// クラス名
const char* CharacterController::CONTROLLER_NAME = "CharacterController";
const char* NormalController::CONTROLLER_NAME = "NormalController";

// クラス名からコントローラを作成する関数
CharacterController* createController(const string controllerName, Brain* brain, CharacterAction* action) {
	CharacterController* controller = nullptr;
	if (controllerName == NormalController::CONTROLLER_NAME) {
		controller = new NormalController(brain, action);
	}
	return controller;
}


/*
* コントローラ
*/
CharacterController::CharacterController(Brain* brain, CharacterAction* characterAction) {

	// 初期化
	m_brain = brain;
	m_characterAction = characterAction;

	// BrainにActionを注入
	m_brain->setCharacterAction(m_characterAction);

}

CharacterController::CharacterController() :
	CharacterController(nullptr, nullptr)
{

}

// Actionインスタンスはここでのみデリートする
CharacterController::~CharacterController() {
	if (m_characterAction != nullptr) {
		delete m_characterAction;
	}
	if (m_brain != nullptr) {
		delete m_brain;
	}
}

// 話しかけたり扉に入ったりするボタンがtrueか
bool CharacterController::getActionKey() const { return m_brain->actionOrder(); }

void CharacterController::setAction(CharacterAction* action) {
	delete m_characterAction;
	m_characterAction = action;
	m_brain->setCharacterAction(m_characterAction);
}
void CharacterController::setBrain(Brain* brain) {
	delete m_brain;
	m_brain = brain;
	m_brain->setCharacterAction(m_characterAction);
}

void CharacterController::setTarget(Character* character) { 
	m_brain->setTarget(character);
}

// アクションのセッタ
void CharacterController::setCharacterGrand(bool grand) {
	m_characterAction->setGrand(grand);
}
void CharacterController::setCharacterGrandRightSlope(bool grand) {
	m_characterAction->setGrandRightSlope(grand);
}
void CharacterController::setCharacterGrandLeftSlope(bool grand) {
	m_characterAction->setGrandLeftSlope(grand);
}
void CharacterController::setActionRightLock(bool lock) {
	m_characterAction->setRightLock(lock);
}
void CharacterController::setActionLeftLock(bool lock) {
	m_characterAction->setLeftLock(lock);
}
void CharacterController::setActionUpLock(bool lock) {
	m_characterAction->setUpLock(lock);
}
void CharacterController::setActionDownLock(bool lock) {
	m_characterAction->setDownLock(lock);
}
void CharacterController::setActionSound(SoundPlayer* soundPlayer) {
	m_characterAction->setSoundPlayer(soundPlayer);
}
void CharacterController::addActionDx(int value) {
	m_characterAction->setDx(m_characterAction->getDx() + value);
}
void CharacterController::setCharacterDeadFlag(bool deadFlag) {
	m_characterAction->setDeadFlag(deadFlag);
}

// キャラクターのセッタ
void CharacterController::setCharacterX(int x) {
	m_characterAction->setCharacterX(x);
}
void CharacterController::setCharacterY(int y) {
	m_characterAction->setCharacterY(y);
}
void CharacterController::setCharacterFreeze(bool freeze) {
	m_characterAction->setCharacterFreeze(freeze);
}

// 行動前の処理
void CharacterController::init() {
	m_characterAction->init();
}

// 攻撃対象を変更
void CharacterController::searchTargetCandidate(const Character* character) {
	m_brain->searchTarget(character);
}

// 追跡対象を変更
void CharacterController::searchFollowCandidate(const Character* character) {
	m_brain->searchFollow(character);
}

// 攻撃対象を強制変更
void CharacterController::setBrainTarget(const Character* character) {
	m_brain->setTarget(character);
}

// 追跡対象を強制変更
void CharacterController::setBrainFollow(const Character* character) {
	m_brain->setFollow(character);
}

// 行動の結果反映
void CharacterController::action() {
	// 物理演算
	m_characterAction->action();
}

// BrainがFreezeならプレイヤーの方向を向かせる
void CharacterController::setPlayerDirection(Character* player_p, bool all) {
	if (m_brain->getBrainName() != "Freeze" && !all) { return; }
	if (!m_characterAction->ableChangeDirection()) { return; }
	m_characterAction->setCharacterLeftDirection(player_p->getCenterX() < m_characterAction->getCharacter()->getCenterX());
}

// AIの目標地点を設定
void CharacterController::setGoal(int gx, int gy) {
	m_brain->setGx(gx);
	m_brain->setGy(gy);
}

// 目標地点へ移動するだけ
bool CharacterController::moveGoal() {
	if (m_characterAction->getCharacter()->getHp() == 0) { return true; }
	// 移動 stickなどの入力状態を更新する
	int rightStick = 0, leftStick = 0, upStick = 0, downStick = 0, jump = 0;
	bool alreadyGoal = m_brain->moveGoalOrder(rightStick, leftStick, upStick, downStick, jump);
	// stickに応じて移動
	m_characterAction->move(rightStick, leftStick, upStick, downStick);
	// ジャンプ
	m_characterAction->jump(jump);
	return alreadyGoal;
}

void CharacterController::consumeStopCnt() {
	m_characterAction->consumeStopCnt();
}

void CharacterController::stopCharacter(int cnt) {
	m_characterAction->stopCharacter(cnt);
}

bool CharacterController::checkAndPushDamagedObjectId(int id) {
	for (unsigned i = 0; i < m_damagedObjectIds.size(); i++) {
		if (m_damagedObjectIds[i] == id) {
			return true;
		}
	}
	m_damagedObjectIds.push_back(id);
	// 増加し続けるのを防ぐため古いIDは不要とみなし掃除する
	if (m_damagedObjectIds.size() > 10) { m_damagedObjectIds.erase(m_damagedObjectIds.begin(), m_damagedObjectIds.begin() + 4); }
	return false;
}


/*
* キャラコントロール マウスを使う可能性もあるのでCameraが必要
*/
NormalController::NormalController(Brain* brain, CharacterAction* characterAction):
	CharacterController(brain, characterAction)
{

}

void NormalController::control() {

	// 移動 stickなどの入力状態を更新する
	int rightStick = 0, leftStick = 0, upStick = 0, downStick = 0;
	if (m_characterAction->getCharacter()->getHp() == 0) {
		// やられているから何もしない
	}
	else {
		m_brain->moveOrder(rightStick, leftStick, upStick, downStick);
	}

	// stickに応じて移動
	m_characterAction->move(rightStick, leftStick, upStick, downStick);

	// ジャンプ
	int jump = 0;
	if (m_characterAction->getCharacter()->getHp() == 0) {
		// やられているから何もしない
	}
	else {
		jump = m_brain->jumpOrder();
	}
	m_characterAction->jump(jump);

	// ブースト
	if (jump == 1 && (rightStick > 0 || leftStick > 0)) {
		m_characterAction->setBoost(leftStick > rightStick);
	}

	// しゃがみ
	int squat = 0;
	if (m_characterAction->getCharacter()->getHp() == 0) {
		// やられているから何もしない
	}
	else {
		squat = m_brain->squatOrder();
	}
	m_characterAction->setSquat(squat);

	// ステップ
	if (downStick > 0 && (leftStick == 1 || rightStick == 1)) {
		m_characterAction->setStep(leftStick == 1);
	}
}

vector<Object*>* NormalController::bulletAttack() {

	if (m_characterAction->getCharacter()->getHp() == 0) {
		return nullptr;
	}

	// 攻撃目標
	int targetX = 0, targetY = 0;
	m_brain->bulletTargetPoint(targetX, targetY);

	// 命令
	int order = m_brain->bulletOrder();

	// 遠距離攻撃の命令がされているなら
	if (order > 0 || m_characterAction->getBulletCnt() > 0) {
		// 目標に向かって射撃
		return m_characterAction->bulletAttack(targetX, targetY);
	}
	return nullptr;
}

vector<Object*>* NormalController::slashAttack() {

	if (m_characterAction->getCharacter()->getHp() == 0) {
		return nullptr;
	}

	// 攻撃目標
	int targetX = 0, targetY = 0;
	m_brain->slashTargetPoint(targetX, targetY);

	// 命令
	int order = m_brain->slashOrder();

	// スライディング
	if (order == 1) {
		m_characterAction->setSliding(m_characterAction->getCharacter()->getCenterX() > targetX);
	}
	if (m_characterAction->getSlidingDone() != 0) {
		return m_characterAction->slidingAttack();
	}

	// 近距離攻撃の命令がされたか、した後で今が攻撃タイミングなら
	if (order == 1 || m_characterAction->getSlashCnt() > 0) {
		return m_characterAction->slashAttack(targetX, targetY);
	}
	return nullptr;
}

void NormalController::damage(int vx, int vy, int damageValue) {
	m_characterAction->damage(vx, vy, damageValue);
}