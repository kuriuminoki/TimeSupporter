#ifndef CHARACTER_DRAWER_H_INCLUDED
#define CHARACTER_DRAWER_H_INCLUDED

class Camera;
class CharacterAction;
class Character;

// 体力バーを表示
void drawHpBar(int x1, int y1, int x2, int y2, int hp, int prevHp, int maxHp, int damageColor, int prevHpColor, int hpColor);

// CharacterActionを見てキャラを描画する
class CharacterDrawer {
private:

	// デバッグ用
	const bool ATARI_DEBUG = false;
	int m_atariGuideHandle;
	int m_damageGuideHandle;

	// キャラの動きの情報 const関数しか呼ばない
	const CharacterAction* m_characterAction;

	const int HP_BAR_WIDE = 200;
	const int HP_BAR_HEIGHT = 20;
	static int HP_COLOR;
	static int PREV_HP_COLOR;
	static int DAMAGE_COLOR;

	// 点滅用
	int m_cnt;

	// 1920を基準としたGAME_WIDEの倍率
	double m_exX;
	// 1080を基準としたGAME_HEIGHTの倍率
	double m_exY;

public:

	CharacterDrawer(const CharacterAction* const characterAction);

	~CharacterDrawer();

	// セッタ
	void setCharacterAction(const CharacterAction* action) { m_characterAction = action; }

	void drawCharacter(const Camera* const camera, int enemyNoticeHandle, int bright = 255);

	void drawPlayerHpBar(int x, int y, int wide, int height, const Character* player, int hpBarGraph);
	void drawPlayerSkillBar(int x, int y, int wide, int height, const Character* player, int hpBarGraph);
	void drawFollowHpBar(int x, int y, int wide, int height, const Character* player, int hpBarGraph, int font);
	void drawBossHpBar(int x, int y, int wide, int height, const Character* player, int hpBarGraph);

};

#endif