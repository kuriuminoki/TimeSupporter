#include "Brain.h"
#include "Character.h"
#include "CharacterAction.h"
#include "Camera.h"
#include "Control.h"
#include "Object.h"
#include "Define.h"
#include "DxLib.h"
#include <cmath>


using namespace std;


// クラス名
const char* Brain::BRAIN_NAME = "Brain";
const char* KeyboardBrain::BRAIN_NAME = "KeyboardBrain";
const char* Freeze::BRAIN_NAME = "Freeze";
const char* NormalAI::BRAIN_NAME = "NormalAI";
const char* ParabolaAI::BRAIN_NAME = "ParabolaAI";
const char* FollowNormalAI::BRAIN_NAME = "FollowNormalAI";
const char* FollowParabolaAI::BRAIN_NAME = "FollowParabolaAI";
const char* ValkiriaAI::BRAIN_NAME = "ValkiriaAI";
const char* FlightAI::BRAIN_NAME = "FlightAI";
const char* FollowFlightAI::BRAIN_NAME = "FollowFlightAI";
const char* HierarchyAI::BRAIN_NAME = "HierarchyAI";
const char* FrenchAI::BRAIN_NAME = "FrenchAI";
const char* CategoryZAI::BRAIN_NAME = "CategoryZAI";
const char* SunAI::BRAIN_NAME = "SunAI";

// クラス名からBrainを作成する関数
Brain* createBrain(const string brainName, const Camera* camera_p) {
	Brain* brain = nullptr;
	if (brainName == KeyboardBrain::BRAIN_NAME) {
		brain = new KeyboardBrain(camera_p);
	}
	else if (brainName == NormalAI::BRAIN_NAME) {
		brain = new NormalAI();
	}
	else if (brainName == ParabolaAI::BRAIN_NAME) {
		brain = new ParabolaAI();
	}
	else if (brainName == FollowNormalAI::BRAIN_NAME) {
		brain = new FollowNormalAI();
	}
	else if (brainName == FollowParabolaAI::BRAIN_NAME) {
		brain = new FollowParabolaAI();
	}
	else if (brainName == Freeze::BRAIN_NAME) {
		brain = new Freeze();
	}
	else if (brainName == ValkiriaAI::BRAIN_NAME) {
		brain = new ValkiriaAI();
	}
	else if (brainName == FlightAI::BRAIN_NAME) {
		brain = new FlightAI();
	}
	else if (brainName == FollowFlightAI::BRAIN_NAME) {
		brain = new FollowFlightAI();
	}
	else if (brainName == HierarchyAI::BRAIN_NAME) {
		brain = new HierarchyAI();
	}
	else if (brainName == FrenchAI::BRAIN_NAME) {
		brain = new FrenchAI();
	}
	else if (brainName == CategoryZAI::BRAIN_NAME) {
		brain = new CategoryZAI();
	}
	else if (brainName == SunAI::BRAIN_NAME) {
		brain = new SunAI();
	}
	return brain;
}


// targetを持つAI用、targetIdに対応するCharacterをcopyのtargetにセット
void copyTarget(std::vector<Character*> characters, int targetId, NormalAI* copy) {
	if (targetId != -1) {
		for (unsigned int i = 0; i < characters.size(); i++) {
			if (targetId == characters[i]->getId()) {
				copy->setTarget(characters[i]);
				break;
			}
		}
	}
}

// followを持つAI用、followIdに対応するCharacterをcopyのfollowにセット
void copyFollow(std::vector<Character*> characters, int followId, FollowNormalAI* copy) {
	if (followId != -1) {
		for (unsigned int i = 0; i < characters.size(); i++) {
			if (followId == characters[i]->getId()) {
				copy->setFollow(characters[i]);
				break;
			}
		}
	}
}


// Brainクラス
Brain::Brain() {
	m_characterAction_p = nullptr;
}


/*
* キーボード
*/
KeyboardBrain::KeyboardBrain(const Camera* camera) {
	m_camera_p = camera;
}

void KeyboardBrain::bulletTargetPoint(int& x, int& y) {
	// マウスの位置
	int mouseX, mouseY;
	GetMousePoint(&mouseX, &mouseY);

	// カメラで座標を補正
	m_camera_p->getMouse(&mouseX, &mouseY);

	x = mouseX;
	y = mouseY;
}

void KeyboardBrain::slashTargetPoint(int& x, int& y) {
	bulletTargetPoint(x, y);
}

// 話しかけたり扉入ったり
bool KeyboardBrain::actionOrder() {
	return controlW() == 1;
}

// 移動（上下左右の入力）
void KeyboardBrain::moveOrder(int& right, int& left, int& up, int& down) {
	right = controlD();
	left = controlA();
	up = controlW();
	down = controlS();
}

// ジャンプ
int KeyboardBrain::jumpOrder() {
	return controlSpace();
}

// しゃがみ
int KeyboardBrain::squatOrder() {
	return controlS();
}

// 近距離攻撃
int KeyboardBrain::slashOrder() {
	return rightClick();
}

