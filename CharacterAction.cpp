#include "CharacterAction.h"
#include "Character.h"
#include "Sound.h"
#include "Define.h"
#include "DxLib.h"


using namespace std;


// クラス名
const char* CharacterAction::ACTION_NAME = "CharacterAction";
const char* StickAction::ACTION_NAME = "StickAction";
const char* ValkiriaAction::ACTION_NAME = "ValkiriaAction";
const char* FlightAction::ACTION_NAME = "FlightAction";
const char* KoharuAction::ACTION_NAME = "KoharuAction";
const char* SunAction::ACTION_NAME = "SunAction";
const char* ArchiveAction::ACTION_NAME = "ArchiveAction";
const char* BossFreezeAction::ACTION_NAME = "BossFreezeAction";

// クラス名からCharacterActionを作成する関数
CharacterAction* createAction(const string actionName, Character* character, SoundPlayer* soundPlayer_p) {

	string tmp = actionName;

	bool heavy = false;
	if (tmp.size() >= 2) {
		string last = tmp.substr(tmp.size() - 2);
		if (last == "_x") {
			heavy = true;
			tmp.erase(tmp.size() - 2, 2);
		}
	}

	CharacterAction* action = nullptr;
	if (tmp == StickAction::ACTION_NAME) {
		action = new StickAction(character, soundPlayer_p);
	}
	else if(tmp == ValkiriaAction::ACTION_NAME) {
		action = new ValkiriaAction(character, soundPlayer_p);
	}
	else if (tmp == FlightAction::ACTION_NAME) {
		action = new FlightAction(character, soundPlayer_p);
	}
	else if (tmp == KoharuAction::ACTION_NAME) {
		action = new KoharuAction(character, soundPlayer_p);
	}
	else if (tmp == BossFreezeAction::ACTION_NAME) {
		action = new BossFreezeAction(character, soundPlayer_p);
	}
	else if (tmp == SunAction::ACTION_NAME) {
		action = new SunAction(character, soundPlayer_p, false);
	}
	else if (tmp == ArchiveAction::ACTION_NAME) {
		action = new ArchiveAction(character, soundPlayer_p, false);
	}

	action->setHeavy(heavy);

	return action;
}


/*
*  キャラクターの基底クラス
*/
CharacterAction::CharacterAction(Character* character, SoundPlayer* soundPlayer_p) {
	m_character_p = character;
	m_soundPlayer_p = soundPlayer_p;

	//初期状態
	m_cnt = 0;
	m_prevLeftDirection = m_character_p->getLeftDirection();
	m_state = CHARACTER_STATE::STAND;
	m_characterVersion = character->getVersion();
	m_characterMoveSpeed = character->getMoveSpeed();
	m_grand = false;
	m_runCnt = -1;
	m_preJumpCnt = -1;
	m_moveRight = false;
	m_moveLeft = false;
	m_moveUp = false;
	m_moveDown = false;
	m_grandLeftSlope = false;
	m_grandRightSlope = false;
	m_vx = 0;
	m_vy = 0;
	m_runVx = 0;
	m_dx = 0;
	m_rightLock = false;
	m_leftLock = false;
	m_upLock = false;
	m_downLock = false;
	m_bulletCnt = 0;
	m_slashCnt = 0;
	m_jumpCnt = 0;
	m_attackLeftDirection = false;
	m_landCnt = 0;
	m_boostCnt = 0;
	m_boostDone = 0;
	m_stepCnt = 0;
	m_stepDone = 0;
	m_slidingCnt = 0;
	m_slidingDone = 0;
	m_damageCnt = 0;
	m_heavy = false;
}

CharacterAction::CharacterAction() :
	CharacterAction(nullptr, nullptr)
{

}

void CharacterAction::setParam(CharacterAction* action) {
	action->setCnt(m_cnt);
	action->setState(m_state);
	action->setCharacterVersion(m_characterVersion);
	action->setCharacterMoveSpeed(m_characterMoveSpeed);
	action->setSimpleGrand(m_grand);
	action->setGrandLeftSlope(m_grandLeftSlope);
	action->setGrandRightSlope(m_grandRightSlope);
	action->setRunCnt(m_runCnt);
	action->setPreJumpCnt(m_preJumpCnt);
	action->setMoveRight(m_moveRight);
	action->setMoveLeft(m_moveLeft);
	action->setMoveUp(m_moveUp);
	action->setMoveDown(m_moveDown);
	action->setVx(m_vx);
	action->setVy(m_vy);
	action->setRunVx(m_runVx);
	action->setRunVy(m_runVy);
	action->setRightLock(m_rightLock);
	action->setLeftLock(m_leftLock);
	action->setUpLock(m_upLock);
	action->setDownLock(m_downLock);
	action->setBulletCnt(m_bulletCnt);
	action->setSlashCnt(m_slashCnt);
	action->setJumpCnt(m_jumpCnt);
	action->setAttackLeftDirection(m_attackLeftDirection);
	action->setLandCnt(m_landCnt);
	action->setBoostCnt(m_boostCnt);
	action->setBoostDone(m_boostDone);
	action->setStepCnt(m_stepCnt);
	action->setStepDone(m_stepDone);
	action->setSlidingCnt(m_slidingCnt);
	action->setSlidingDone(m_slidingDone);
	action->setDamageCnt(m_damageCnt);
	action->setHeavy(m_heavy);
}

void CharacterAction::setState(CHARACTER_STATE state) {
	if (m_state == CHARACTER_STATE::DAMAGE || m_state == CHARACTER_STATE::INIT) { return; }
	m_state = state;
}

// キャラを動けなくする
void CharacterAction::setRightLock(bool lock) { 
	m_rightLock = lock;
}
void CharacterAction::setLeftLock(bool lock) { 
	m_leftLock = lock;
}
void CharacterAction::setUpLock(bool lock) { 
	m_upLock = lock;
}
void CharacterAction::setDownLock(bool lock) { 
	m_downLock = lock;
}

// Boost
void CharacterAction::setBoost(bool leftDirection) {
	if ((leftDirection && m_leftLock) || (!leftDirection && m_rightLock)) {
		return;
	}
	if (!damageFlag() && !m_grand && !m_boostDone) {
		m_boostCnt = BOOST_TIME;
		m_vy -= BOOST_SPEED;
		finishBullet();
		finishSlash();
		if (leftDirection) {
			m_vx -= BOOST_SPEED;
			m_boostDone = 2;
		}
		else {
			m_vx += BOOST_SPEED;
			m_boostDone = 1;
		}
		m_character_p->setLeftDirection(leftDirection);
	}
}
void CharacterAction::finishBoost() {
	m_boostCnt = 0;
	if (m_boostDone == 1) {
		m_vx -= BOOST_SPEED;
	}
	else if (m_boostDone == 2) {
		m_vx += BOOST_SPEED;
	}
	m_boostDone = 0;
}

