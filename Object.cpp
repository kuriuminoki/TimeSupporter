#include "Object.h"
#include "Character.h"
#include "CharacterAction.h"
#include "CharacterController.h"
#include "Define.h"
#include "GraphHandle.h"
#include "Animation.h"
#include "Item.h"
#include "DxLib.h"
#include <algorithm>
#include <cmath>
#include <sstream>

using namespace std;


const double OBJECT_DEFAULT_SIZE = 0.3;


int Object::objectId;

Object::Object() :
	Object(0, 0, 0, 0, -1)
{
	
}

Object::Object(int x1, int y1, int x2, int y2, int hp) {
	objectId++;
	m_id = objectId;

	m_x1 = x1;
	m_y1 = y1;
	m_x2 = x2;
	m_y2 = y2;
	// 大小関係は 1 <= 2
	if (m_x1 > m_x2) { std::swap(m_x1, m_x2); }
	if (m_y1 > m_y2) { std::swap(m_y1, m_y2); }

	m_handle = nullptr;

	m_hp = hp;
	m_bomb = false;
	m_damageCnt = 0;

	m_deleteFlag = false;

	m_effectHandles_p = nullptr;
	m_soundHandle_p = -1;
}

void Object::setParam(Object* object) {
	object->setId(m_id);
	object->setAtariIdList(m_atariIdList);
	object->setX1(m_x1);
	object->setY1(m_y1);
	object->setX2(m_x2);
	object->setY2(m_y2);
	object->setHp(m_hp);
	object->setDamageCnt(m_damageCnt);
	object->setDeleteFlag(m_deleteFlag);
	object->setEffectHandles(m_effectHandles_p);
	object->setSoundHandle(m_soundHandle_p);
}

// HPを減らす
bool Object::decreaseHp(int damageValue, int id) {
	if (!getAbleDelete()) { return false; }
	for (unsigned int i = 0; i < m_atariIdList.size(); i++) {
		if (m_atariIdList[i] == id) { 
			return false;
		}
	}
	m_atariIdList.push_back(id);
	m_hp = max(0, m_hp - damageValue);
	if (m_hp == 0) { 
		setDeleteFlag(true);
	}
	m_damageCnt = DAMAGE_CNT_SUM;
	return true;
}

// 単純に四角の落下物と衝突しているか
bool Object::atariDropBox(int x1, int y1, int x2, int y2, int& vx, int& vy) {
	// 上から衝突してきた (座標がint型で+-1の誤差があるためそれは許容する)
	if (x2 + vx >= m_x1 && x1 + vx <= m_x2 && y2 - 1 <= m_y1 && y2 + 1 + vy >= m_y1) {
		vx = 0; vy = 0;
		return true;
	}
	// 左右からの衝突
	if (x2 <= m_x1 && x2 + vx >= m_x1 && y2 > m_y1 && y1 < m_y2) {
		vx = 0;
		return false;
	}
	if (x1 >= m_x2 && x1 + vx <= m_x2 && y2 > m_y1 && y1 < m_y2) {
		vx = 0;
		return false;
	}
	// 下からの衝突
	if (y1 > y2 && y1 + vy <= y2 && x2 > m_x1 && x1 < m_x2) {
		vy = 1;
		return false;
	}
	// 埋まっている
	if (x2 > m_x1 && x1 < m_x2 && y2 > m_y1 && y1 < m_y2) {
		if ((y1 + y2) < (m_y1 + m_y2)) { vy = -1; }
		else { vy = 1; }
		if ((x1 + x2) < (m_x1 + m_x2)) { vx = -1; }
		else { vx = 1; }
		return false;
	}
	return false;
}

// アニメーション作成
Animation* BulletObject::createAnimation(int x, int y, int flameCnt) {
	if (m_effectHandles_p == nullptr) {
		return nullptr;
	}
	return new Animation((m_x1 + m_x2) / 2, (m_y1 + m_y2) / 2, 3, m_effectHandles_p);
}

// アニメーション作成
Animation* SlashObject::createAnimation(int x, int y, int flameCnt) {
	if (m_effectHandles_p == nullptr) {
		return nullptr;
	}
	// 座標の平均をとる
	x = (x + (m_x1 + m_x2) / 2) / 2;
	y = (y + (m_y1 + m_y2) / 2) / 2;
	return new Animation(x, y, flameCnt, m_effectHandles_p);
}

/*
* 四角形のオブジェクト
*/
BoxObject::BoxObject(int x1, int y1, int x2, int y2, const char* fileName, int color, int hp) :
	Object(x1, y1, x2, y2, hp)
{
	m_fileName = fileName;
	if (m_fileName != "null") {
		string filePath = "picture/stageMaterial/";
		filePath += m_fileName;
		m_handle = new GraphHandle(filePath.c_str(), OBJECT_DEFAULT_SIZE, 0.0, true);
	}
	m_color = color;
}

BoxObject::~BoxObject() {
	if (m_handle != nullptr) {
		delete m_handle;
	}
}