// 遠距離攻撃
int KeyboardBrain::bulletOrder() {
	return leftClick();
}


/*
* Normal AI
*/
NormalAI::NormalAI() {
	m_target_p = nullptr;
	m_gx = 0;
	m_gy = 0;
	m_rightKey = 0;
	m_leftKey = 0;
	m_upKey = 0;
	m_downKey = 0;
	m_jumpCnt = 0;
	m_squatCnt = 0;
	m_moveCnt = 0;
	m_prevX = 0;
}


Brain* NormalAI::createCopy(std::vector<Character*> characters, const Camera* camera) {
	NormalAI* res = new NormalAI();
	copyTarget(characters, getTargetId(), res);
	setParam(res);
	return res;
}

void NormalAI::setParam(NormalAI* brain) {
	// 移動用
	brain->setRightKey(m_rightKey);
	brain->setLeftKey(m_leftKey);
	brain->setUpKey(m_upKey);
	brain->setDownKey(m_downKey);
	// ジャンプの長さ
	brain->setJumpCnt(m_jumpCnt);
	// しゃがむ長さ
	brain->setSquatCnt(m_squatCnt);
	// 移動目標
	brain->setGx(m_gx);
	brain->setGy(m_gy);
	brain->setMoveCnt(m_moveCnt);
	brain->setPrevX(m_prevX);
}

void NormalAI::setCharacterAction(const CharacterAction* characterAction) {
	m_characterAction_p = characterAction;
	// 目標地点は現在地に設定
	m_gx = m_characterAction_p->getCharacter()->getCenterX();
	m_gy = m_characterAction_p->getCharacter()->getCenterY();
}

void NormalAI::bulletTargetPoint(int& x, int& y) {
	if (m_target_p == nullptr) {
		x = 0;
		y = 0;
	}
	else { // ターゲットに向かって射撃攻撃
		x = m_target_p->getCenterX() + (GetRand(BULLET_ERROR) - BULLET_ERROR / 2);
		y = m_target_p->getCenterY() + (GetRand(BULLET_ERROR) - BULLET_ERROR / 2);
	}
}

void NormalAI::slashTargetPoint(int& x, int& y) {
	if (m_target_p == nullptr) {
		x = 0;
		y = 0;
	}
	else { // ターゲットに向かって射撃攻撃
		x = m_target_p->getCenterX();
		y = m_target_p->getCenterY();
	}
}

void NormalAI::moveOrder(int& right, int& left, int& up, int& down) {

	// 動かないキャラ
	if (m_characterAction_p->getCharacter()->getCharacterInfo()->moveSpeed() == 0) { 
		return;
	}

	// 現在地
	int x = m_characterAction_p->getCharacter()->getCenterX();
	int y = m_characterAction_p->getCharacter()->getCenterY();

		// (壁につっかえるなどで)移動できてないから諦める
	if (m_moveCnt >= GIVE_UP_MOVE_CNT) {
			m_gx = x;
			m_gy = y;
	}

	// 目標地点設定
	bool alreadyGoal = m_gx > x - GX_ERROR && m_gx < x + GX_ERROR;
	// ダメージを受けたらリセット
	if (m_characterAction_p->getState() == CHARACTER_STATE::DAMAGE) {
		m_gx = x, m_gy = y;
	}
	else if (alreadyGoal && GetRand(MOVE_RAND) == 0) {
		if (m_target_p != nullptr && abs(x - m_target_p->getCenterX()) < TARGET_DISTANCE) {
			// targetについていく
			setGoalToTarget();
		}
		else {
			// ランダムに設定
			m_gx = GetRand(400) - 200;
			m_gx += x;
		}
		if (abs(x - m_gx) < 50) { m_gx = x; }
	}
	stickOrder(right, left, up, down);
}

// 上下移動するAIが使う
void NormalAI::moveUpDownOrder(int x, int y, bool& tryFlag) {
		// (壁につっかえるなどで)移動できてないから諦める
	if (m_moveCnt >= GIVE_UP_MOVE_CNT || (m_gy > y && m_characterAction_p->getGrand())) {
			m_gx = x;
			m_gy = y;
			tryFlag = false;
	}

	// 壁にぶつかったから上下移動
	if ((m_rightKey > 0 && m_characterAction_p->getRightLock()) || (m_leftKey > 0 && m_characterAction_p->getLeftLock())) {
		if (!tryFlag) {
			if (GetRand(100) < 50) {
				m_gy = y - 1000;
			}
			else {
				m_gy = y + 1000;
			}
			tryFlag = true;
		}
		else if (m_upKey > 0 && m_characterAction_p->getUpLock()) {
			m_gy = y + 2000;
		}
		else if (m_downKey > 0 && m_characterAction_p->getDownLock()) {
			m_gy = y - 2000;
		}
	}
	else {
		if (tryFlag) { m_gy = y; }
		tryFlag = false;
	}
}