void CharacterAction::setStep(bool leftDirection) {
	if (!m_character_p->haveSlidingGraph()) { return; }
	if (m_state != CHARACTER_STATE::SQUAT || m_stepDone != 0 || m_slidingCnt > 0) { return; }
	m_stepCnt = STEP_TIME;
	if (leftDirection && !m_leftLock) {
		m_vx -= STEP_SPEED;
		m_stepDone = 2;
		m_character_p->setLeftDirection(false);
	}
	else if(!m_rightLock) {
		m_vx += STEP_SPEED;
		m_stepDone = 1;
		m_character_p->setLeftDirection(true);
	}
	finishBullet();
}
void CharacterAction::finishStep() {
	if (m_stepCnt > STEP_STOP_TIME) {
		if (m_stepDone == 1) {
			m_vx -= STEP_SPEED;
		}
		else if (m_stepDone == 2) {
			m_vx += STEP_SPEED;
		}
	}
	m_stepCnt = 0;
	m_stepDone = 0;
}

void CharacterAction::setSliding(bool leftDirection) {
	if (!m_character_p->haveStepGraph()) { return; }
	if (m_state != CHARACTER_STATE::SQUAT || m_slidingDone != 0 || m_stepCnt > 0) { return; }
	m_slidingCnt = SLIDING_SPEED;
	if (leftDirection) {
		m_vx -= SLIDING_SPEED;
		m_slidingDone = 2;
		m_character_p->setLeftDirection(true);
	}
	else {
		m_vx += SLIDING_SPEED;
		m_slidingDone = 1;
		m_character_p->setLeftDirection(false);
	}
	finishBullet();
}
void CharacterAction::finishSliding() {
	m_slidingCnt = 0;
	if (m_slidingDone == 1) {
		m_vx -= m_slidingCnt;
	}
	else if (m_slidingDone == 2) {
		m_vx += m_slidingCnt;
	}
	m_slidingDone = 0;
}

// キャラクターのセッタ
void CharacterAction::setCharacterX(int x) {
	m_character_p->setX(x);
}
void CharacterAction::setCharacterY(int y) {
	m_character_p->setY(y);
}
void CharacterAction::setCharacterLeftDirection(bool leftDirection) {
	m_character_p->setLeftDirection(leftDirection);
}
void CharacterAction::setCharacterFreeze(bool freeze) {
	m_character_p->setFreeze(freeze);
}
void CharacterAction::setDeadFlag(bool deadFlag) {
	m_character_p->setDeadFlag(deadFlag);
}

// 行動前の処理 毎フレーム行う
void CharacterAction::init() {

	m_cnt++;

	m_dx = 0;

	// 前のフレームのleftDirectionを保存しておく
	m_prevLeftDirection = m_character_p->getLeftDirection();

	// いったん全方向に動けるようにする
	m_rightLock = false;
	m_leftLock = false;
	m_upLock = false;
	m_downLock = false;

	// いったん宙に浮かせる
	m_grand = false;
	m_grandRightSlope = false;
	m_grandLeftSlope = false;

	// prevHpをhpに追いつかせる
	m_character_p->setPrevHp(m_character_p->getPrevHp() - 1);

	// キャラのバージョンが変化した場合
	if (m_characterVersion != m_character_p->getVersion()) {
		if (m_moveUp) { m_vy += m_characterMoveSpeed; m_vy -= m_character_p->getMoveSpeed(); }
		if (m_moveDown) { m_vy -= m_characterMoveSpeed; m_vy += m_character_p->getMoveSpeed(); }
		m_characterVersion = m_character_p->getVersion();
		m_characterMoveSpeed = m_character_p->getMoveSpeed();
	}
}

void CharacterAction::bulletAction() {
	// 射撃のインターバル処理
	if (m_bulletCnt > 0) {
		m_bulletCnt--;
		if (m_bulletCnt == 0) { finishBullet(); }
	}
}

void CharacterAction::slashAction() {
	// 斬撃のインターバル処理
	if (m_slashCnt > 0) {
		m_slashCnt--;
		if (m_slashCnt == 0) { finishSlash(); }
	}
}

void CharacterAction::damageAction() {
	// ダメージ受け状態は最低１秒近くある
	if (m_damageCnt > 0) { m_damageCnt--; }
}

void CharacterAction::otherAction() {
	// アニメーション用のカウント
	if (m_landCnt > 0) { m_landCnt--; }
	if (m_boostCnt > 0) { m_boostCnt--; }
	if (m_stepCnt > 0) { 
		m_stepCnt--;
		if (m_stepCnt == STEP_STOP_TIME) {
			if (m_stepDone == 1) {
				m_vx -= STEP_SPEED;
			}
			else if (m_stepDone == 2) {
				m_vx += STEP_SPEED;
			}
		}
		if (m_stepCnt == 0) {
			finishStep();
		}
	}
	if (m_slidingCnt > 0) {
		m_slidingCnt--;
		if (m_slidingDone == 1) { m_vx--; }
		if (m_slidingDone == 2) { m_vx++; }
		if (m_slidingCnt == 0) {
			finishSliding();
		}
	}
}

void CharacterAction::moveAction() {
	// 移動
	if (!m_heavy && ((m_dx < 0 && !m_leftLock) || (m_dx > 0 && !m_rightLock))) {
		m_character_p->moveRight(m_dx);
	}
	if (m_vx + m_runVx > 0) {// 右
		if (m_rightLock) {
			stopMoveLeft(); // 左に移動したいのに吹っ飛び等で右へ移動しているとき、いったん左移動への入力をキャンセルさせないとバグる
			m_vx = 0;
			m_runVx = 0;
		}
		else {
			m_character_p->moveRight(m_vx + m_runVx);
		}
	}
	else if (m_vx + m_runVx < 0) { // 左
		if (m_leftLock) {
			stopMoveRight();// 右に移動したいのに吹っ飛び等で左へ移動しているとき、いったん右移動への入力をキャンセルさせないとバグる
			m_vx = 0;
			m_runVx = 0;
		}
		else {
			m_character_p->moveLeft(-m_vx - m_runVx);
		}
	}
	if (m_vy + m_runVy < 0) { // 上
		if (m_upLock) {
			stopMoveDown();
			m_vy = 0;
			m_runVy = 0;
		}
		else {
			m_character_p->moveUp(-m_vy - m_runVy);
		}
	}
	else if (m_vy + m_runVy > 0) { // 下
		if (m_downLock) {
			stopMoveUp();
			m_vy = 0;
			m_runVy = 0;
		}
		else {
			m_character_p->moveDown(m_vy + m_runVy);
		}
	}
}