// キャラクターとの当たり判定
bool BoxObject::atari(CharacterController* characterController) {
	// キャラの情報　座標と移動スピード
	int characterX1 = characterController->getAction()->getCharacter()->getX();
	int characterY1 = characterController->getAction()->getCharacter()->getY();
	int characterWide = characterController->getAction()->getCharacter()->getWide();
	int characterHeight = characterController->getAction()->getCharacter()->getHeight();
	int characterX2 = characterX1 + characterWide;
	int characterY2 = characterY1 + characterHeight;
	int characterVx = characterController->getAction()->getVx();
	int characterVy = characterController->getAction()->getVy();
	characterController->getAction()->getCharacter()->getAtariArea(&characterX1, &characterY1, &characterX2, &characterY2);
	characterWide = characterX2 - characterX1;
	characterHeight = characterY2 - characterY1;

	// キャラが上下移動で当たっているか判定
	if (characterX2 > m_x1 && characterX1 < m_x2) {
		// 下に移動中のキャラが上から当たっているか判定
		if (characterY2 <= m_y1 && characterY2 + characterVy >= m_y1) {
			// 着地
			characterController->setCharacterGrand(true);
			// キャラは下へ移動できない
			characterController->setActionDownLock(true);
			// 密着状態までは移動させる
			int height = characterY2 - characterController->getAction()->getCharacter()->getY();
			characterController->setCharacterY(m_y1 - height);
		}
		// 上に移動中のキャラが下から当たっているか判定
		else if (characterY1 >= m_y2 && characterY1 + characterVy <= m_y2) {
			// キャラは上へ移動できない
			characterController->setActionUpLock(true);
			// 密着状態までは移動させる
			int topD = characterY1 - characterController->getAction()->getCharacter()->getY();
			characterController->setCharacterY(m_y2 - topD);
		}
	}

	// キャラが左右移動で当たっているか判定
	if (characterY2 + characterVy > m_y1 && characterY1 + characterVy < m_y2) {
		bool slope = characterController->getAction()->getGrandLeftSlope() || characterController->getAction()->getGrandRightSlope();
		// 右に移動中のキャラが左から当たっているか判定
		if (characterX2 <= m_x1 && characterX2 + characterVx >= m_x1) {
			// 段差とみなして乗り越える
			if (slope && characterY2 - STAIR_HEIGHT <= m_y1) {

			}
			else {
				// キャラは右へ移動できない
				characterController->setActionRightLock(true);
				// 密着状態までは移動させる
				int wide = characterX2 - characterController->getAction()->getCharacter()->getX();
				characterController->setCharacterX(m_x1 - wide);
			}
		}
		// 左に移動中のキャラが右から当たっているか判定
		else if (characterX1 >= m_x2 && characterX1 + characterVx <= m_x2) {
			if (slope && characterY2 - STAIR_HEIGHT <= m_y1) {

			}
			else {
				// キャラは左へ移動できない
				characterController->setActionLeftLock(true);
				// 密着状態までは移動させる
				int leftD = characterX1 - characterController->getAction()->getCharacter()->getX();
				characterController->setCharacterX(m_x2 - leftD);
			}
		}
	}
	return false;
}

// キャラがオブジェクトに入り込んでいるときの処理
void BoxObject::penetration(CharacterController* characterController) {
	// キャラの情報　座標と移動スピード
	int characterX1 = characterController->getAction()->getCharacter()->getX();
	int characterY1 = characterController->getAction()->getCharacter()->getY();
	int characterWide = characterController->getAction()->getCharacter()->getWide();
	int characterHeight = characterController->getAction()->getCharacter()->getHeight();
	int characterX2 = characterX1 + characterWide;
	int characterY2 = characterY1 + characterHeight;
	characterController->getAction()->getCharacter()->getAtariArea(&characterX1, &characterY1, &characterX2, &characterY2);
	characterWide = characterX2 - characterX1;
	characterHeight = characterY2 - characterY1;
	// 万が一オブジェクトの中に入り込んでしまったら
	bool slope = characterController->getAction()->getGrandLeftSlope() || characterController->getAction()->getGrandRightSlope();
	if (!slope && characterY2 > m_y1 && characterY1 < m_y2 && characterX2 > m_x1 && characterX1 < m_x2) {
		int overlapX = min(characterX2 - m_x1, m_x2 - characterX1);
		int overlapY = min(characterY2 - m_y1, m_y2 - characterY1);
		// キャラが横にはみ出しているなら
		if (overlapX < overlapY) {
			if ((characterX1 + characterX2) < (m_x1 + m_x2)) {
				// 密着状態まで移動させる
				int wide = characterX2 - characterController->getAction()->getCharacter()->getX();
				characterController->setCharacterX(m_x1 - wide);
				// キャラは右へ移動できない
				characterController->setActionRightLock(true);
			}
			else {
				// 密着状態まで移動させる
				int leftD = characterX1 - characterController->getAction()->getCharacter()->getX();
				characterController->setCharacterX(m_x2 - leftD);
				// キャラは左へ移動できない
				characterController->setActionLeftLock(true);
			}
		}
		else {
			if ((characterY1 + characterY2) < (m_y1 + m_y2)) {
				// 真上へ
				int height = characterY2 - characterController->getAction()->getCharacter()->getY();
				characterController->setCharacterY(m_y1 - height);
				// 着地
				characterController->setCharacterGrand(true);
				// キャラは下へ移動できない
				characterController->setActionDownLock(true);
			}
			else {
				// 真下へ
				int topD = characterY1 - characterController->getAction()->getCharacter()->getY();
				characterController->setCharacterY(m_y2 - topD);
				// キャラは上へ移動できない
				characterController->setActionUpLock(true);
			}
		}
	}
}

// 攻撃オブジェクトとの当たり判定
bool BoxObject::atariFromObject(Object* object) {
	// 当たっているなら
	if (m_x2 > object->getX1() && m_x1 < object->getX2() && m_y2 > object->getY1() && m_y1 < object->getY2()) {
		if (object->getAbleDelete()) {
			object->setDeleteFlag(true);
		}
		// 自分の体力を減らす
		return decreaseHp(object->getDamage(), object->getId());
	}
	return false;
}

void BoxObject::action() {
	if (m_damageCnt > 0) { m_damageCnt--; }
}

TriangleObject::TriangleObject(int x1, int y1, int x2, int y2, const char* fileName, int color, bool leftDown, int hp):
	Object(x1, y1, x2, y2, hp)
{
	m_color = color;
	m_fileName = fileName;
	if (m_fileName != "null") {
		string filePath = "picture/stageMaterial/";
		filePath += m_fileName;
		m_handle = new GraphHandle(filePath.c_str(), OBJECT_DEFAULT_SIZE, 0.0, true, !leftDown);
	}
	m_leftDown = leftDown;
}