// スティック操作
void NormalAI::stickOrder(int& right, int& left, int& up, int& down) {
	// 現在地
	int x = m_characterAction_p->getCharacter()->getCenterX();
	int y = m_characterAction_p->getCharacter()->getY() + m_characterAction_p->getCharacter()->getHeight();

	// 目標に向かって走る
	if (m_gx > x + GX_ERROR) {
		m_rightKey++;
		m_leftKey = 0;
		m_moveCnt++;
	}
	else if (m_gx < x - GX_ERROR) {
		m_rightKey = 0;
		m_leftKey++;
		m_moveCnt++;
	}
	else {
		if (m_moveCnt == 0) {
			m_rightKey = 0;
			m_leftKey = 0;
		}
		else {
			m_moveCnt = 0;
		}
	}

	// 目標に向かって上下移動
	if (m_gy > y + GY_ERROR && !m_characterAction_p->getGrand()) {
		m_downKey++;// 宙にいるなら下へ移動、そうじゃないならしゃがんじゃうから
		m_upKey = 0;
	}
	else if (m_gy < y - GY_ERROR) {
		m_downKey = 0;
		m_upKey++;
	}
	else {
		m_downKey = 0;
		m_upKey = 0;
	}

	// 反映
	right = m_rightKey;
	left = m_leftKey;
	up = m_upKey;
	down = m_downKey;
}

int NormalAI::jumpOrder() {
	// ダメージを食らったらリセット
	if (m_characterAction_p->getState() == CHARACTER_STATE::DAMAGE) {
		m_jumpCnt = 0;
		// 受け身
		if (GetRand(120) == 0) { return 1; }
	}

	int maxJump = m_characterAction_p->getPreJumpMax();
	int minJump = maxJump / 3;

	if (m_characterAction_p->getPreJumpCnt() == -1 && m_characterAction_p->getGrand()) {
		// ランダムでジャンプ
		if (m_squatCnt == 0 && GetRand(99) == 0) { m_jumpCnt = GetRand(maxJump - minJump) + minJump; }

		// 壁にぶつかったからジャンプ
		if (m_rightKey > 0 && m_characterAction_p->getRightLock()) { m_jumpCnt = maxJump; }
		else if (m_leftKey > 0 && m_characterAction_p->getLeftLock()) { m_jumpCnt = maxJump; }
	}

	if (m_jumpCnt > 0) { m_jumpCnt--; }
	return m_jumpCnt == 0 ? 0 : maxJump - m_jumpCnt;
}

int NormalAI::squatOrder() {
	// ダメージを食らったらリセット
	if (m_characterAction_p->getState() == CHARACTER_STATE::DAMAGE) {
		m_squatCnt = 0;
	}
	if (m_characterAction_p->getBulletCnt() > 0 || m_characterAction_p->getSlashCnt() > 0) {
		m_squatCnt = 0;
	}

	// 目標地点にいないならしゃがまない
	int x = m_characterAction_p->getCharacter()->getCenterX();
	bool alreadyGoal = m_gx > x - GX_ERROR && m_gx < x + GX_ERROR;
	if (!alreadyGoal) { m_squatCnt = 0; }

	// ランダムでしゃがむ
	if (alreadyGoal && m_characterAction_p->getGrand() && GetRand(99) == 0) {
		m_squatCnt = GetRand(120) + 60;
	}

	if (m_squatCnt > 0) { m_squatCnt--; }
	return m_squatCnt;
}

int NormalAI::slashOrder() {
	// 斬撃攻撃を持っていない
	if (!m_characterAction_p->getCharacter()->haveSlashAttack()) {
		return 0;
	}
	// ターゲットがいない
	if (m_target_p == nullptr || m_target_p->getHp() == 0) {
		return 0;
	}
	// 遠距離の敵には斬撃しない
	if (abs(m_target_p->getCenterX() - m_characterAction_p->getCharacter()->getCenterX()) > getSlashReach()) {
		return 0;
	}
	// ランダムで斬撃
	if (GetRand(120) == 0) {
		return 1;
	}
	return 0;
}

int NormalAI::bulletOrder() {
	// 射撃攻撃を持っていない
	if (!m_characterAction_p->getCharacter()->haveBulletAttack()) {
		return 0;
	}
	// ターゲットがいない
	if (m_target_p == nullptr || m_target_p->getHp() == 0) {
		return 0;
	}
	// 遠距離の敵には射撃しない
	int x = m_characterAction_p->getCharacter()->getCenterX();
	if (abs(x - m_target_p->getCenterX()) > TARGET_DISTANCE) {
		return 0;
	}
	// ランダムで射撃
	int rapid = m_characterAction_p->getCharacter()->getAttackInfo()->bulletRapid();
	if (GetRand(rapid) == 0) {
		return m_characterAction_p->getBulletCnt() + 1;
	}
	return 0;
}