void CharacterAction::action() {
	// 状態(state)に応じて画像をセット
	switchHandle();

	bulletAction();

	slashAction();

	damageAction();

	otherAction();

	moveAction();
}

// ダメージ
void CharacterAction::damage(int vx, int vy, int damageValue) {
	m_damageCnt = DAMAGE_TIME;
	if (!m_heavy) {
		setState(CHARACTER_STATE::DAMAGE);
		m_vx += vx;
		m_vy += vy;
		m_character_p->setLeftDirection(vx > 0);
		// 宙に浮かせる
		m_grand = false;
		m_grandRightSlope = false;
		m_grandLeftSlope = false;
	}
	// HP減少
	m_character_p->damageHp(damageValue);
	m_boostCnt = 0;
	finishSliding();
}

void CharacterAction::startBullet() {

}

void CharacterAction::finishBullet() {
	m_bulletCnt = 0;
}

void CharacterAction::startSlash() {

}

void CharacterAction::finishSlash() {
	m_slashCnt = 0;
}

bool CharacterAction::ableDamage() const {
	return !(m_state == CHARACTER_STATE::DAMAGE || m_damageCnt > 0 || m_boostCnt > max(0, BOOST_TIME - 10) || m_stepCnt > STEP_STOP_TIME);
}

bool CharacterAction::ableAttack() const {
	return !(m_bulletCnt > 0 || m_slashCnt > 0 || m_slidingCnt > 0 || m_stepCnt > STEP_STOP_TIME);
}

bool CharacterAction::ableWalk() const {
	return !m_moveRight && !m_moveLeft && m_state != CHARACTER_STATE::SQUAT;
}

bool CharacterAction::ableChangeDirection() const {
	if (m_character_p->getHp() == 0
		|| m_bulletCnt > 0 || m_slashCnt > 0
		|| m_moveLeft || m_moveRight || m_moveUp || m_moveDown
		|| m_damageCnt > 0) {
		return false;
	}
	return true;
}

// 着地
void CharacterAction::setGrand(bool grand) {
	if (m_vy + m_runVy > 0) { // 着地モーションになる
		m_landCnt = LAND_TIME;
		finishSlash();
		// 効果音
		if (m_soundPlayer_p != nullptr) {
			m_soundPlayer_p->pushSoundQueue(m_character_p->getLandSound(),
				adjustPanSound(m_character_p->getCenterX(),
					m_soundPlayer_p->getCameraX()));
		}
	}
	m_grand = grand;
	finishBoost();
	if (m_state == CHARACTER_STATE::DAMAGE && m_damageCnt == 0) {
		m_vx = 0;
		m_vy = 0;
		m_state = CHARACTER_STATE::STAND;
	}
}

void CharacterAction::setSquat(bool squat) {
	if (m_state != CHARACTER_STATE::DAMAGE && m_state != CHARACTER_STATE::PREJUMP && m_state != CHARACTER_STATE::INIT) {
		if ((squat && m_grand && m_slashCnt == 0) || (m_stepCnt > 0 || m_slidingCnt > 0)) {
			// しゃがめる状態なのでしゃがむ
			m_state = CHARACTER_STATE::SQUAT;
		}
		else {
			m_state = CHARACTER_STATE::STAND;
		}
	}
}

// 歩き始める
void CharacterAction::startMoveLeft() {
	// 左へ歩き始める
	m_moveLeft = true;
}
void CharacterAction::startMoveRight() {
	// 右へ歩き始める
	m_moveRight = true;
}
void CharacterAction::startMoveUp() {
	// 上へ歩き始める
	m_moveUp = true;
}
void CharacterAction::startMoveDown() {
	// 下へ歩き始める
	m_moveDown = true;
}

// 歩くのをやめる
void CharacterAction::stopMoveLeft() {
	// 左へ歩くのをやめる
	if (m_moveLeft) {
		m_moveLeft = false;
		m_runCnt = -1;
	}
	if (m_boostDone == 2) {
		finishBoost();
	}
	if (m_leftLock) {
		if (m_stepDone == 2) {
			finishStep();
		}
		if (m_slidingDone == 2) {
			finishSliding();
		}
	}
}
void CharacterAction::stopMoveRight() {
	// 右へ歩くのをやめる
	if (m_moveRight) {
		m_moveRight = false;
		m_runCnt = -1;
	}
	if (m_boostDone == 1) {
		finishBoost();
	}
	if (m_rightLock) {
		if (m_stepDone == 1) {
			finishStep();
		}
		if (m_slidingDone == 1) {
			finishSliding();
		}
	}
}
void CharacterAction::stopMoveUp() {
	// 上へ歩くのをやめる
	if (m_moveUp) {
		m_moveUp = false;
		m_runCnt = -1;
	}
}
void CharacterAction::stopMoveDown() {
	// 下へ歩くのをやめる
	if (m_moveDown) {
		m_moveDown = false;
		m_runCnt = -1;
	}
}

void CharacterAction::consumeStopCnt() {
	m_character_p->setStopCnt(max(0, m_character_p->getStopCnt() - 1));
}

void CharacterAction::stopCharacter(int cnt) {
	m_character_p->setStopCnt(cnt);
}

// 画像のサイズ変更による位置調整 (座標は画像の左上であることに注意)
void CharacterAction::afterChangeGraph(int beforeX1, int afterX1, int beforeY1, int afterY1, int beforeX2, int afterX2, int beforeY2, int afterY2) {
	int dy = 0;
	if (m_downLock) {
		if (afterY2 > beforeY2) {
			dy -= afterY2 - beforeY2;
		}
		else {
			dy += beforeY2 - afterY2;
		}
	}
	else if (m_upLock) {
		if (afterY1 < beforeY1) {
			dy += beforeY1 - afterY1;
		}
		else {
			dy -= afterY1 - beforeY1;
		}
	}
	else {
		dy = ((beforeY2 - afterY2) + (beforeY1 - afterY1)) / 2;
	}

	m_character_p->moveDown(dy);

	int dx = 0;
	if (m_rightLock) {
		if (afterX2 > beforeX2) {
			dx -= afterX2 - beforeX2;
		}
		else {
			dx += beforeX2 - afterX2;
		}
	}
	else if (m_leftLock) {
		if (afterX1 < beforeX1) {
			dx += beforeX1 - afterX1;
		}
		else {
			dx -= afterX1 - beforeX1;
		}
	}
	else {
		dx = ((beforeX2 - afterX2) + (beforeX1 - afterX1)) / 2;
	}

	m_character_p->moveRight(dx);
}


/*
* 空を飛ばない普通の棒人間
*/
StickAction::StickAction(Character* character, SoundPlayer* soundPlayer_p):
	CharacterAction(character, soundPlayer_p)
{

}