TriangleObject::~TriangleObject() {
	if (m_handle != nullptr) {
		delete m_handle;
	}
}

// 座標XにおけるY座標（傾きから算出する）
int TriangleObject::getY(int x) const {
	if (x < m_x1) {
		if (m_leftDown) {
			return m_y2;
		}
		else {
			return m_y1;
		}
	}
	if (x > m_x2) {
		if (m_leftDown) {
			return m_y1;
		}
		else {
			return m_y2;
		}
	}
	// 傾き
	// 分子
	double a1 = (double)(m_y1 - m_y2);
	if (!m_leftDown) {
		a1 *= -1;
	}
	// 分母
	double a2 = (double)(m_x2 - m_x1);

	// 切片
	double b;
	if (m_leftDown) {
		b = m_y1 - (a1 * m_x2 / a2);
	}
	else {
		b = m_y1 - (a1 * m_x1 / a2);
	}

	// 座標
	double y = a1 * x / a2 + b;

	return (int)y;
}

// キャラクターとの当たり判定
bool TriangleObject::atari(CharacterController* characterController) {
	// キャラの情報　座標と移動スピード
	int characterX1 = characterController->getAction()->getCharacter()->getX();
	int characterY1 = characterController->getAction()->getCharacter()->getY();
	int characterWide = characterController->getAction()->getCharacter()->getWide();
	int characterHeight = characterController->getAction()->getCharacter()->getHeight();
	int characterX2 = characterX1 + characterWide;
	int characterY2 = characterY1 + characterHeight;
	int characterX1_5 = characterController->getAction()->getCharacter()->getCenterX();
	int characterY1_5 = characterController->getAction()->getCharacter()->getCenterY();
	int characterVx = characterController->getAction()->getVx();
	int characterVy = characterController->getAction()->getVy();
	characterController->getAction()->getCharacter()->getAtariArea(&characterX1, &characterY1, &characterX2, &characterY2);
	characterWide = characterX2 - characterX1;
	characterHeight = characterY2 - characterY1;
	characterX1_5 = (characterX1 + characterX2) / 2;
	characterY1_5 = (characterY1 + characterY2) / 2;

	// キャラが上下移動で当たっているか判定
	if (characterX2 > m_x1 && characterX1 < m_x2) {
		// 下りているときはこの条件式がtrueになる 誤差+-1は許容
		if (characterY2 - 1 <= getY(characterX1_5 - characterVx) && characterY2 + 1 >= getY(characterX1_5 - characterVx)) {
			// 前のフレームでは着地していたので、引き続き着地
			characterController->setCharacterGrand(true);
			if (characterX1_5 > m_x1 && characterX1_5 < m_x2) {
				if (m_leftDown) {
					characterController->setCharacterGrandRightSlope(true);
				}
				else {
					characterController->setCharacterGrandLeftSlope(true);
				}
			}
			// キャラは下へ移動できない
			characterController->setActionDownLock(true);
			// 密着状態までは移動させる
			int height = characterY2 - characterController->getAction()->getCharacter()->getY();
			characterController->setCharacterY(getY(characterX1_5) - height);
		}
		// 下に移動中のキャラが上から当たっているか判定
		else if (characterY2 <= getY(characterX1_5) && characterY2 + characterVy >= getY(characterX1_5)) {
			// 着地
			characterController->setCharacterGrand(true);
			if (characterX1_5 > m_x1 && characterX1_5 < m_x2) {
				if (m_leftDown) {
					characterController->setCharacterGrandRightSlope(true);
				}
				else {
					characterController->setCharacterGrandLeftSlope(true);
				}
			}
			// キャラは下へ移動できない
			characterController->setActionDownLock(true);
			// 密着状態までは移動させる
			int height = characterY2 - characterController->getAction()->getCharacter()->getY();
			characterController->setCharacterY(getY(characterX1_5) - height);
		}
		// 上に移動中のキャラが下から当たっているか判定
		else if (characterY1 >= m_y2 && characterY1 + characterVy <= m_y2) {
			// キャラは上へ移動できない
			characterController->setActionUpLock(true);
			// 密着状態までは移動させる
			int topD = characterY1 - characterController->getAction()->getCharacter()->getY();
			characterController->setCharacterY(m_y2 - topD);
		}
	}

	// 坂を移動
	if (characterX2 > m_x1 && characterX1 < m_x2 && characterY2 <= m_y2 && characterY2 >= getY(characterX1_5)) {
		// 着地
		characterController->setCharacterGrand(true);
		if (characterX1_5 > m_x1 && characterX1_5 <= m_x2) {
			if (m_leftDown) {
				characterController->setCharacterGrandRightSlope(true);
			}
			else {
				characterController->setCharacterGrandLeftSlope(true);
			}
		}
		// キャラは下へ移動できない
		characterController->setActionDownLock(true);
		// 適切な高さへ移動
		int height = characterY2 - characterController->getAction()->getCharacter()->getY();
		characterController->setCharacterY(getY(characterX1_5) - height);
	}

	// 坂の鋭角（先端）の当たり判定
	if (m_y2 > characterY1 && m_y2 < characterY2) {
		if (m_leftDown) {// 左肩下がり
			// 右に移動中のキャラが右から当たっているか判定
			if (characterX2 <= m_x1 && characterX2 + characterVx >= m_x1) {
				// キャラは右へ移動できない
				characterController->setActionRightLock(true);
				// 密着状態までは移動させる
				int wide = characterX2 - characterController->getAction()->getCharacter()->getX();
				characterController->setCharacterX(m_x1 - wide);
			}
		}
		else {
			// 左に移動中のキャラが左から当たっているか判定
			if (characterX1 >= m_x2 && characterX1 + characterVx <= m_x2) {
				// キャラは左へ移動できない
				characterController->setActionLeftLock(true);
				// 密着状態までは移動させる
				int leftD = characterX1 - characterController->getAction()->getCharacter()->getX();
				characterController->setCharacterX(m_x2 - leftD);
			}
		}
	}

	// キャラが左右移動で当たっているか判定
	if (characterY2 > m_y1 && characterY1 < m_y2) {
		if (m_leftDown) {// 左肩下がり
			// 左に移動中のキャラが右から当たっているか判定
			if (characterX1 >= m_x2 && characterX1 + characterVx <= m_x2) {
				// キャラは左へ移動できない
				characterController->setActionLeftLock(true);
				// 密着状態までは移動させる
				int leftD = characterX1 - characterController->getAction()->getCharacter()->getX();
				characterController->setCharacterX(m_x2 - leftD);
			}
		}
		else {
			// 右に移動中のキャラが左から当たっているか判定
			if (characterX2 <= m_x1 && characterX2 + characterVx >= m_x1) {
				// キャラは右へ移動できない
				characterController->setActionRightLock(true);
				// 密着状態までは移動させる
				int wide = characterX2 - characterController->getAction()->getCharacter()->getX();
				characterController->setCharacterX(m_x1 - wide);
			}
		}
	}
	return false;
}