void NormalAI::setGoalToTarget() {
	// targetについていく
	int NEAR_TARGET = 2000;
	if (!m_characterAction_p->getCharacter()->haveBulletAttack()) {
		NEAR_TARGET = 500; // 近距離攻撃しかないからより近づく
	}
	int targetX1 = 0, targetY1 = 0, targetX2 = 0, targetY2 = 0;
	m_target_p->getAtariArea(&targetX1, &targetY1, &targetX2, &targetY2);
	m_gx = (targetX1 + targetX2) / 2 + GetRand(NEAR_TARGET) - NEAR_TARGET / 2;
}

// 攻撃対象を決める(targetのままか、characterに変更するか)
void NormalAI::searchTarget(const Character* character) {
	if (GetRand(99) < 50) {
		int x = m_characterAction_p->getCharacter()->getCenterX();
		// 距離が遠い
		if (abs(x - character->getCenterX()) > TARGET_DISTANCE) {
			return;
		}
		// 味方じゃなければ&中立じゃなければ
		int groupId = character->getGroupId();
		if (groupId != m_characterAction_p->getCharacter()->getGroupId() && groupId != -1) {
			m_target_p = character;
		}
	}
}

// 攻撃対象を変更する必要があるならtrueでアピールする。
bool NormalAI::needSearchTarget() const {
	// ターゲットを決めていないから
	if (m_target_p == nullptr || m_target_p->getHp() == 0) {
		return true;
	}
	// 気まぐれで、or不適切な相手だから
	if (GetRand(99) == 0 || m_target_p->getGroupId() == -1) {
		return true;
	}
	// 今のターゲットは距離が遠いから
	if (abs(m_target_p->getCenterX() - m_characterAction_p->getCharacter()->getCenterX()) > TARGET_DISTANCE) {
		return true;
	}
	return false;
}

int  NormalAI::getTargetId() const { 
	return m_target_p == nullptr ? -1 : m_target_p->getId();
}

const char*  NormalAI::getTargetName() const { 
	return m_target_p == nullptr ? "" : m_target_p->getName().c_str();
}

// 目標地点へ移動するだけ 達成済みならtrueで何もしない
bool NormalAI::moveGoalOrder(int& right, int& left, int& up, int& down, int& jump) {
	// 現在地
	int x = m_characterAction_p->getCharacter()->getCenterX();
	int y = m_characterAction_p->getCharacter()->getY() + m_characterAction_p->getCharacter()->getHeight();
	// 1秒ごとに座標を確認
	if (m_moveCnt % FPS_N == FPS_N - 1) {
		// (壁につっかえるなどで)移動できてないから諦める
		if (m_moveCnt >= FPS_N && abs(m_prevX - x) < 20) {
			m_gx = x;
			m_gy = y;
		}
		m_prevX = x;
	}
	// 目標に到達しているか
	if (m_gx > x - GX_ERROR && m_gx < x + GX_ERROR && m_gy > y - GY_ERROR && m_gy < y + GY_ERROR) {
		return true;
	}
	// 到達していないので移動
	stickOrder(right, left, up, down);
	// 壁にぶつかったからジャンプ
	int maxJump = m_characterAction_p->getPreJumpMax();
	if (m_jumpCnt == 0) {
		if (m_rightKey > 0 && m_characterAction_p->getRightLock()) { m_jumpCnt = maxJump; }
		else if (m_leftKey > 0 && m_characterAction_p->getLeftLock()) { m_jumpCnt = maxJump; }
	}
	if (m_jumpCnt > 0) { m_jumpCnt--; }
	jump = m_jumpCnt == 0 ? 0 : maxJump - m_jumpCnt;
	return false;
}


/*
* キャラについていくNormalAI
*/
FollowNormalAI::FollowNormalAI() :
	NormalAI()
{
	m_follow_p = nullptr;
}

Brain* FollowNormalAI::createCopy(std::vector<Character*> characters, const Camera* camera) {
	FollowNormalAI* res = new FollowNormalAI();
	if (m_follow_p != nullptr) {
		for (unsigned int i = 0; i < characters.size(); i++) {
			if (m_follow_p->getId() == characters[i]->getId()) {
				res->setFollow(characters[i]);
				break;
			}
		}
	}
	if (m_target_p != nullptr) {
		for (unsigned int i = 0; i < characters.size(); i++) {
			if (m_target_p->getId() == characters[i]->getId()) {
				res->setTarget(characters[i]);
				break;
			}
		}
	}
	setParam(res);
	return res;
}

int FollowNormalAI::getFollowId() const { 
	return m_follow_p == nullptr ? -1 : m_follow_p->getId();
}

const char* FollowNormalAI::getFollowName() const { 
	return m_follow_p == nullptr ? "ハート" : m_follow_p->getName().c_str();
}

const Character* FollowNormalAI::getFollow() const {
	return m_follow_p;
}

// 追跡対象をセット
void FollowNormalAI::setFollow(const Character* character) {
	m_follow_p = character;
	if (m_characterAction_p != nullptr) {
		m_gx = m_characterAction_p->getCharacter()->getX();
		m_gy = m_characterAction_p->getCharacter()->getY();
	}
}

