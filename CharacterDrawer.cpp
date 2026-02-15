#include "CharacterDrawer.h"
#include "Camera.h"
#include "Character.h"
#include "CharacterAction.h"
#include "GraphHandle.h"
#include "Define.h"
#include "DxLib.h"

#include <cmath>


// 体力バーを表示
void drawHpBar(int x1, int y1, int x2, int y2, int hp, int prevHp, int maxHp, int damageColor, int prevHpColor, int hpColor) {
	int wide = x2 - x1;
	int prev = wide * prevHp / maxHp;
	int remain = wide * hp / maxHp;
	DrawBox(x1, y1, x1 + remain, y2, hpColor, TRUE);
	DrawBox(x1 + remain, y1, x1 + prev, y2, prevHpColor, TRUE);
	DrawBox(x1 + prev, y1, x2, y2, damageColor, TRUE);
}


// 敵がいる方向の通知
void drawEnemyNotice(int x, int y, double ex, int handle) {
	int wide = 0, height = 0;
	GetGraphSize(handle, &wide, &height);
	wide = (int)(wide * ex);
	height = (int)(height * ex);
	int drawX = max(0 + wide, min(GAME_WIDE - wide, x));
	int drawY = max(0 + wide, min(GAME_HEIGHT - wide, y));
	double r = atan2(y - GAME_HEIGHT / 2, x - GAME_WIDE / 2);
	DrawRotaGraph(drawX, drawY, ex, r, handle, TRUE);
}


int CharacterDrawer::HP_COLOR = GetColor(0, 255, 0);
int CharacterDrawer::PREV_HP_COLOR = GetColor(255, 0, 0);
int CharacterDrawer::DAMAGE_COLOR = GetColor(0, 0, 0);

CharacterDrawer::CharacterDrawer(const CharacterAction* const characterAction) {
	
	m_cnt = 0;
	getGameEx(m_exX, m_exY);

	// デバッグ用
	m_atariGuideHandle = LoadGraph("picture/stick/atariGuide.png"); // (青色)
	m_damageGuideHandle = LoadGraph("picture/stick/damageGuide.png"); // (赤色)
	m_characterAction = characterAction;

}

CharacterDrawer::~CharacterDrawer() {
	// デバッグ用
	DeleteGraph(m_atariGuideHandle);
	DeleteGraph(m_damageGuideHandle);
}

// キャラを描画する
void CharacterDrawer::drawCharacter(const Camera* const camera, int enemyNoticeHandle, int bright) {
	// 描画するキャラクター
	const Character* character = m_characterAction->getCharacter();

	// 画像の情報
	const GraphHandle* graphHandle = character->getGraphHandle();

	// 座標と拡大率取得
	int x1 = character->getX(), y1 = character->getY();
	int wide = character->getWide(), height = character->getHeight();
	double ex;
	// 画像の中心を座標とする
	int x = x1 + wide / 2;
	int y = y1 + height / 2;
	// 画像固有の拡大率取得
	ex = graphHandle->getEx();

	// カメラで調整
	camera->setCamera(&x, &y, &ex);

	// 画面外
	wide = (int)(wide * camera->getEx());
	height = (int)(height * camera->getEx());
	if (x - wide / 2 > GAME_WIDE || y - height / 2 > GAME_HEIGHT || x + wide / 2 < 0 || y + height / 2 < 0) {
		drawEnemyNotice(x, y, 0.5, enemyNoticeHandle);
		return;
	}

	// 描画 ダメージ状態なら点滅 y+1して宙に浮いて見えないようにする
	if (!m_characterAction->ableDamage() && ++m_cnt / 2 % 2 == 1) {
		int dark = max(0, bright - 100);
		SetDrawBright(dark, dark, dark);
		graphHandle->draw(x, y + 1, ex);
		SetDrawBright(bright, bright, bright);
	}
	else {
		graphHandle->draw(x, y + 1, ex);
	}

	if (character->getDispHpCnt() > 0 && character->getName() != "ハート" && !character->getBossFlag()) {
		// 座標をカメラで調整
		x = character->getX() + (character->getWide() / 2);
		y = character->getY();
		ex = graphHandle->getEx();
		camera->setCamera(&x, &y, &ex);
		int wide = (int)(HP_BAR_WIDE / 2 * camera->getEx());
		int height = (int)(HP_BAR_HEIGHT * camera->getEx());
		y -= (int)(10 * camera->getEx());
		// 体力の描画
		drawHpBar(x - wide, y - height, x + wide, y, character->getHp(), character->getPrevHp(), character->getMaxHp(), DAMAGE_COLOR, PREV_HP_COLOR, HP_COLOR);
	}

	// デバッグ用
	if (ATARI_DEBUG) {
		// 壁や床との当たり判定(青色)
		int x2 = 0, y2 = 0;
		character->getAtariArea(&x1, &y1, &x2, &y2);
		camera->setCamera(&x1, &y1, &ex);
		camera->setCamera(&x2, &y2, &ex);
		DrawExtendGraph(x1, y1, x2, y2, m_atariGuideHandle, TRUE);

		// ダメージの当たり判定(赤色)
		character->getDamageArea(&x1, &y1, &x2, &y2);
		camera->setCamera(&x1, &y1, &ex);
		camera->setCamera(&x2, &y2, &ex);
		DrawExtendGraph(x1, y1, x2, y2, m_damageGuideHandle, TRUE);
	}

}