// 単純に四角の落下物と衝突しているか
bool TriangleObject::atariDropBox(int x1, int y1, int x2, int y2, int& vx, int& vy) {
	int y = getY((x1 + x2) / 2);
	// 上から衝突してきた (座標がint型で+-1の誤差があるためそれは許容する)
	if (x2 + vx >= m_x1 && x1 + vx <= m_x2 && y2 - 1 <= y && y2 + 1 + vy >= y) {
		vx = 0; vy = 0;
	}
	// 左右からの衝突
	if (x2 <= m_x1 && x2 + vx >= m_x1 && y2 > y && y1 < m_y2) {
		vx = 0;
		return false;
	}
	if (x1 >= m_x2 && x1 + vx <= m_x2 && y2 > y && y1 < m_y2) {
		vx = 0;
		return false;
	}
	// 下からの衝突
	if (y1 > y2 && y1 + vy <= y2 && x2 > m_x1 && x1 < m_x2) {
		vy = 1;
		return false;
	}
	// 埋まっている
	if (x2 > m_x1 && x1 < m_x2 && y2 > y && y1 < m_y2) {
		if ((y1 + y2) < (m_y1 + m_y2)) { vy = -1; }
		else { vy = 1; }
		if ((x1 + x2) < (m_x1 + m_x2)) { vx = -1; }
		else { vx = 1; }
		return false;
	}
	return false;
}

// キャラがオブジェクトに入り込んでいるときの処理
void TriangleObject::penetration(CharacterController* characterController) {
	// キャラの情報　座標と移動スピード
	int characterX1 = characterController->getAction()->getCharacter()->getX();
	int characterY1 = characterController->getAction()->getCharacter()->getY();
	int characterWide = characterController->getAction()->getCharacter()->getWide();
	int characterHeight = characterController->getAction()->getCharacter()->getHeight();
	int characterX1_5 = characterController->getAction()->getCharacter()->getCenterX();
	int characterY1_5 = characterController->getAction()->getCharacter()->getCenterY();
	int characterX2 = characterX1 + characterWide;
	int characterY2 = characterY1 + characterHeight;
	characterController->getAction()->getCharacter()->getAtariArea(&characterX1, &characterY1, &characterX2, &characterY2);
	characterWide = characterX2 - characterX1;
	characterHeight = characterY2 - characterY1;
	characterX1_5 = (characterX1 + characterX2) / 2;
	characterY1_5 = (characterY1 + characterY2) / 2;
	int slopeY = getY(characterX1_5);
	// 万が一オブジェクトの中に入り込んでしまったら
	if (characterY2 > slopeY && characterY1 < m_y2 && characterX2 > m_x1 && characterX1 < m_x2) {
		int overlapX = min(characterX2 - m_x1, m_x2 - characterX1);
		int overlapY = min(characterY2 - slopeY, m_y2 - characterY1);
		// キャラが横にはみ出しているなら
		if (overlapX < overlapY) {
			if ((characterX1 + characterX2) < (m_x1 + m_x2)) {
				// 密着状態まで移動させる
				int wide = characterX2 - characterController->getAction()->getCharacter()->getX();
				characterController->setCharacterX(m_x1 - wide);
				// キャラは右へ移動できない
				characterController->setActionRightLock(true);
			}
			else {
				// 密着状態まで移動させる
				int leftD = characterX1 - characterController->getAction()->getCharacter()->getX();
				characterController->setCharacterX(m_x2 - leftD);
				// キャラは左へ移動できない
				characterController->setActionLeftLock(true);
			}
		}
		else {
			if ((characterY1 + characterY2) < (slopeY + m_y2)) {
				// 真上へ
				int height = characterY2 - characterController->getAction()->getCharacter()->getY();
				characterController->setCharacterY(slopeY - height);
				// 着地
				characterController->setCharacterGrand(true);
				// キャラは下へ移動できない
				characterController->setActionDownLock(true);
			}
			else {
				// 真下へ
				int topD = characterY1 - characterController->getAction()->getCharacter()->getY();
				characterController->setCharacterY(m_y2 - topD);
				// キャラは上へ移動できない
				characterController->setActionUpLock(true);
			}
		}
	}
}