bool FollowNormalAI::checkAlreadyFollow() {
	// 追跡対象がいない
	if (m_follow_p == nullptr) { return true; }
	int followX = m_follow_p->getCenterX();
	return  m_gx < followX + FOLLOW_X_ERROR && m_gx > followX - FOLLOW_X_ERROR;
}

void FollowNormalAI::moveOrder(int& right, int& left, int& up, int& down) {

	// ハートがスキル発動中で動かないなら無視
	if (m_follow_p != nullptr && m_follow_p->getFreeze()) { 
		NormalAI::moveOrder(right, left, up, down);
		return;
	}

	// 現在地
	int x = m_characterAction_p->getCharacter()->getCenterX();
	int y = m_characterAction_p->getCharacter()->getCenterY();

		// (壁につっかえるなどで)移動できてないから諦める
	if (m_moveCnt >= GIVE_UP_MOVE_CNT) {
			m_gx = x;
			m_gy = y;
	}

	// 目標地点設定用パラメータ
	bool alreadyGoal = m_gx > x - GX_ERROR && m_gx < x + GX_ERROR;
	bool alreadyFollow = checkAlreadyFollow();

	// 目標地点設定
	if ((alreadyGoal && GetRand(MOVE_RAND) == 0) || !alreadyFollow) {
		if (m_follow_p != nullptr) {
			// followについていく
			m_gx = m_follow_p->getCenterX() + GetRand(FOLLOW_X_ERROR * 2) - FOLLOW_X_ERROR;
		}
		else {
			// ランダムに設定
			m_gx = GetRand(400) - 200;
			m_gx += x;
		}
		if (abs(x - m_gx) < 50) { m_gx = x; }
	}

	stickOrder(right, left, up, down);
}

// 追跡対象を決める(AIクラスでオーバライドする。)
void FollowNormalAI::searchFollow(const Character* character) {
	// 味方のみ
	if (character->getId() != m_characterAction_p->getCharacter()->getId() && character->getGroupId() == m_characterAction_p->getCharacter()->getGroupId()) {
		m_follow_p = character;
	}
}

// 追跡対象を変更する必要があるならtrueでアピールする(AIクラスでオーバライドする)。
bool FollowNormalAI::needSearchFollow() const {
	if (m_follow_p == nullptr || m_follow_p->getHp() == 0) {
		return true;
	}
	return false;
}


/*
* 斜方投射するNormalAI
*/
ParabolaAI::ParabolaAI() :
	NormalAI()
{

}

Brain* ParabolaAI::createCopy(std::vector<Character*> characters, const Camera* camera) {
	ParabolaAI* res = new ParabolaAI();
	copyTarget(characters, getTargetId(), res);
	setParam(res);
	return res;
}

// 斜方投射の計算をする
void setParabolaBulletTarget(int& x, int& y, const CharacterAction* characterAction_p, const Character* target_p) {
	if (target_p == nullptr) {
		x = 0;
		y = 0;
	}
	else { // ターゲットに向かって射撃攻撃
		const int G = -ParabolaBullet::G;
		int dx = target_p->getCenterX() - characterAction_p->getCharacter()->getCenterX();
		int gx = abs(dx);
		int gy = -(target_p->getCenterY() - characterAction_p->getCharacter()->getCenterY());
		int v = characterAction_p->getCharacter()->getAttackInfo()->bulletSpeed();
		double A = (G * gx * gx) / (2 * v * v);
		double a = gx / A;
		double b = 1 - (gy / A);
		double routeInside = a * a / 4 - b;
		if (routeInside >= 0) {
			double route = sqrt(routeInside);
			if (GetRand(99) < 50) { route *= -1; }
			double r = atan(route - (a / 2));
			if (dx > 0) {
				x = (int)(characterAction_p->getCharacter()->getCenterX() + v * cos(r));
			}
			else {
				x = (int)(characterAction_p->getCharacter()->getCenterX() - v * cos(r));
			}
			y = (int)(characterAction_p->getCharacter()->getCenterY() - v * sin(r));
		}
		else {
			// 射程外なら45度で投げる
			double r = 3.14 / 4;
			if (dx > 0) {
				x = (int)(characterAction_p->getCharacter()->getCenterX() + v * cos(r));
			}
			else {
				x = (int)(characterAction_p->getCharacter()->getCenterX() - v * cos(r));
			}
			y = (int)(characterAction_p->getCharacter()->getCenterY() - v * sin(r));
		}
	}
}

void ParabolaAI::bulletTargetPoint(int& x, int& y) {
	setParabolaBulletTarget(x, y, m_characterAction_p, m_target_p);
}


/*
* 斜方投射するFollowNormalAI
*/
FollowParabolaAI::FollowParabolaAI() :
	FollowNormalAI()
{

}