CharacterAction* StickAction::createCopy(vector<Character*> characters) {
	CharacterAction* res = nullptr;
	for (unsigned int i = 0; i < characters.size(); i++) {
		if (m_character_p->getId() == characters[i]->getId()) {
			res = new StickAction(characters[i], m_soundPlayer_p);
			// コピーする
			setParam(res);
			break;
		}
	}
	return res;
}

void StickAction::action() {
	// 重力の処理
	if (!m_grand) {
		// 重力
		m_vy += G;
	}
	else {
		m_jumpCnt = 0;
	}
	// 走りによる速度変化
	if (m_moveLeft) {
		m_runVx = max(m_runVx - 1, -m_character_p->getMoveSpeed());
	}
	else if (m_runVx < 0) {
		m_runVx += 1;
	}
	if (m_moveRight) {
		m_runVx = min(m_runVx + 1, m_character_p->getMoveSpeed());
	}
	else if (m_runVx > 0) {
		m_runVx -= 1;
	}
	CharacterAction::action();
}

// 状態に応じて画像セット
void StickAction::switchHandle() {
	// セット前の画像のサイズ
	int x1 = 0, y1 = 0, x2 = 0, y2 = 0;
	bool nowLeftDirection = m_character_p->getLeftDirection();
	m_character_p->setLeftDirection(m_prevLeftDirection);
	m_character_p->getAtariArea(&x1, &y1, &x2, &y2);
	
	// やられ画像
	if (m_grand && m_character_p->getHp() == 0 && m_character_p->haveDeadGraph() && m_state != CHARACTER_STATE::DAMAGE) {
		m_character_p->switchDead();
	}
	else if (m_grand) { // 地面にいるとき
		switch (m_state) {
		case CHARACTER_STATE::STAND: //立ち状態
			if (m_slashCnt > 0) {
				m_character_p->switchSlash(m_slashCnt);
			}
			else if (m_landCnt > 0) {
				m_character_p->switchLand();
			}
			else if (m_bulletCnt > 0) {
				if (m_runCnt != -1) {
					m_character_p->switchRunBullet(m_runCnt);
				}
				else {
					m_character_p->switchBullet(m_bulletCnt);
				}
			}
			else if (m_runCnt != -1) {
				m_character_p->switchRun(m_runCnt);
			}
			else {
				m_character_p->switchStand();
			}
			break;
		case CHARACTER_STATE::SQUAT:
			if (m_bulletCnt > 0) {
				m_character_p->switchSquatBullet(m_bulletCnt);
			}
			else if (m_stepCnt > 0) {
				m_character_p->switchStep(m_stepCnt);
			}
			else if (m_slidingCnt > 0) {
				m_character_p->switchSliding(m_slidingCnt);
			}
			else {
				m_character_p->switchSquat();
			}
			break;
		case CHARACTER_STATE::PREJUMP:
			m_character_p->switchPreJump(m_preJumpCnt);
			break;
		case CHARACTER_STATE::DAMAGE:
			if (m_boostCnt > 0) { 
				if (m_slashCnt > 0) {
					m_character_p->switchSlash(m_slashCnt);
				}
				else if (m_bulletCnt > 0) {
					if (m_runCnt != -1) {
						m_character_p->switchRunBullet(m_runCnt);
					}
					else {
						m_character_p->switchBullet(m_bulletCnt);
					}
				}
				else {
					m_character_p->switchBoost();
				}
			}
			else {
				m_character_p->switchDamage();
			}
			break;
		}
	}
	else { // 宙にいるとき
		switch (m_state) {
		case CHARACTER_STATE::STAND: //立ち状態(なにもなしの状態)
			if (m_slashCnt > 0) {
				m_character_p->switchAirSlash(m_slashCnt);
			}
			else if (m_bulletCnt > 0) {
				m_character_p->switchAirBullet(m_bulletCnt);
			}
			else if (m_boostCnt > 0) {
				m_character_p->switchBoost();
			}
			else if (m_vy < 0) {
				m_character_p->switchJump(m_jumpCnt++);
			}
			else {
				m_character_p->switchDown();
			}
			break;
		case CHARACTER_STATE::DAMAGE:
			if (m_boostCnt > 0) {
				if (m_slashCnt > 0) {
					m_character_p->switchAirSlash();
				}
				else if (m_bulletCnt > 0) {
					m_character_p->switchAirBullet(m_bulletCnt);
				}
				else {
					m_character_p->switchBoost();
				}
			}
			else {
				m_character_p->switchDamage();
			}
			break;
		}
	}

	m_character_p->setLeftDirection(nowLeftDirection);

	// セット後の画像のサイズ
	int afterX1 = 0, afterY1 = 0, afterX2 = 0, afterY2 = 0;
	m_character_p->getAtariArea(&afterX1, &afterY1, &afterX2, &afterY2);

	// サイズ変更による位置調整
	afterChangeGraph(x1, afterX1, y1, afterY1, x2, afterX2, y2, afterY2);

}

// 歩く ダメージ中、しゃがみ中、壁ぶつかり中は不可
void StickAction::walk(bool right, bool left) {
	// 右へ歩くのをやめる
	if (!right || m_rightLock || m_state == CHARACTER_STATE::SQUAT || damageFlag()) {
		stopMoveRight();
	}
	if (m_slashCnt > 0 && !m_attackLeftDirection && (m_rightLock || damageFlag()) && m_vx > 0) {
		finishSlash();
	}
	if (m_bulletCnt > 0 && !m_attackLeftDirection && m_rightLock && m_vx > 0) {
		finishBullet();
	}
	// 左へ歩くのをやめる
	if (!left || m_leftLock || m_state == CHARACTER_STATE::SQUAT || damageFlag()) {
		stopMoveLeft();
	}
	if (m_slashCnt > 0 && m_attackLeftDirection && (m_leftLock || damageFlag()) && m_vx < 0) {
		finishSlash();
	}
	if (m_bulletCnt > 0 && m_attackLeftDirection && m_leftLock && m_vx < 0) {
		finishBullet();
	}
	if (damageFlag()) {
		return;
	}

	// 右へ歩き始める
	if (!m_rightLock && ableWalk() && right && (!left || !m_character_p->getLeftDirection())) { // 右へ歩く
		startMoveRight();
	}
	// 左へ歩き始める
	if (!m_leftLock && ableWalk() && left && (!right || m_character_p->getLeftDirection())) { // 左へ歩く
		startMoveLeft();
	}
	// アニメーション用にカウント
	if (m_moveLeft || m_moveRight) {
		m_runCnt++;
	}
}