// 他オブジェクトとの当たり判定
bool TriangleObject::atariFromObject(Object* object) {
	// 斜辺を考慮して当たり判定を計算
	int y = object->getY1();
	if (m_leftDown) {
		y = getY(object->getX2());
	}
	else {
		y = getY(object->getX1());
	}
	// 当たっているなら
	if (m_x2 > object->getX1() && m_x1 < object->getX2() && m_y2 > object->getY1() && y < object->getY2()) {
		if (object->getAbleDelete()) {
			object->setDeleteFlag(true);
		}
		// 自分の体力を減らす
		return decreaseHp(object->getDamage(), object->getId());
	}
	return false;
}

void TriangleObject::action() {
	if (m_damageCnt > 0) { m_damageCnt--; }
}


BulletObject::BulletObject(int x, int y, int color, int gx, int gy, int energyEraseTime, AttackInfo* attackInfo) :
	Object(x - attackInfo->bulletRx(), y - attackInfo->bulletRy(), x + attackInfo->bulletRx(), y + attackInfo->bulletRy())
{
	// 必要なら後からセッタで設定
	m_characterId = -1;
	m_groupId = -1;

	// 各パラメータの設定
	m_color = color;
	m_gx = gx;
	m_gy = gy;
	m_rx = attackInfo->bulletRx();
	m_ry = attackInfo->bulletRy();
	m_damage = attackInfo->bulletDamage();
	m_d = attackInfo->bulletDistance();
	m_hp = attackInfo->bulletHp();
	m_bomb = attackInfo->bulletBomb();

	// 角度を計算し、VXとVYを決定
	int dx = gx - x;
	int dy = gy - y;
	double r = std::atan2((double)dy, (double)dx);
	m_v = attackInfo->bulletSpeed();
	m_vx = (int)(m_v * std::cos(r));
	m_vy = (int)(m_v * std::sin(r));

	// エフェクトの画像
	m_effectHandles_p = attackInfo->bulletEffectHandle();

	// サウンド
	m_soundHandle_p = attackInfo->bulletSoundeHandle();

	m_handle = nullptr;

	m_energyCnt = 0;
	m_energyEraseTime = energyEraseTime;
}

BulletObject::BulletObject(int x, int y, int color, int gx, int gy, int energyEraseTime) :
	Object()
{
	m_characterId = -1;
	m_groupId = -1;
	m_color = color;
	m_gx = gx;
	m_gy = gy;
	m_rx = 0;
	m_ry = 0;
	m_damage = 0;
	m_d = 0;
	m_hp = 0;
	m_v = 0;
	m_vx = 0;
	m_vy = 0;
	m_effectHandles_p = nullptr;
	m_soundHandle_p = -1;
	m_handle = nullptr;
	m_energyCnt = 0;
	m_energyEraseTime = energyEraseTime;
}

BulletObject::BulletObject(int x, int y, GraphHandle* handle, int gx, int gy, int energyEraseTime, AttackInfo* attackInfo):
	BulletObject(x, y, WHITE, gx, gy, energyEraseTime, attackInfo)
{
	m_handle = handle;
}

// キャラとの当たり判定
// 当たっているならキャラを操作する。
bool BulletObject::atari(CharacterController* characterController) {
	// 自滅防止
	if (m_characterId == characterController->getAction()->getCharacter()->getId()) {
		return false;
	}
	// チームキル防止 && 中立キャラには当たらない
	int groupId = characterController->getAction()->getCharacter()->getGroupId();
	if (m_groupId == groupId || groupId == -1) {
		return false;
	}

	// キャラの情報　座標と移動スピード
	int characterX1 = characterController->getAction()->getCharacter()->getX();
	int characterY1 = characterController->getAction()->getCharacter()->getY();
	int characterX2 = characterX1 + characterController->getAction()->getCharacter()->getWide();
	int characterY2 = characterY1 + characterController->getAction()->getCharacter()->getHeight();
	characterController->getAction()->getCharacter()->getDamageArea(&characterX1, &characterY1, &characterX2, &characterY2);
	if (characterX1 == characterX2 && characterY1 == characterY2) { return false; }

	// 当たり判定
	if (characterX2 > m_x1 && characterX1 < m_x2 && characterY2 > m_y1 && characterY1 < m_y2 && characterController->getAction()->ableDamage()) {
		// 貫通弾じゃないなら消滅
		m_deleteFlag = true;
		characterController->damage(m_vx / 2, m_vy / 2, m_damage);
		return true;
	}
	return false;
}

// 他オブジェクトに対する当たり判定
bool BulletObject::atariToObject(Object* object) {
	// どちらかが破壊不能オブジェクト
	if (m_groupId == object->getGroupId()) { 
		return false;
	}
	// 当たっているなら
	if (m_x2 > object->getX1() && m_x1 < object->getX2() && m_y2 > object->getY1() && m_y1 < object->getY2()) {
		return object->decreaseHp(m_damage, m_id);
	}
	return false;
}

void BulletObject::action() {
	if (m_damageCnt > 0) { m_damageCnt--; }
	m_x1 += m_vx;
	m_x2 += m_vx;
	m_y1 += m_vy;
	m_y2 += m_vy;
	m_d -= m_v;
	// 飛距離がきたら消滅
	if (m_d <= 0) {
		m_deleteFlag = true;
	}
}

Item* BulletObject::createAttackEnergy() {
	m_energyCnt++;
	if (m_energyEraseTime > 0 && m_energyCnt % 15 == 0) {
		EnergyItem* energyItem = new EnergyItem("energy", (m_x1 + m_x2) / 2, (m_y1 + m_y2) / 2, m_damage, m_energyEraseTime);
		return energyItem;
	}
	return nullptr;
}

// 画像ハンドルを返す
GraphHandle* BulletObject::getHandle() const {
	if (m_handle == nullptr) { return nullptr; }
	if (m_vx < 0) { m_handle->setReverseX(true); }
	else { m_handle->setReverseX(false); }
	return m_handle;
}