Brain* FollowParabolaAI::createCopy(std::vector<Character*> characters, const Camera* camera) {
	FollowParabolaAI* res = new FollowParabolaAI();
	copyTarget(characters, getTargetId(), res);
	copyFollow(characters, getFollowId(), res);
	setParam(res);
	return res;
}
void FollowParabolaAI::bulletTargetPoint(int& x, int& y) {
	setParabolaBulletTarget(x, y, m_characterAction_p, m_target_p);
}


/*
* ヴァルキリア用AI
*/
ValkiriaAI::ValkiriaAI() :
	FollowNormalAI()
{

}

Brain* ValkiriaAI::createCopy(std::vector<Character*> characters, const Camera* camera) {
	ValkiriaAI* res = new ValkiriaAI();
	copyTarget(characters, getTargetId(), res);
	copyFollow(characters, getFollowId(), res);
	setParam(res);
	return res;
}

int ValkiriaAI::slashOrder() {
	if (m_target_p == nullptr || m_target_p->getHp() == 0) {
		return 0;
	}
	int x = m_characterAction_p->getCharacter()->getCenterX();
	int y = m_characterAction_p->getCharacter()->getY() + m_characterAction_p->getCharacter()->getHeight();
	// 距離の近い敵が高くにいるなら
	if ((abs(x - m_target_p->getCenterX()) < getSlashReach()) && (y - m_target_p->getCenterY() > 200)) {
		// 地面にいるうちは斬撃しない
		if (m_characterAction_p->getGrand()) {
			return 0;
		}
	}
	// 遠距離の敵には斬撃しない
	if (abs(m_target_p->getCenterX() - x) >= getSlashReach()) {
		return 0;
	}
	// ランダムで斬撃
	if (GetRand(50) == 0) {
		return 1;
	}
	return 0;
}

void ValkiriaAI::moveOrder(int& right, int& left, int& up, int& down) {

	// ハートがスキル発動中で動かないなら無視
	if (m_follow_p != nullptr && m_follow_p->getFreeze()) {
		NormalAI::moveOrder(right, left, up, down);
		return;
	}

	if (m_characterAction_p->getSlashCnt() > 0) {
		// 攻撃中は移動しない
		right = 0; left = 0; up = 0; down = 0;
		return;
	}
	int x = m_characterAction_p->getCharacter()->getCenterX();
	if (m_follow_p != nullptr && m_target_p != nullptr && m_target_p->getHp() > 0) {
		// 戦闘中の敵が近くにいるならハートとの距離をある程度気にせずtargetを追跡
		if (abs(m_follow_p->getCenterX() - x) < FOLLOW_X_ERROR * 2 && abs(m_target_p->getCenterX() - x) < getSlashReach()) {
			NormalAI::moveOrder(right, left, up, down);
			return;
		}
	}
	FollowNormalAI::moveOrder(right, left, up, down);
}

int ValkiriaAI::jumpOrder() {
	int maxJump = m_characterAction_p->getPreJumpMax();
	int minJump = maxJump / 3;
	int x = m_characterAction_p->getCharacter()->getCenterX();
	int y = m_characterAction_p->getCharacter()->getY() + m_characterAction_p->getCharacter()->getHeight();
	if (m_jumpCnt == 0) {
		// ランダムでジャンプ
		if (m_squatCnt == 0 && GetRand(99) < 20 && m_target_p != nullptr && m_target_p->getHp() > 0) {
			// 距離の近い敵が高くにいるなら
			if ((abs(x - m_target_p->getCenterX()) < getSlashReach()) && (y - m_target_p->getCenterY() > 200)) {
				m_jumpCnt = GetRand(maxJump - minJump) + minJump;
			}
		}
	}
	return NormalAI::jumpOrder();
}

// 攻撃対象を変更する必要があるならtrueでアピールする。
bool ValkiriaAI::needSearchTarget() const {
	// 今のターゲットは距離が遠いから
	if (m_target_p != nullptr && m_target_p->getHp() > 0) {
		if (abs(m_target_p->getCenterX() - m_characterAction_p->getCharacter()->getCenterX()) > getSlashReach() * 2) {
			return true;
		}
	}
	return NormalAI::needSearchTarget();
}


/*
* 空を飛ぶAI
*/
FlightAI::FlightAI() :
	NormalAI()
{

}

Brain* FlightAI::createCopy(std::vector<Character*> characters, const Camera* camera) {
	FlightAI* res = new FlightAI();
	copyTarget(characters, getTargetId(), res);
	setParam(res);
	return res;
}

void FlightAI::moveOrder(int& right, int& left, int& up, int& down) {
	// 現在地
	int x = m_characterAction_p->getCharacter()->getCenterX();
	int y = m_characterAction_p->getCharacter()->getCenterY();

	// 上下移動の制御
	moveUpDownOrder(x, y, m_try);

	// 目標地点設定
	bool alreadyGoal = m_gx > x - GX_ERROR && m_gx < x + GX_ERROR;
	// ダメージを受けたらリセット
	if (m_characterAction_p->getState() == CHARACTER_STATE::DAMAGE) {
		m_gx = x, m_gy = y;
		m_try = false;
	}
	else if (alreadyGoal && GetRand(MOVE_RAND) == 0) {
		if (m_target_p != nullptr && abs(x - m_target_p->getCenterX()) < TARGET_DISTANCE) {
			setGoalToTarget();
		}
		else {
			// ランダムに設定
			m_gx = x + (GetRand(400) - 200);
			m_gy = y + (GetRand(200) - 100);
		}
		if (abs(x - m_gx) < 50) { m_gx = x; }
		m_try = false;
	}
	stickOrder(right, left, up, down);
}

