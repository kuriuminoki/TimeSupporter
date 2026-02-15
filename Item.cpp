#include "Animation.h"
#include "Character.h"
#include "GraphHandle.h"
#include "Item.h"
#include "DxLib.h"

using namespace std;


/*
* アイテム
*/
Item::Item(const char* itemName, int x, int y) {
	m_itemName = itemName;
	m_x = x;
	m_y = y;
	m_cnt = 0;
	m_vx = 0;
	m_vy = 0;
	m_grand = false;
	m_sound = LoadSoundMem(("sound/item/" + m_itemName + ".wav").c_str());
	m_deleteFlag = false;
	m_exRate = 1.0;
}

// デストラクタ
Item::~Item() {
	DeleteSoundMem(m_sound);
	delete m_handles;
	delete m_animation;
}

// コピー
void Item::setParam(Item* item) {
	item->setCnt(m_cnt);
	item->setGrand(m_grand);
	item->setAnimation(m_animation->createCopy());
	item->setVx(m_vx);
	item->setVy(m_vy);
	item->setExRate(m_exRate);
}

void Item::loadItemGraph() {
	m_handles = new GraphHandles(("picture/item/" + m_itemName).c_str(), 1, 0.05 * m_exRate, 0.0, true);
	m_animation = new Animation(m_x, m_y, 10, m_handles);
}

void Item::setY(int y) {
	int wide = 0, height = 0;
	getGraphSize(&wide, &height);
	m_y = y - (height / 2);
	m_animation->setY(m_y);
}

// 取得済みかつ効果音が再生中じゃないなら削除してもらう
bool Item::getDeleteFlag() const { 
	return m_deleteFlag;
}

// アイテムの大きさ
void Item::getGraphSize(int* wide, int* height) const {
	double ex = m_animation->getHandle()->getEx();
	GetGraphSize(m_animation->getHandle()->getHandle(), wide, height);
	*wide = (int)(*wide * ex);
	*height = (int)(*height * ex);
}

// 座標
void Item::getPoint(int* x1, int* y1, int* x2, int* y2) {
	int wide = 0, height = 0;
	getGraphSize(&wide, &height);
	wide = (int)(wide * m_exRate);
	height = (int)(height * m_exRate);
	*x1 = m_x - (wide / 2);
	*y1 = m_y - (height / 2);
	*x2 = *x1 + wide;
	*y2 = *y1 + height;
}

// 毎フレームの初期化処理
void Item::init() {
	m_grand = false;
}

// 動き 毎フレーム呼ぶ
void Item::action() {

	m_cnt++;
	if (m_cnt > getEraseCnt()) {
		m_deleteFlag = true;
	}

	// アニメをリセット
	if (m_animation->getFinishFlag()) {
		m_animation->init();
	}

	// アニメを動かす
	m_animation->count();

	// 重力の影響を受ける
	if (getEnableGravity()) {
		if (m_grand) {
			// 着地してる
			m_vx = 0;
			m_vy = 0;
		}

		// 移動
		m_x += m_vx;
		m_y += m_vy;
		m_animation->setX(m_x);
		m_animation->setY(m_y);

		if (!m_grand) {
			// 重力
			m_vy += 1;
		}
	}

}

// プレイヤーとの当たり判定
bool Item::atariCharacter(Character* player) {

	// キャラの座標
	int cx1 = 0;
	int cy1 = 0;
	int cx2 = 0;
	int cy2 = 0;
	player->getAtariArea(&cx1, &cy1, &cx2, &cy2);
	
	// このアイテムの座標
	int x1 = 0, y1 = 0, x2 = 0, y2 = 0;
	getPoint(&x1, &y1, &x2, &y2);

	// 当たったか判定
	if (x2 > cx1 && x1 < cx2 && y2 > cy1 && y1 < cy2) {
		arrangePlayer(player);
		m_deleteFlag = true;
		return true;
	}

	return false;

}


/*
* 体力回復アイテム
*/
CureItem::CureItem(const char* itemName, int x, int y, int cureValue):
	Item(itemName, x, y)
{
	m_itemCode = ITEM_CURE;
	m_cureValue = cureValue;
	loadItemGraph();
}

// スキル発動用
Item* CureItem::createCopy() {
	CureItem* item = new CureItem(m_itemName.c_str(), m_x, m_y, m_cureValue);
	setParam(item);
	return item;
}

// プレイヤーに対するアクション
void CureItem::arrangePlayer(Character* player) {
	// HP回復
	player->setHp(player->getHp() + m_cureValue);
}


/*
* お金獲得アイテム
*/
MoneyItem::MoneyItem(const char* itemName, int x, int y, int moneyValue) :
	Item(itemName, x, y)
{
	m_itemCode = ITEM_MONEY;
	m_moneyValue = moneyValue;
	loadItemGraph();
}

// スキル発動用
Item* MoneyItem::createCopy() {
	MoneyItem* item = new MoneyItem(m_itemName.c_str(), m_x, m_y, m_moneyValue);
	setParam(item);
	return item;
}

// プレイヤーに対するアクション
void MoneyItem::arrangePlayer(Character* player) {
	// お金獲得
	player->setMoney(player->getMoney() + m_moneyValue);
}


/*
* エネルギー獲得アイテム
*/
EnergyItem::EnergyItem(const char* itemName, int x, int y, int energyValue, int eraseTime) :
	Item(itemName, x, y)
{
	m_itemCode = ITEM_ENERGY;
	m_energyValue = energyValue;
	m_eraseTime = eraseTime;
	// 獲得量に応じてサイズアップ
	m_exRate = 1.0 + energyValue / 10;
	loadItemGraph();
}

// スキル発動用
Item* EnergyItem::createCopy() {
	EnergyItem* item = new EnergyItem(m_itemName.c_str(), m_x, m_y, m_energyValue, m_eraseTime);
	setParam(item);
	return item;
}

void EnergyItem::loadItemGraph() {
	m_handles = new GraphHandles(("picture/item/" + m_itemName).c_str(), 2, 0.2 * m_exRate, 0.0, true);
	m_animation = new Animation(m_x, m_y, 3, m_handles);
}

// プレイヤーに対するアクション
void EnergyItem::arrangePlayer(Character* player) {
	// エネルギー獲得
	player->setSkillGage(player->getSkillGage() + m_energyValue);
}