ParabolaBullet::ParabolaBullet(int x, int y, int color, int gx, int gy, int energyEraseTime, AttackInfo* attackInfo):
	BulletObject(x, y, color, gx, gy, energyEraseTime, attackInfo)
{

}
ParabolaBullet::ParabolaBullet(int x, int y, GraphHandle* handle, int gx, int gy, int energyEraseTime, AttackInfo* attackInfo):
	BulletObject(x, y, handle, gx, gy, energyEraseTime, attackInfo)
{
	// 攻撃範囲に合わせて画像の拡大率を設定
	int attackSize = max(attackInfo->bulletRx(), attackInfo->bulletRy());
	int graphX = 0, graphY = 0;
	GetGraphSize(handle->getHandle(), &graphX, &graphY);
	int graphSize = min(graphX, graphY);
	m_handle->setEx((double)attackSize / (double)graphSize);
}
ParabolaBullet::ParabolaBullet(int x, int y, GraphHandle* handle, int gx, int gy, int energyEraseTime) :
	BulletObject(x, y, -1, gx, gy, energyEraseTime)
{
	m_handle = handle;
}

void ParabolaBullet::action() {
	if (m_damageCnt > 0) { m_damageCnt--; }
	m_x1 += m_vx;
	m_x2 += m_vx;
	m_vy += G;
	m_y1 += m_vy;
	m_y2 += m_vy;
}

// 画像ハンドルを返す
GraphHandle* ParabolaBullet::getHandle() const {
	if (m_handle == nullptr) { return nullptr; }
	double r = atan2((double)m_vy, (double)m_vx);
	if (m_vy == 0) { r = 0; }
	m_handle->setAngle(r);
	return m_handle;
}


SlashObject::SlashObject(int x1, int y1, int x2, int y2, GraphHandle* handle, int slashCountSum, int energyEraseTime, const AttackInfo* attackInfo) :
	Object(x1, y1, x2, y2, attackInfo->slashHp())
{
	// 必要なら後からセッタで設定
	m_characterId = -1;
	m_groupId = -1;

	// 画像
	m_handle = handle;
	
	// ダメージ
	m_damage = attackInfo->slashDamage();

	// 全体フレーム
	m_slashCountSum = slashCountSum;

	// カウント
	m_cnt = 0;

	// 吹っ飛び(X方向の初速)
	m_slashImpactX = attackInfo->slashImpactX();

	// 吹っ飛び(Y方向の初速)
	m_slashImpactY = attackInfo->slashImpactY();


	// エフェクトの画像
	m_effectHandles_p = attackInfo->slashEffectHandle();

	// サウンド
	m_soundHandle_p = attackInfo->slashSoundHandle();

	m_energyEraseTime = energyEraseTime;
}

SlashObject::SlashObject(int x1, int y1, int x2, int y2, GraphHandle* handle, int slashCountSum, int energyEraseTime) :
	Object(x1, y1, x2, y2, 0)
{
	m_characterId = -1;
	m_groupId = -1;
	m_handle = handle;
	m_damage = 0;
	m_slashCountSum = slashCountSum;
	m_cnt = 0;
	m_slashImpactX = 0;
	m_slashImpactY = 0;
	m_effectHandles_p = nullptr;
	m_soundHandle_p = -1;
	m_energyEraseTime = energyEraseTime;
}

// 大きさを指定しない場合。画像からサイズ取得。生存時間、AttackInfo
SlashObject::SlashObject(int x, int y, GraphHandle* handle, int slashCountSum, int energyEraseTime, AttackInfo* attackInfo) {
	int x2 = 0;
	int y2 = 0;
	GetGraphSize(handle->getHandle(), &x2, &y2);
	x2 += x;
	y2 = y;
	m_hp = attackInfo->slashHp();
	SlashObject(x, y, x2, y2, handle, slashCountSum, energyEraseTime, attackInfo);
}

// キャラとの当たり判定
// 当たっているならキャラを操作する。
bool SlashObject::atari(CharacterController* characterController) {
	// 自滅防止
	if (m_characterId == characterController->getAction()->getCharacter()->getId()) {
		return false;
	}
	// チームキル防止
	int groupId = characterController->getAction()->getCharacter()->getGroupId();
	if (m_groupId == groupId || groupId == -1) {
		return false;
	}

	// キャラの情報　座標と移動スピード
	int characterX1 = characterController->getAction()->getCharacter()->getX();
	int characterY1 = characterController->getAction()->getCharacter()->getY();
	int characterX2 = characterX1 + characterController->getAction()->getCharacter()->getWide();
	int characterY2 = characterY1 + characterController->getAction()->getCharacter()->getHeight();
	characterController->getAction()->getCharacter()->getDamageArea(&characterX1, &characterY1, &characterX2, &characterY2);

	// 当たり判定
	if (characterX2 > m_x1 && characterX1 < m_x2 && characterY2 > m_y1 && characterY1 < m_y2 && characterController->getAction()->ableDamage()) {
		// 貫通しない斬撃なら消滅
		// m_deleteFlag = true;
		if (characterX1 + characterX2 < m_x1 + m_x2) {
			characterController->damage(-m_slashImpactX, -m_slashImpactY, m_damage);
		}
		else {
			characterController->damage(m_slashImpactX, -m_slashImpactY, m_damage);
		}
		m_stopCnt = SLASH_STOP_CNT;
		return true;
	}
	return false;
}

// 他オブジェクトに対する当たり判定
bool SlashObject::atariToObject(Object* object) {
	// 味方の攻撃
	if (m_groupId == object->getGroupId()) {
		return false;
	}
	// 当たっているなら
	if (m_x2 > object->getX1() && m_x1 < object->getX2() && m_y2 > object->getY1() && m_y1 < object->getY2()) {
		return object->decreaseHp(m_damage, m_id);
	}
	return false;
}