// 目標地点へ移動するだけ 達成済みならtrueで何もしない
bool FlightAI::moveGoalOrder(int& right, int& left, int& up, int& down, int& jump) {
	// 現在地
	int x = m_characterAction_p->getCharacter()->getCenterX();
	int y = m_characterAction_p->getCharacter()->getY() + m_characterAction_p->getCharacter()->getHeight();
	moveUpDownOrder(x, y, m_try);
	bool flag = NormalAI::moveGoalOrder(right, left, up, down, jump);
	return flag;
}

void FlightAI::setGoalToTarget() {
	// targetについていく
	int NEAR_TARGET_X = 2000, NEAR_TARGET_Y = 600;
	if (!m_characterAction_p->getCharacter()->haveBulletAttack()) {
		NEAR_TARGET_X = 500; // 近距離攻撃しかないからより近づく
		NEAR_TARGET_Y = 300;
	}
	int targetX1 = 0, targetY1 = 0, targetX2 = 0, targetY2 = 0;
	m_target_p->getAtariArea(&targetX1, &targetY1, &targetX2, &targetY2);
	m_gx = (targetX1 + targetX2) / 2 + GetRand(NEAR_TARGET_X) - NEAR_TARGET_X / 2;
	m_gy = targetY1 + GetRand(NEAR_TARGET_Y) - (NEAR_TARGET_Y - 200);
}


/*
* 空を飛ぶAI
*/
FollowFlightAI::FollowFlightAI() :
	FollowNormalAI()
{

}

Brain* FollowFlightAI::createCopy(std::vector<Character*> characters, const Camera* camera) {
	FollowFlightAI* res = new FollowFlightAI();
	copyTarget(characters, getTargetId(), res);
	copyFollow(characters, getFollowId(), res);
	setParam(res);
	return res;
}

void FollowFlightAI::moveOrder(int& right, int& left, int& up, int& down) {

	// ハートがスキル発動中で動かないなら無視
	if (m_follow_p != nullptr && m_follow_p->getFreeze()) {
		NormalAI::moveOrder(right, left, up, down);
		return;
	}

	// 現在地
	int x = m_characterAction_p->getCharacter()->getCenterX();
	int y = m_characterAction_p->getCharacter()->getCenterY();

	// 上下移動の制御
	moveUpDownOrder(x, y, m_try);

	// 目標地点設定
	bool alreadyGoal = m_gx > x - GX_ERROR && m_gx < x + GX_ERROR;
	bool alreadyFollow = checkAlreadyFollow();

	// ダメージを受けたらリセット
	if (m_characterAction_p->getState() == CHARACTER_STATE::DAMAGE) {
		m_gx = x, m_gy = y;
		m_try = false;
	}
	else if ((alreadyGoal && GetRand(MOVE_RAND) == 0) || !alreadyFollow) {
		if (m_follow_p != nullptr) {
			// followについていく
			m_gx = m_follow_p->getCenterX() + GetRand(FOLLOW_X_ERROR * 2) - FOLLOW_X_ERROR;
			m_gy = m_follow_p->getCenterY() + GetRand(800) - 700;
		}
		else {
			// ランダムに設定
			m_gx = x + (GetRand(400) - 200);
			m_gy = y + (GetRand(200) - 100);
		}
		if (abs(x - m_gx) < 50) { m_gx = x; }
		m_try = false;
	}
	stickOrder(right, left, up, down);
}

// 目標地点へ移動するだけ 達成済みならtrueで何もしない
bool FollowFlightAI::moveGoalOrder(int& right, int& left, int& up, int& down, int& jump) {
	// 現在地
	int x = m_characterAction_p->getCharacter()->getCenterX();
	int y = m_characterAction_p->getCharacter()->getY() + m_characterAction_p->getCharacter()->getHeight();
	moveUpDownOrder(x, y, m_try);
	bool flag = NormalAI::moveGoalOrder(right, left, up, down, jump);
	return flag;
}


/*
* ヒエラルキー用AI
*/
HierarchyAI::HierarchyAI() :
	NormalAI()
{

}
Brain* HierarchyAI::createCopy(std::vector<Character*> characters, const Camera* camera) {
	HierarchyAI* res = new HierarchyAI();
	copyTarget(characters, getTargetId(), res);
	setParam(res);
	return res;
}
int HierarchyAI::bulletOrder() {
	return m_characterAction_p->getBulletCnt() + 1;
}
void HierarchyAI::bulletTargetPoint(int& x, int& y) {
	x = GetRand(600) - 300 + m_characterAction_p->getCharacter()->getCenterX();
	y = m_characterAction_p->getCharacter()->getCenterY() - GetRand(300);
}