// 移動
void StickAction::move(bool right, bool left, bool up, bool down) {
	if ((m_state == CHARACTER_STATE::STAND || m_state == CHARACTER_STATE::SQUAT) && m_grand && m_slashCnt == 0 && m_bulletCnt == 0) {
		// 移動方向へ向く
		if(left && !right && m_stepCnt == 0 && m_slidingCnt == 0){
			m_character_p->setLeftDirection(true);
		}
		if (right && !left && m_stepCnt == 0 && m_slidingCnt == 0) {
			m_character_p->setLeftDirection(false);
		}
	}
	// 左右の移動
	walk(right, left);
}

// ジャンプ
void StickAction::jump(int cnt) {
	// ダメージ状態ならジャンプできないためreturn
	if (damageFlag()) {
		// 受け身はできる
		if (cnt == 1 || m_boostCnt > 0) {
			if (m_boostCnt == 0) {
				// 受け身をした瞬間
				m_vy -= m_character_p->getJumpHeight() / 2;
				m_grand = false;
				m_preJumpCnt = -1;
				stopMoveLeft();
				stopMoveRight();
				// 効果音
				if (m_soundPlayer_p != nullptr) {
					m_soundPlayer_p->pushSoundQueue(m_character_p->getPassiveSound(),
						adjustPanSound(m_character_p->getCenterX(),
							m_soundPlayer_p->getCameraX()));
				}
			}
			// ダメージ状態が解除されるまではずっとBoost
			m_boostCnt = BOOST_TIME;
		}
		return;
	}
	// 斬撃中はジャンプ不可
	if (m_slashCnt > 0) { return; }
	// 宙に浮いたらジャンプ中止
	if (!m_grand) {
		m_preJumpCnt = -1;
		if (m_state == CHARACTER_STATE::PREJUMP) {
			setState(CHARACTER_STATE::STAND);
		}
	}
	// ジャンプの準備開始
	if (cnt > 0 && m_grand && m_preJumpCnt == -1) {
		m_preJumpCnt = 0;
		setState(CHARACTER_STATE::PREJUMP);
	}
	if (m_grand && m_preJumpCnt >= 0) {
		if (cnt == 0 || m_preJumpCnt == getPreJumpMax()) {
			// ジャンプ
			int rate = (100 * m_preJumpCnt) / getPreJumpMax();
			int power = (m_character_p->getJumpHeight() * rate) / 100;
			m_vy -= power;
			m_grand = false;
			m_preJumpCnt = -1;
			setState(CHARACTER_STATE::STAND);
			// 効果音
			if (m_soundPlayer_p != nullptr) {
				m_soundPlayer_p->pushSoundQueue(m_character_p->getJumpSound(),
					adjustPanSound(m_character_p->getCenterX(),
						m_soundPlayer_p->getCameraX()));
			}
		}
		else {
			// ジャンプ前の溜め中
			m_preJumpCnt++;
		}
	}
}

// 射撃攻撃
vector<Object*>* StickAction::bulletAttack(int gx, int gy) {
	if (damageFlag() && m_boostCnt == 0) {
		finishBullet();
		return nullptr;
	}
	// 射撃可能状態なら
	if (ableAttack()) {
		// 射撃不可能状態にして
		m_bulletCnt = m_character_p->getBulletRapid();
		// 撃つ方向へ向く
		if (m_character_p->getCharacterInfo()->sameBulletDirection()) {
			m_character_p->setLeftDirection(m_character_p->getCenterX() > gx);
		}
		startBullet();
	}
	// 攻撃を返す
	return m_character_p->bulletAttack(m_bulletCnt, gx, gy, m_soundPlayer_p);
}

// 斬撃攻撃
vector<Object*>* StickAction::slashAttack(int gx, int gy) {
	if (damageFlag() && m_boostCnt == 0) {
		if (m_slashCnt > 0) { finishSlash(); }
		return nullptr;
	}
	// 攻撃開始
	if (ableAttack()) {
		// ジャンプはキャンセル
		m_preJumpCnt = -1;
		if (m_state == CHARACTER_STATE::PREJUMP) {
			setState(CHARACTER_STATE::STAND);
		}
		m_attackLeftDirection = m_character_p->getCenterX() > gx;
		m_slashCnt = m_character_p->getSlashCountSum() + m_character_p->getSlashInterval();
		// 攻撃の方向へ向く
		m_character_p->setLeftDirection(m_attackLeftDirection);
		// 斬撃開始時の処理
		startSlash();
	}
	// 攻撃のタイミングじゃないならnullptrが返る
	return m_character_p->slashAttack(m_attackLeftDirection, m_slashCnt, m_grand, m_soundPlayer_p);
}

// スライディング攻撃
vector<Object*>* StickAction::slidingAttack() {
	if (m_slidingCnt == 0) {
		return nullptr;
	}
	// 攻撃のタイミングじゃないならnullptrが返る
	return m_character_p->slidingAttack(m_slidingCnt == SLIDING_SPEED, m_soundPlayer_p);
}


/*
* ヴァルキリア用Action 斬撃時に移動する
*/
ValkiriaAction::ValkiriaAction(Character* character, SoundPlayer* soundPlayer_p) :
	StickAction(character, soundPlayer_p)
{
	m_slashNow = false;
}

CharacterAction* ValkiriaAction::createCopy(vector<Character*> characters) {
	ValkiriaAction* res = nullptr;
	for (unsigned int i = 0; i < characters.size(); i++) {
		if (m_character_p->getId() == characters[i]->getId()) {
			res = new ValkiriaAction(characters[i], m_soundPlayer_p);
			res->setSlashNow(m_slashNow);
			// コピーする
			setParam(res);
			break;
		}
	}
	return res;
}

// 着地 ヴァルキリアは斬撃中に着地しても着地モーションにならない
void ValkiriaAction::setGrand(bool grand) {
	if (m_vy > 0) { // 着地モーションになる
		if (m_slashCnt == 0) { // 斬撃中ではない
			m_landCnt = LAND_TIME;
			// 効果音
			if (m_soundPlayer_p != nullptr) {
				m_soundPlayer_p->pushSoundQueue(m_character_p->getLandSound(),
					adjustPanSound(m_character_p->getCenterX(),
						m_soundPlayer_p->getCameraX()));
			}
		}
	}
	m_grand = grand;
	finishBoost();
	if (m_state == CHARACTER_STATE::DAMAGE && m_damageCnt == 0) {
		m_vx = 0;
		m_vy = 0;
		m_state = CHARACTER_STATE::STAND;
	}
}

void ValkiriaAction::startSlash() {
	if (m_attackLeftDirection) {
		if (!m_leftLock) {
			m_vx -= SLASH_MOVE_SPEED;
			m_slashNow = true;
		}
	}
	else {
		if (!m_rightLock) {
			m_vx += SLASH_MOVE_SPEED;
			m_slashNow = true;
		}
	}
}