void SlashObject::action() {
	if (m_damageCnt > 0) { m_damageCnt--; }
	// 時間経過
	m_cnt++;

	// 時間が来たので消滅
	if (m_cnt == m_slashCountSum) {
		m_deleteFlag = true;
	}
}

Item* SlashObject::createAttackEnergy() {
	if (m_energyEraseTime > 0 && m_cnt == m_slashCountSum - 1) {
		EnergyItem* energyItem = new EnergyItem("energy", (m_x1 + m_x2) / 2, (m_y1 + m_y2) / 2, m_damage, m_energyEraseTime);
		m_energyEraseTime = 0; // ヒットストップなどが起きてもエネルギー放出は一度だけ
		return energyItem;
	}
	return nullptr;
}

BombObject::BombObject(int x, int y, int dx, int dy, int damage, Animation* bombAnimation) :
	Object(x - dx / 2, y - dy / 2, x + dx / 2, y + dy / 2, -1)
{
	m_animation = bombAnimation;
	m_x = x;
	m_y = y;
	m_dx = dx;
	m_dy = dy;
	dx /= 2;
	dy /= 2;
	m_distance = (int)sqrt(dx * dx + dy * dy);
	m_damage = damage;
	// デフォルトでは無敵キャラ以外の全員に当たる
	m_characterId = -1;
	m_groupId = -1;
}

BombObject::~BombObject() {
	delete m_animation;
}

// キャラとの当たり判定
// 当たっているならキャラを操作する。
bool BombObject::atari(CharacterController* characterController) {
	// まだ判定なし
	if (!ableDamage()) { return false; }

	// 自滅防止
	if (m_characterId == characterController->getAction()->getCharacter()->getId()) {
		return false;
	}
	// チームキル防止
	int groupId = characterController->getAction()->getCharacter()->getGroupId();
	if (m_groupId == groupId || groupId == -1) {
		return false;
	}

	// キャラの情報　座標と移動スピード
	int characterX1 = characterController->getAction()->getCharacter()->getX();
	int characterY1 = characterController->getAction()->getCharacter()->getY();
	int characterX2 = characterX1 + characterController->getAction()->getCharacter()->getWide();
	int characterY2 = characterY1 + characterController->getAction()->getCharacter()->getHeight();
	characterController->getAction()->getCharacter()->getDamageArea(&characterX1, &characterY1, &characterX2, &characterY2);

	// 当たり判定
	if (characterX2 > m_x1 && characterX1 < m_x2 && characterY2 > m_y1 && characterY1 < m_y2 && characterController->getAction()->ableDamage()) {
		double damageRate = 1.0;
		int x = characterX1, y = abs(characterY1 - m_y);
		if (abs(characterX2 - m_x) < abs(characterX1 - m_x)) {
			x = characterX2;
		}
		if (abs(characterY2 - m_y) < abs(characterY1 - m_y)) {
			y = characterY2;
		}
		damageRate = calcDamageRate(x, y);
		int bombImpact = (int)(damageRate * BOMB_IMPACT);
		if (characterX1 + characterX2 < m_x1 + m_x2) {
			characterController->damage(-bombImpact, -bombImpact, (int)(m_damage * damageRate));
		}
		else {
			characterController->damage(bombImpact, -bombImpact, (int)(m_damage * damageRate));
		}
		return true;
	}
	return false;
}

// 他オブジェクトに対する当たり判定
bool BombObject::atariToObject(Object* object) {
	// まだ判定なし
	if (!ableDamage()) { return false; }
	// どちらかが破壊不能オブジェクト
	if (m_groupId == object->getGroupId()) {
		return false;
	}
	// 当たっているなら
	if (m_x2 > object->getX1() && m_x1 < object->getX2() && m_y2 > object->getY1() && m_y1 < object->getY2()) {
		int damage = (int)(m_damage * calcDamageRate(min(abs(m_x - object->getX1()), abs(m_x - object->getX2())), min(abs(m_y - object->getY1()), abs(m_y - object->getY2()))));
		return object->decreaseHp(damage, m_id);
	}
	return false;
}

// 動くオブジェクト用 毎フレーム行う
void BombObject::action() {
	m_animation->count();
	if (m_animation->getFinishFlag()) {
		m_deleteFlag = true;
	}
	else {
		m_handle = m_animation->getHandle();
		// 攻撃範囲に合わせて画像の拡大率を設定
		int graphX = 0, graphY = 0;
		GetGraphSize(m_handle->getHandle(), &graphX, &graphY);
		int graphSize = min(graphX, graphY);
		m_handle->setEx((double)m_distance / (double)graphSize);
	}
}

double BombObject::calcDamageRate(int x, int y) {
	x -= m_x;
	y -= m_y;
	double distance = sqrt(x * x + y * y);
	return min(1.0, (m_distance - distance) / m_distance);
}

bool BombObject::ableDamage() {
	return m_animation->getAnimeNum() > 2 && m_animation->getAnimeNum() < 6;
}


DoorObject::DoorObject(int x1, int y1, int x2, int y2, const char* fileName, int areaNum) :
	Object(x1, y1, x2, y2)
{
	m_fileName = fileName;
	string filePath = "picture/stageMaterial/";
	filePath += m_fileName;
	m_handle = new GraphHandle(filePath.c_str(), 1.0, 0.0, true);
	m_areaNum = areaNum;
	m_text = "";
	m_defaultText = "Ｗキーで入る";
	m_textNum = -1;
	m_textDisp = false;
}

DoorObject::~DoorObject() {
	delete m_handle;
}

