#ifndef GAME_DRAWER_H_INCLUDED
#define GAME_DRAWER_H_INCLUDED

class Game;
class WorldDrawer;

class GameDrawer {
private:
	const Game* m_game;

	int m_screenEffectHandle;

	// テキストやフォントのサイズの倍率
	double m_exX;
	double m_exY;

	WorldDrawer* m_worldDrawer;

	// スキルの情報のフォント
	const int SKILL_SIZE = 100;
	int m_skillHandle;

	// スキルの情報の枠
	int m_skillInfoHandle;
	int m_skillInfoBackHandle;

	// 時間
	int m_timeBarNoonHandle;
	int m_timeBarEveningHandle;
	int m_timeBarNightHandle;
	int m_needleHandle;

	// ゲームオーバーの画像
	int m_gameoverHandle;

	// スピードアップモード
	int m_quickModeHandle;
	const int QUICK_SIZE = 50;
	int m_quickModeFontHandle;
	int m_quickModeCnt = 0;

	// セーブ完了通知の画像
	int m_noticeSaveDataHandle;
	int m_noticeX, m_noticeY;
	double m_noticeEx;

	// 画面の加工用
	const int THIN = 4;
	int m_tmpScreenR;
	int m_tmpScreenG;
	int m_tmpScreenB;

public:
	GameDrawer(const Game* game);

	~GameDrawer();

	void draw(int screen);

private:
	// レトロゲーム風の画面加工を行う
	void filterRetroDisp(int screen);
};


#endif