#include "GameDrawer.h"
#include "Game.h"
#include "TimeSupporter.h"
#include "Story.h"
#include "World.h"
#include "WorldDrawer.h"
#include "PausePage.h"
#include "Define.h"
#include "DxLib.h"
#include <string>
#include <sstream>


using namespace std;


GameDrawer::GameDrawer(const Game* game) {
	m_game = game;

	m_screenEffectHandle = LoadGraph("picture/battleMaterial/screen.png");

	getGameEx(m_exX, m_exY);

	m_worldDrawer = new WorldDrawer(nullptr);

	m_skillHandle = CreateFontToHandle(nullptr, (int)(SKILL_SIZE * m_exX), 10);

	m_skillInfoHandle = LoadGraph("picture/battleMaterial/skillInfo.png");
	m_skillInfoBackHandle = LoadGraph("picture/battleMaterial/skillInfoBack.png");

	m_gameoverHandle = LoadGraph("picture/system/gameover.png");

	m_noticeSaveDataHandle = LoadGraph("picture/system/noticeSaveDone.png");
	m_noticeEx = 0.3;
	GetGraphSize(m_noticeSaveDataHandle, &m_noticeX, &m_noticeY);
	m_noticeX = (int)(m_noticeX * (m_exX / 2 * m_noticeEx));
	m_noticeY = (int)(m_noticeY * (m_exY / 2 * m_noticeEx));
	m_noticeX += (int)(10 * m_exX);
	m_noticeY = GAME_HEIGHT - m_noticeY - (int)(10 * m_exY);

	m_tmpScreenR = MakeScreen(GAME_WIDE, GAME_HEIGHT, TRUE);
	m_tmpScreenG = MakeScreen(GAME_WIDE, GAME_HEIGHT, TRUE);
	m_tmpScreenB = MakeScreen(GAME_WIDE, GAME_HEIGHT, TRUE);
}

GameDrawer::~GameDrawer() {
	delete m_worldDrawer;
	DeleteFontToHandle(m_skillHandle);
	DeleteGraph(m_screenEffectHandle);
	DeleteGraph(m_skillInfoHandle);
	DeleteGraph(m_skillInfoBackHandle);
	DeleteGraph(m_gameoverHandle);
	DeleteGraph(m_noticeSaveDataHandle);
	DeleteGraph(m_tmpScreenR);
	DeleteGraph(m_tmpScreenG);
	DeleteGraph(m_tmpScreenB);
}

void GameDrawer::draw(int screen) {

	if (m_game->getStory() == nullptr) {
		int handX = 0, handY = 0;
		GetMousePoint(&handX, &handY);
		m_game->getSelectStagePage()->draw(handX, handY);
	}
	else {
		// ゲームオーバー
		int gameoverCnt = m_game->getGameoverCnt();
		if (gameoverCnt > 0) {
			if ((gameoverCnt < FPS_N && gameoverCnt / 2 % 2 == 0) || gameoverCnt > FPS_N) {
				DrawRotaGraph(GAME_WIDE / 2, GAME_HEIGHT / 2, min(m_exX, m_exY) * 0.7, 0.0, m_gameoverHandle, TRUE);
			}

			return;
		}

		// 世界を描画
		m_worldDrawer->setWorld(m_game->getStory()->getWorld());
		m_worldDrawer->draw(false); // TODO: 必殺技バーをスキルバーにするなら引数にロジックを入れる
	}

	// セーブ完了通知
	int noticeSaveDone = m_game->getGameData()->getNoticeSaveDone();
	int alpha = 0;
	if (noticeSaveDone > 0) {
		if (noticeSaveDone * 3 > m_game->getGameData()->NOTICE_SAVE_DONE_TIME * 2) {
			alpha = min(255, (m_game->getGameData()->NOTICE_SAVE_DONE_TIME - noticeSaveDone) * 3);
		}
		else if (noticeSaveDone * 3 > m_game->getGameData()->NOTICE_SAVE_DONE_TIME) {
			alpha = 255;
		}
		else {
			alpha = max(0, noticeSaveDone * 3);
		}
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
		DrawRotaGraph(m_noticeX, m_noticeY, min(m_exX, m_exY) * m_noticeEx, 0.0, m_noticeSaveDataHandle, TRUE);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
	}

	// 一時停止画面
	if (m_game->getGamePause() != nullptr) {
		SetMouseDispFlag(TRUE);//マウス表示
		m_game->getGamePause()->draw();
	}
	else{
		SetMouseDispFlag(MOUSE_DISP);//マウス表示
	}
}