bool DoorObject::atari(CharacterController* characterController) {
	if (!characterController->getAction()->ableDamage() || !characterController->getAction()->getGrand()) {
		m_textDisp = false;
		return false;
	}
	// キャラの情報　座標と移動スピード
	int characterX1 = characterController->getAction()->getCharacter()->getX();
	int characterY1 = characterController->getAction()->getCharacter()->getY();
	int characterX2 = characterX1 + characterController->getAction()->getCharacter()->getWide();
	int characterY2 = characterY1 + characterController->getAction()->getCharacter()->getHeight();
	characterController->getAction()->getCharacter()->getAtariArea(&characterX1, &characterY1, &characterX2, &characterY2);

	// 当たり判定
	if (characterX2 > m_x1 && characterX1 < m_x2 && characterY2 > m_y1 && characterY1 < m_y2) {
		m_textDisp = true;
		return true;
	}
	m_textDisp = false;
	return false;
}

StageObject::StageObject(int x1, int y1, int x2, int y2, const char* fileName, int textNum) :
	DoorObject(x1, y1, x2, y2, fileName, -1)
{
	m_textNum = textNum;
	if (textNum == -1) {
		m_defaultText = "";
	}
	else {
		m_defaultText = "Ｗキーで調べる";
	}
}

StageObject::~StageObject() {
	// DoorObjectでdeleteされるので不要
	//delete m_handle;
}


// コピー作成
Object* BoxObject::createCopy() {
	Object* res = new BoxObject(m_x1, m_y1, m_x2, m_y2, m_fileName.c_str(), m_color, m_hp);
	setParam(res);
	return res;
}
Object* TriangleObject::createCopy() {
	Object* res = new TriangleObject(m_x1, m_y1, m_x2, m_y2, m_fileName.c_str(), m_color, m_leftDown, m_hp);
	setParam(res);
	return res;
}
Object* BulletObject::createCopy() {
	BulletObject* res = new BulletObject(m_x1, m_y1, m_color, m_gx, m_gy, m_energyEraseTime);
	setParam(res);
	setBulletParam(res);
	setEnergyCnt(m_energyCnt);
	return res;
}
void BulletObject::setBulletParam(BulletObject* object) {
	object->setCharacterId(m_characterId);
	object->setGroupId(m_groupId);
	object->setColor(m_color);
	object->setRx(m_rx);
	object->setRy(m_ry);
	object->setV(m_v);
	object->setVx(m_vx);
	object->setVy(m_vy);
	object->setGx(m_gx);
	object->setGy(m_gy);
	object->setD(m_d);
	object->setDamage(m_damage);
	object->setEffectHandles(m_effectHandles_p);
	object->setGraphHandle(m_handle);
	
}
Object* ParabolaBullet::createCopy() {
	ParabolaBullet* res = new ParabolaBullet(m_x1, m_y1, m_handle, m_gx, m_gy, m_energyEraseTime);
	setParam(res);
	setBulletParam(res);
	res->setGraphHandle(m_handle);
	return res;
}
Object* SlashObject::createCopy() {
	SlashObject* res = new SlashObject(m_x1, m_y1, m_x2, m_y2, m_handle, m_slashCountSum, m_energyEraseTime);
	setParam(res);
	setSlashParam(res);
	return res;
}
void SlashObject::setSlashParam(SlashObject* object) {
	object->setCharacterId(m_characterId);
	object->setGroupId(m_groupId);
	object->setDamage(m_damage);
	object->setGraphHandle(m_handle);
	object->setCnt(m_cnt);
	object->setSlashImpactX(m_slashImpactX);
	object->setSlashImpactY(m_slashImpactY);
	object->setEffectHandles(m_effectHandles_p);
}
Object* BombObject::createCopy() {
	BombObject* res = new BombObject(m_x, m_y, m_dx, m_dy, m_damage, m_animation->createCopy());
	return res;
}
Object* DoorObject::createCopy() {
	DoorObject* res = new DoorObject(m_x1, m_y1, m_x2, m_y2, m_fileName.c_str(), m_areaNum);
	setParam(res);
	res->setText(m_text.c_str());
	res->setTextDisp(m_textDisp);
	return res;
}
Object* StageObject::createCopy() {
	StageObject* res = new StageObject(m_x1, m_y1, m_x2, m_y2, m_fileName.c_str(), m_textNum);
	setParam(res);
	res->setText(m_text.c_str());
	res->setTextDisp(m_textDisp);
	return res;
}


// 描画用
// オブジェクト描画（画像がないときに使う）
void BoxObject::drawObject(int x1, int y1, int x2, int y2) const {
	if (m_damageCnt > 0) {
		DrawBox(x1, y1, x2, y2, RED, TRUE);
	}
	else {
		DrawBox(x1, y1, x2, y2, m_color, TRUE);
	}
}
// オブジェクト描画（画像がないときに使う）
void TriangleObject::drawObject(int x1, int y1, int x2, int y2) const {
	if (m_leftDown) {
		if (m_damageCnt > 0) {
			DrawTriangle(x2, y1, x2, y2, x1, y2, RED, TRUE);
		}
		else {
			DrawTriangle(x2, y1, x2, y2, x1, y2, m_color, TRUE);
		}
	}
	else {
		if (m_damageCnt > 0) {
			DrawTriangle(x1, y1, x2, y2, x1, y2, RED, TRUE);
		}
		else {
			DrawTriangle(x1, y1, x2, y2, x1, y2, m_color, TRUE);
		}
	}
}
// オブジェクト描画（画像がないときに使う）
void BulletObject::drawObject(int x1, int y1, int x2, int y2) const {
	int rx = abs(x2 - x1) / 2;
	int ry = abs(y2 - y1) / 2;
	if (m_damageCnt > 0) {
		DrawOval(x1 + rx, y1 + ry, rx, ry, RED, TRUE);
	}
	else {
		DrawOval(x1 + rx, y1 + ry, rx, ry, m_color, TRUE);
	}
}