void ValkiriaAction::finishSlash() {
	CharacterAction::finishSlash();
	if (m_slashNow) {
		if (m_attackLeftDirection && !m_leftLock) {
			m_vx += SLASH_MOVE_SPEED;
		}
		else if (!m_rightLock) {
			m_vx -= SLASH_MOVE_SPEED;
		}
	}
	m_slashNow = false;
}

// ダメージを受ける ヴァルキリアは斬撃中はHPが減るだけ
void ValkiriaAction::damage(int vx, int vy, int damageValue) {
	if (m_slashCnt > 0) {
		// HP減少
		m_character_p->damageHp(damageValue / 2);
	}
	else {
		CharacterAction::damage(vx, vy, damageValue);
	}
}


/*
* 空を飛ぶキャラ
*/
FlightAction::FlightAction(Character* character, SoundPlayer* soundPlayer_p):
	CharacterAction(character, soundPlayer_p)
{

}

CharacterAction* FlightAction::createCopy(std::vector<Character*> characters) {
	CharacterAction* res = nullptr;
	for (unsigned int i = 0; i < characters.size(); i++) {
		if (m_character_p->getId() == characters[i]->getId()) {
			res = new FlightAction(characters[i], m_soundPlayer_p);
			// コピーする
			setParam(res);
			break;
		}
	}
	return res;
}

// キャラの画像を状態(state)に応じて変更
void FlightAction::switchHandle() {
	// セット前の画像のサイズ
	int x1 = 0, y1 = 0, x2 = 0, y2 = 0;
	bool nowLeftDirection = m_character_p->getLeftDirection();
	m_character_p->setLeftDirection(m_prevLeftDirection);
	m_character_p->getAtariArea(&x1, &y1, &x2, &y2);

	if (m_grand) { // 地面にいるとき
		switch (m_state) {
		case CHARACTER_STATE::STAND: //立ち状態
			if (m_bulletCnt > 0) {
				if (m_runCnt != -1) {
					m_character_p->switchRunBullet(m_runCnt);
				}
				else {
					m_character_p->switchAirBullet();
				}
			}
			else if (m_slashCnt > 0) {
				m_character_p->switchAirSlash();
			}
			else if (m_runCnt != -1) {
				m_character_p->switchRun(m_runCnt);
			}
			else {
				m_character_p->switchStand();
			}
			break;
		case CHARACTER_STATE::DAMAGE:
			m_character_p->switchDamage();
			break;
		}
	}
	else { // 宙にいるとき
		switch (m_state) {
		case CHARACTER_STATE::STAND: //立ち状態(なにもなしの状態)
			if (m_slashCnt > 0) {
				m_character_p->switchAirSlash();
			}
			else if (m_bulletCnt > 0) {
				if (m_runCnt != -1) {
					m_character_p->switchRunBullet(m_runCnt);
				}
				else {
					m_character_p->switchAirBullet();
				}
			}
			else if (m_runCnt != -1) {
				m_character_p->switchRun(m_runCnt);
			}
			else if (m_vy != 0) {
				m_character_p->switchDown();
			}
			else {
				m_character_p->switchJump();
			}
			break;
		case CHARACTER_STATE::DAMAGE:
			m_character_p->switchDamage();
			break;
		}
	}

	m_character_p->setLeftDirection(nowLeftDirection);

	// セット後の画像のサイズ
	int afterX1 = 0, afterY1 = 0, afterX2 = 0, afterY2 = 0;
	m_character_p->getAtariArea(&afterX1, &afterY1, &afterX2, &afterY2);

	// サイズ変更による位置調整
	afterChangeGraph(x1, afterX1, y1, afterY1, x2, afterX2, y2, afterY2);

}

void FlightAction::damageAction() {
	// ダメージ受け状態は最低１秒近くある
	if (m_damageCnt > 0) {
		m_damageCnt--;
		if (m_damageCnt == 0 && !m_heavy) {
			m_vx = 0;
			m_vy = 0;
			m_state = CHARACTER_STATE::STAND;
		}
	}
}
void FlightAction::otherAction() {
	// アニメーション用のカウント
	if (m_landCnt > 0) { m_landCnt--; }
	if (m_boostCnt > 0) {
		m_boostCnt--;
		if (m_boostCnt == 0) {
			finishBoost();
		}
	}
	if (m_stepCnt > 0) { m_stepCnt--; }
	if (m_slidingCnt > 0) {
		m_slidingCnt--;
		if (m_slidingDone == 1) { m_vx--; }
		if (m_slidingDone == 2) { m_vx++; }
	}
}

void FlightAction::walk(bool right, bool left, bool up, bool down) {
	// 右へ歩くのをやめる
	if (!right || m_rightLock || m_state == CHARACTER_STATE::SQUAT || damageFlag()) {
		stopMoveRight();
	}
	if (m_slashCnt > 0 && !m_attackLeftDirection && (m_rightLock || damageFlag())) {
		finishSlash();
	}
	// 左へ歩くのをやめる
	if (!left || m_leftLock || m_state == CHARACTER_STATE::SQUAT || damageFlag()) {
		stopMoveLeft();
	}
	if (m_slashCnt > 0 && m_attackLeftDirection && (m_leftLock || damageFlag())) {
		finishSlash();
	}
	// 上へ歩くのをやめる
	if (!up || m_upLock || damageFlag()) {
		stopMoveUp();
	}
	// 下へ歩くのをやめる
	if (!down || m_downLock || damageFlag()) {
		stopMoveDown();
	}
	if (damageFlag()) {
		return;
	}
	// 右へ歩き始める
	if (!m_rightLock && !m_moveRight && !m_moveLeft && right && (!left || !m_character_p->getLeftDirection()) && m_state != CHARACTER_STATE::SQUAT) { // 右へ歩く
		startMoveRight();
	}
	// 左へ歩き始める
	if (!m_leftLock && !m_moveRight && !m_moveLeft && left && (!right || m_character_p->getLeftDirection()) && m_state != CHARACTER_STATE::SQUAT) { // 左へ歩く
		startMoveLeft();
	}
	// 上へ歩き始める
	if (!m_upLock && !m_moveDown && !m_moveUp && up && !down) { // 上へ歩く
		startMoveUp();
	}
	// 下へ歩き始める
	if (!m_downLock && !m_moveUp && !m_moveDown && down && !up) { // 下へ歩く
		startMoveDown();
	}
	// アニメーション用にカウント
	if (m_moveLeft || m_moveRight) {
		m_runCnt++;
	}
}

void FlightAction::action() {
	flightAction();
	CharacterAction::action();
}