/*
* フレンチ用AI
*/
FrenchAI::FrenchAI() :
	NormalAI()
{

}
Brain* FrenchAI::createCopy(std::vector<Character*> characters, const Camera* camera) {
	FrenchAI* res = new FrenchAI();
	copyTarget(characters, getTargetId(), res);
	setParam(res);
	return res;
}
int FrenchAI::slashOrder() {
	if (m_target_p == nullptr || m_target_p->getHp() == 0) {
		return 0;
	}
	int x = m_characterAction_p->getCharacter()->getCenterX();
	int y = m_characterAction_p->getCharacter()->getY() + m_characterAction_p->getCharacter()->getHeight();
	// 距離の近い敵が高くにいるなら
	if ((abs(x - m_target_p->getCenterX()) < getSlashReach()) && (y - m_target_p->getCenterY() > 200)) {
		// 地面にいるうちは斬撃しない
		if (m_characterAction_p->getGrand()) {
			return 0;
		}
	}
	// 遠距離の敵には斬撃しない
	if (abs(m_target_p->getCenterX() - x) >= getSlashReach()) {
		return 0;
	}
	// ランダムで斬撃
	if (GetRand(30) == 0) {
		return 1;
	}
	return 0;
}
void FrenchAI::moveOrder(int& right, int& left, int& up, int& down) {

	if (m_characterAction_p->getSlashCnt() > 0) {
		// 攻撃中は移動しない
		right = 0; left = 0; up = 0; down = 0;
		m_gx = m_characterAction_p->getCharacter()->getCenterX();
		m_gy = m_characterAction_p->getCharacter()->getY();
		return;
	}
	NormalAI::moveOrder(right, left, up, down);
}


/*
*  カテゴリーZ用AI
*/
CategoryZAI::CategoryZAI() :
	FrenchAI()
{

}

Brain* CategoryZAI::createCopy(std::vector<Character*> characters, const Camera* camera) {
	CategoryZAI* res = new CategoryZAI();
	copyTarget(characters, getTargetId(), res);
	setParam(res);
	return res;
}


/*
* Boss1: サン
*/
SunAI::SunAI() :
	FlightAI()
{

}

Brain* SunAI::createCopy(std::vector<Character*> characters, const Camera* camera) {
	SunAI* res = new SunAI();
	copyTarget(characters, getTargetId(), res);
	setParam(res);
	return res;
}

// 移動（上下左右の入力）
void SunAI::moveOrder(int& right, int& left, int& up, int& down) {
	// 現在地
	int x = m_characterAction_p->getCharacter()->getCenterX();
	int y = m_characterAction_p->getCharacter()->getCenterY();

	if ((x > m_gx && m_characterAction_p->getLeftLock()) || (x < m_gx && m_characterAction_p->getRightLock())) {
		m_gx = x, m_gy = y;
		m_try = false;
	}

	if (m_characterAction_p->getState() != CHARACTER_STATE::INIT) {
		FlightAI::moveOrder(right, left, up, down);
	}
}

// ジャンプの制御
int SunAI::jumpOrder() {
	if (m_characterAction_p->getState() != CHARACTER_STATE::INIT) {
		return FlightAI::jumpOrder();
	}
	return 0;
}

// しゃがみの制御
int SunAI::squatOrder() {
	if (m_characterAction_p->getState() != CHARACTER_STATE::INIT) {
		return FlightAI::squatOrder();
	}
	return 0;
}

// 近距離攻撃
int SunAI::slashOrder() {
	if (m_characterAction_p->getState() != CHARACTER_STATE::INIT) {
		return FlightAI::slashOrder();
	}
	return 0;
}

// 遠距離攻撃
int SunAI::bulletOrder() {
	if (m_characterAction_p->getState() != CHARACTER_STATE::INIT) {
		return m_characterAction_p->getBulletCnt() + 1;
	}
	return 0;
}

void SunAI::bulletTargetPoint(int& x, int& y) {
	x = GetRand(m_characterAction_p->getCharacter()->getWide()) + m_characterAction_p->getCharacter()->getX();
	y = m_characterAction_p->getCharacter()->getY() - GetRand(300);
}

void SunAI::setGoalToTarget() {
	// targetについていく
	int NEAR_TARGET_X = 1500, NEAR_TARGET_Y = 400;
	m_gx = m_characterAction_p->getCharacter()->getCenterX() + GetRand(NEAR_TARGET_X) - NEAR_TARGET_X / 2;
	m_gy = m_target_p->getCenterY() + GetRand(NEAR_TARGET_Y) - (NEAR_TARGET_Y / 2);
	if (m_characterAction_p->getLeftLock()) {
		m_gx += NEAR_TARGET_X / 2;
	}
	else if (m_characterAction_p->getRightLock()) {
		m_gx -= NEAR_TARGET_X / 2;
	}
}