void CharacterDrawer::drawPlayerHpBar(int x, int y, int wide, int height, const Character* player, int hpBarGraph) {

	// 解像度変更に対応
	x = (int)(x * m_exX);
	y = (int)(y * m_exY);
	wide = (int)(wide * m_exX);
	height = (int)(height * m_exY);

	DrawExtendGraph(x, y, x + wide, y + height, hpBarGraph, TRUE);

	int dx1 = (int)(230 * m_exX);
	int dx2 = (int)(40 * m_exX);
	int dy1 = (int)(15 * m_exY);
	int dy2 = (int)(15 * m_exY);

	// 体力の描画
	drawHpBar(x + dx1, y + dy1, x + wide - dx2, y + height - dy2, player->getHp(), player->getPrevHp(), player->getMaxHp(), DAMAGE_COLOR, PREV_HP_COLOR, HP_COLOR);

}

void CharacterDrawer::drawPlayerSkillBar(int x, int y, int wide, int height, const Character* player, int hpBarGraph) {

	// 解像度変更に対応
	x = (int)(x * m_exX);
	y = (int)(y * m_exY);
	wide = (int)(wide * m_exX);
	height = (int)(height * m_exY);

	DrawExtendGraph(x, y, x + wide, y + height, hpBarGraph, TRUE);

	int dx1 = (int)(200 * m_exX);
	int dx2 = (int)(65 * m_exX);
	int dy1 = (int)(10 * m_exY);
	int dy2 = (int)(10 * m_exY);

	// 体力の描画
	int skillGage = player->getSkillGage();
	int maxSkillGage = player->getMaxSkillGage();
	if (skillGage == maxSkillGage) {
		drawHpBar(x + dx1, y + dy1, x + wide - dx2, y + height - dy2, player->getSkillGage(), player->getSkillGage(), player->getMaxSkillGage(), WHITE, ORANGE, ORANGE);
	}
	else {
		drawHpBar(x + dx1, y + dy1, x + wide - dx2, y + height - dy2, player->getSkillGage(), player->getSkillGage(), player->getMaxSkillGage(), WHITE, DARK_ORANGE, DARK_ORANGE);
	}

}

void CharacterDrawer::drawFollowHpBar(int x, int y, int wide, int height, const Character* player, int hpBarGraph, int font) {

	// 解像度変更に対応
	x = (int)(x * m_exX);
	y = (int)(y * m_exY);
	wide = (int)(wide * m_exX);
	height = (int)(height * m_exY);

	DrawExtendGraph(x, y, x + wide, y + height, hpBarGraph, TRUE);

	int dx = (int)(20 * m_exX);
	int dy1 = (int)(50 * m_exY);
	int dy2 = (int)(10 * m_exY);

	// 体力の描画
	DrawStringToHandle(x, y, player->getName().c_str(), BLACK, font);
	drawHpBar(x + dx, y + dy1, x + wide - dx, y + height - dy2, player->getHp(), player->getPrevHp(), player->getMaxHp(), DAMAGE_COLOR, PREV_HP_COLOR, HP_COLOR);

}

void CharacterDrawer::drawBossHpBar(int x, int y, int wide, int height, const Character* player, int hpBarGraph) {

	// 解像度変更に対応
	x = (int)(x * m_exX);
	y = (int)(y * m_exY);
	wide = (int)(wide * m_exX);
	height = (int)(height * m_exY);

	DrawExtendGraph(x, y, x + wide, y + height, hpBarGraph, TRUE);

	int dx1 = (int)(370 * m_exX);
	int dx2 = (int)(100 * m_exX);
	int dy1 = (int)(20 * m_exY);
	int dy2 = (int)(20 * m_exY);

	// 体力の描画
	drawHpBar(x + dx1, y + dy1, x + wide - dx2, y + height - dy2, player->getHp(), player->getPrevHp(), player->getMaxHp(), DAMAGE_COLOR, PREV_HP_COLOR, HP_COLOR);

}