void FlightAction::flightAction() {
	// 走りによる速度変化
	if (m_moveLeft) {
		m_runVx = max(m_runVx - 1, -m_character_p->getMoveSpeed());
	}
	else if (m_runVx < 0) {
		m_runVx += 1;
	}
	if (m_moveRight) {
		m_runVx = min(m_runVx + 1, m_character_p->getMoveSpeed());
	}
	else if (m_runVx > 0) {
		m_runVx -= 1;
	}
	if (m_moveUp) {
		m_runVy = max(m_runVy - 1, -m_character_p->getMoveSpeed());
	}
	else if (m_runVy < 0) {
		m_runVy += 1;
	}
	if (m_moveDown) {
		m_runVy = min(m_runVy + 1, m_character_p->getMoveSpeed());
	}
	else if (m_runVy > 0) {
		m_runVy -= 1;
	}
}

// 移動 引数は４方向分
void FlightAction::move(bool right, bool left, bool up, bool down) {
	if ((m_state == CHARACTER_STATE::STAND || m_state == CHARACTER_STATE::SQUAT) && m_slashCnt == 0 && m_bulletCnt == 0) {
		// 移動方向へ向く
		if (left && !right) {
			m_character_p->setLeftDirection(true);
		}
		if (right && !left) {
			m_character_p->setLeftDirection(false);
		}
	}
	// 移動
	walk(right, left, up, down);
}

// ジャンプ cntフレーム目
void FlightAction::jump(int cnt) {
	
}

void FlightAction::setBoost(bool leftDirection) {
	if ((leftDirection && m_leftLock) || (!leftDirection && m_rightLock)) {
		return;
	}
	if (!damageFlag() && !m_grand && !m_boostDone) {
		m_boostCnt = BOOST_TIME;
		finishBullet();
		finishSlash();
		if (leftDirection) {
			m_vx -= BOOST_SPEED;
			m_boostDone = 2;
		}
		else {
			m_vx += BOOST_SPEED;
			m_boostDone = 1;
		}
		m_character_p->setLeftDirection(leftDirection);
	}
}

// 射撃攻撃
vector<Object*>* FlightAction::bulletAttack(int gx, int gy) {
	if (damageFlag() && m_boostCnt == 0) {
		finishBullet();
		return nullptr;
	}
	// 射撃可能状態なら
	if (ableAttack()) {
		// 射撃不可能状態にして
		m_bulletCnt = m_character_p->getBulletRapid();
		// 撃つ方向へ向く
		if (m_character_p->getCharacterInfo()->sameBulletDirection()) {
			m_character_p->setLeftDirection(m_character_p->getCenterX() > gx);
		}
		startBullet();
	}
	// 攻撃を返す
	return m_character_p->bulletAttack(m_bulletCnt, gx, gy, m_soundPlayer_p);
}

// 斬撃攻撃
vector<Object*>* FlightAction::slashAttack(int gx, int gy) {
	if (damageFlag() && m_boostCnt == 0) {
		if (m_slashCnt > 0) { finishSlash(); }
		return nullptr;
	}
	// 攻撃開始
	if (ableAttack()) {
		m_attackLeftDirection = m_character_p->getCenterX() > gx;
		m_slashCnt = m_character_p->getSlashCountSum() + m_character_p->getSlashInterval();
		// 攻撃の方向へ向く
		m_character_p->setLeftDirection(m_attackLeftDirection);
		// 斬撃開始時の処理
		startSlash();
	}
	// 攻撃のタイミングじゃないならnullptrが返る
	return m_character_p->slashAttack(m_attackLeftDirection, m_slashCnt, m_grand, m_soundPlayer_p);
}


/*
* コハル用Action 斬撃時に移動する
*/
KoharuAction::KoharuAction(Character* character, SoundPlayer* soundPlayer_p) :
	StickAction(character, soundPlayer_p)
{

}

CharacterAction* KoharuAction::createCopy(vector<Character*> characters) {
	CharacterAction* res = nullptr;
	for (unsigned int i = 0; i < characters.size(); i++) {
		if (m_character_p->getId() == characters[i]->getId()) {
			res = new KoharuAction(characters[i], m_soundPlayer_p);
			// コピーする
			setParam(res);
			break;
		}
	}
	return res;
}

// 射撃攻撃
vector<Object*>* KoharuAction::bulletAttack(int gx, int gy) {
	if (damageFlag() && m_boostCnt == 0) {
		finishBullet();
		return nullptr;
	}
	// 射撃可能状態なら
	if (ableAttack()) {
		// 射撃不可能状態にして
		m_bulletCnt = m_character_p->getBulletRapid();
		stopMoveRight();
		stopMoveLeft();
		if (m_bulletCnt == 0) {
			if (gx > m_character_p->getCenterX()) {
				m_vx -= BULLET_MOVE_SPEED;
			}
			else {
				m_vx += BULLET_MOVE_SPEED;
			}
		}
		// 撃つ方向へ向く
		if (m_character_p->getCharacterInfo()->sameBulletDirection()) {
			m_character_p->setLeftDirection(m_character_p->getCenterX() > gx);
		}
		startBullet();
	}
	// 攻撃を返す
	return m_character_p->bulletAttack(m_bulletCnt, gx, gy, m_soundPlayer_p);
}

void KoharuAction::startBullet() {
	if (m_character_p->getLeftDirection()) {
		m_vx += BULLET_MOVE_SPEED;
	}
	else {
		m_vx -= BULLET_MOVE_SPEED;
	}
}

void KoharuAction::finishBullet() {
	if (m_bulletCnt > 0) { 
		StickAction::finishBullet();
	}
	if (m_character_p->getLeftDirection() && !m_rightLock) {
		m_vx -= BULLET_MOVE_SPEED;
	}
	else if(!m_leftLock){
		m_vx += BULLET_MOVE_SPEED;
	}
}

bool KoharuAction::ableAttack() const {
	return !(m_bulletCnt > 0 || m_slashCnt > 0);
}

bool KoharuAction::ableWalk() const {
	return StickAction::ableWalk() && m_bulletCnt == 0;
}


/*
* 行動開始前のBoss
*/
BossFreezeAction::BossFreezeAction(Character* character, SoundPlayer* soundPlayer_p) :
	CharacterAction(character, soundPlayer_p)
{

}

CharacterAction* BossFreezeAction::createCopy(vector<Character*> characters) {
	CharacterAction* res = nullptr;
	for (unsigned int i = 0; i < characters.size(); i++) {
		if (m_character_p->getId() == characters[i]->getId()) {
			res = new BossFreezeAction(characters[i], m_soundPlayer_p);
			// コピーする
			setParam(res);
			break;
		}
	}
	return res;
}
void BossFreezeAction::switchHandle() {
	m_character_p->switchSpecial1();
}


