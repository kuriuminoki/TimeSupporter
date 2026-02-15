#ifndef WORLD_DRAWER_H_INCLUDED
#define WORLD_DRAWER_H_INCLUDED

class World;
class Camera;
class CharacterDrawer;
class ObjectDrawer;
class AnimationDrawer;
class ConversationDrawer;


class TargetDrawer {
private:
	const double TARGET_EX = 0.3;
	double m_ex;
	int m_targetHandle1;
	int m_targetHandle2;
	int m_targetHandle3;
	const double PI = 3.14;
	double m_targetAngle1;
	double m_targetAngle2;
	double m_targetAngle3;
public:
	TargetDrawer();
	~TargetDrawer();

	void setEx(double ex) { m_ex = ex * TARGET_EX; }

	void draw();

private:
	void anime();
};


// 世界を描画する
class WorldDrawer {
private:

	// 描画する世界
	const World* m_world;

	// 背景画像
	int m_noonHaikei;
	int m_eveningHaikei;
	int m_nightHaikei;

	// お金の描画枠
	int m_moneyBoxGraph;

	// キャラ描画用
	CharacterDrawer* m_characterDrawer;

	// オブジェクト描画用
	ObjectDrawer* m_objectDrawer;

	// アニメーション描画用
	AnimationDrawer* m_animationDrawer;

	// マウスカーソル代わり
	TargetDrawer m_targetDrawer;

	// HPバー
	int m_hpBarGraph;

	// skillバー
	int m_skillBarGraph;

	// 仲間のHPバー
	int m_followHpBarGraph;

	// ボスのHPバー
	int m_bossHpBarGraph;

	// 会話イベント
	ConversationDrawer* m_conversationDrawer;

	int m_enemyNotice;

	double m_exX, m_exY;

	int m_font;

	int m_followerNameFont;

public:
	WorldDrawer(const World* world);

	~WorldDrawer();

	// ゲッタ
	inline const World* getWorld() const { return m_world; }

	// セッタ
	inline void setWorld(World* world) { m_world = world; }

	// 描画する
	void draw(bool drawSkillBar);

private:

	void drawBattleField(const Camera* camera, int bright, bool drawSkillBar);
};

#endif