/*
* Boss1: サン
*/
SunAction::SunAction(Character* character, SoundPlayer* soundPlayer_p, bool duplicationFlag) :
	FlightAction(character, soundPlayer_p)
{
	m_state = CHARACTER_STATE::INIT;
	m_initCnt = -FPS_N;
	m_initHp = m_character_p->getHp();
	if (!duplicationFlag) {
		m_character_p->setHp(min(1, m_initHp));
	}
	m_startAnimeCnt = 0;
}

CharacterAction* SunAction::createCopy(vector<Character*> characters) {
	SunAction* res = nullptr;
	for (unsigned int i = 0; i < characters.size(); i++) {
		if (m_character_p->getId() == characters[i]->getId()) {
			res = new SunAction(characters[i], m_soundPlayer_p, true);
			// コピーする
			setParam(res);
			res->setInitHp(m_initHp);
			res->setInitCnt(m_initCnt);
			res->setHideFlag(m_hideFlag);
			res->setStartAnimeCnt(m_startAnimeCnt);
			break;
		}
	}
	return res;
}

void SunAction::action() {
	// 状態(state)に応じて画像をセット
	switchHandle();
	if (m_hideFlag && m_initCnt != 0) {
		// 隠れ途中
		m_initCnt--;
	}
	else if (!m_hideFlag && m_initCnt != NOT_HIDE_CNT) {
		// 出現途中
		m_initCnt++;
		if (m_state == CHARACTER_STATE::INIT) {
			m_character_p->setHp(min(m_character_p->getHp() + 10, m_initHp));
		}
		if (m_initCnt == NOT_HIDE_CNT && m_character_p->getHp() == m_initHp) {
			m_state = CHARACTER_STATE::STAND;
		}
	}
	else {
		m_startAnimeCnt++;
		// 隠れ・出現の開始
		if (m_hideFlag) {
			// 現在隠れ状態
			m_bulletCnt = 1;
			slashAction();
			damageAction();
			otherAction();
			moveAction();
			flightAction();
			if (m_startAnimeCnt > 300 && (GetRand(120) == 0 || m_startAnimeCnt == 600)) {
				m_hideFlag = false;
				m_initCnt = 0;
				m_startAnimeCnt = 0;
				m_runVx = 0;
				m_runVy = 0;
			}
		}
		else {
			// 現在出現状態
			bulletAction();
			slashAction();
			damageAction();
			otherAction();
			if (m_startAnimeCnt > 300 && (GetRand(300) == 0 || m_startAnimeCnt == 600)) {
				m_hideFlag = true;
				m_initCnt = NOT_HIDE_CNT - 1;
				m_startAnimeCnt = 0;
			}
		}
	}
}

// 状態に応じて画像セット
void SunAction::switchHandle() {

	m_prevLeftDirection = false;
	m_character_p->setLeftDirection(false);

	if (m_initCnt != NOT_HIDE_CNT) {
		// セット前の画像のサイズ
		int x1 = 0, y1 = 0, x2 = 0, y2 = 0;
		bool nowLeftDirection = false;
		m_character_p->setLeftDirection(false);
		m_character_p->getAtariArea(&x1, &y1, &x2, &y2);

		// 画像セット
		if (m_initCnt == 0 && m_hideFlag) {
			m_character_p->switchSpecial1();
		}
		else {
			m_character_p->switchInit(m_initCnt);
		}

		m_character_p->setLeftDirection(nowLeftDirection);

		// セット後の画像のサイズ
		int afterX1 = 0, afterY1 = 0, afterX2 = 0, afterY2 = 0;
		m_character_p->getAtariArea(&afterX1, &afterY1, &afterX2, &afterY2);

		// サイズ変更による位置調整
		afterChangeGraph(x1, afterX1, y1, afterY1, x2, afterX2, y2, afterY2);
	}
	else {
		FlightAction::switchHandle();
	}
}


/*
* Boss2: アーカイブ
*/
ArchiveAction::ArchiveAction(Character* character, SoundPlayer* soundPlayer_p, bool duplicationFlag) :
	StickAction(character, soundPlayer_p)
{
	m_state = CHARACTER_STATE::INIT;
	m_initCompFlag = false;
	m_initHp = m_character_p->getHp();
	if (!duplicationFlag) {
		m_character_p->setHp(min(1, m_initHp));
	}
	m_jumpCnt = 0;
	m_slashVx = 0;
}

CharacterAction* ArchiveAction::createCopy(vector<Character*> characters) {
	ArchiveAction* res = nullptr;
	for (unsigned int i = 0; i < characters.size(); i++) {
		if (m_character_p->getId() == characters[i]->getId()) {
			res = new ArchiveAction(characters[i], m_soundPlayer_p, true);
			// コピーする
			setParam(res);
			res->setInitCompFlag(m_initCompFlag);
			res->setInitHp(m_initHp);
			res->setSlashVx(m_slashVx);
			break;
		}
	}
	return res;
}

void ArchiveAction::action() {
	if (!m_initCompFlag) {
		// 状態(state)に応じて画像をセット
		switchHandle();
		if (m_state == CHARACTER_STATE::INIT) {
			m_character_p->setHp(min(m_character_p->getHp() + 10, m_initHp));
		}
		if (m_character_p->getHp() == m_initHp) {
			m_state = CHARACTER_STATE::STAND;
			m_initCompFlag = true;
		}
	}
	else {
		if (m_slashVx > 0) {
			m_slashVx -= 2;
			if (m_attackLeftDirection) {
				if (!m_leftLock) {
					m_vx += 2;
				}
			}
			else {
				if (!m_rightLock) {
					m_vx -= 2;
				}
			}
		}
		StickAction::action();
		m_landCnt = 0;
		if (m_state == CHARACTER_STATE::PREJUMP) {
			m_state = CHARACTER_STATE::STAND;
		}
	}
}

void ArchiveAction::startSlash() {
	if (m_attackLeftDirection) {
		if (!m_leftLock) {
			m_vx -= SLASH_MOVE_SPEED;
		}
	}
	else {
		if (!m_rightLock) {
			m_vx += SLASH_MOVE_SPEED;
		}
	}
	m_slashVx = SLASH_MOVE_SPEED;
}

void ArchiveAction::finishSlash() {
	CharacterAction::finishSlash();
	if (m_slashVx > 0) {
		if (m_attackLeftDirection && !m_leftLock) {
			m_vx += m_slashVx;
		}
		else if (!m_rightLock) {
			m_vx -= m_slashVx;
		}
	}
	m_slashVx = 0;
}

bool ArchiveAction::ableDamage() const {
	return StickAction::ableDamage() && m_initCompFlag;
}