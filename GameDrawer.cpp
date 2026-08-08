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
	m_noticeX = GAME_WIDE - m_noticeX;
	m_noticeY = GAME_HEIGHT - m_noticeY - (int)(10 * m_exY);

	m_tmpScreenR = MakeScreen(GAME_WIDE, GAME_HEIGHT, TRUE);
	m_tmpScreenG = MakeScreen(GAME_WIDE, GAME_HEIGHT, TRUE);
	m_tmpScreenB = MakeScreen(GAME_WIDE, GAME_HEIGHT, TRUE);

	m_font = CreateFontToHandle(nullptr, (int)(20 * m_exX), 3);

	for (int i = 0; i < LOADING_HANDLE_SUM; i++) {
		ostringstream oss;
		oss << "picture/system/loading" << i + 1 << ".png";
		m_loadingHandle[i] = LoadGraph(oss.str().c_str());
	}
	
	int wide, height;
	GetGraphSize(m_loadingHandle[0], &wide, &height);
	m_ex = min((double)GAME_WIDE / wide, (double)GAME_HEIGHT / height);
	m_needLoadNum = 0;
	m_useLoadingNum = 0;
}

GameDrawer::~GameDrawer() {
	delete m_worldDrawer;
	DeleteFontToHandle(m_skillHandle);
	DeleteFontToHandle(m_font);
	DeleteGraph(m_screenEffectHandle);
	DeleteGraph(m_skillInfoHandle);
	DeleteGraph(m_skillInfoBackHandle);
	DeleteGraph(m_gameoverHandle);
	DeleteGraph(m_noticeSaveDataHandle);
	DeleteGraph(m_tmpScreenR);
	DeleteGraph(m_tmpScreenG);
	DeleteGraph(m_tmpScreenB);
	for (int i = 0; i < LOADING_HANDLE_SUM; i++) {
		DeleteGraph(m_loadingHandle[i]);
	}
}

void GameDrawer::draw(int screen) {

	int aSyncLoadNum = GetASyncLoadNum();

	if (aSyncLoadNum == 0) {
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

		if (filterRetroDispFlag) {
			filterRetroDisp(screen);
		}
		filterRetroDispFlag = false; // フィルタが必要なら毎フレームtrueにする。基本はfalse
	}
	else {
		if (m_needLoadNum == 0) {
			m_useLoadingNum = GetRand(LOADING_HANDLE_SUM - 1);
		}
		m_needLoadNum = aSyncLoadNum == 0 ? 0 : max(m_needLoadNum, aSyncLoadNum);
		DrawRotaGraph(GAME_WIDE / 2, GAME_HEIGHT / 2, m_ex, 0.0, m_loadingHandle[m_useLoadingNum], TRUE);
		const int WIDE = 600 * m_exX;
		const int X = 50 * m_exX;
		const int HEIGHT = 50 * m_exY;
		const int Y = 200 * m_exY;
		int rate = 100;
		if (m_needLoadNum > 0) {
			rate = WIDE * (m_needLoadNum - aSyncLoadNum) / m_needLoadNum;
		}
		DrawBox(X, Y, X + rate, Y + HEIGHT, WHITE, TRUE);
		DrawBox(X + rate, Y, X + WIDE, Y + HEIGHT, BLACK, TRUE);
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
	else {
		SetMouseDispFlag(MOUSE_DISP);//マウス表示
		if (aSyncLoadNum == 0 && (m_game->getStory() == nullptr || m_worldDrawer->battleNow())) {
			ostringstream oss;
			oss << m_game->getPauseKeyName() << ": 一時停止";
			DrawStringToHandle(30 + m_exX, GAME_HEIGHT - 30 * m_exY, oss.str().c_str(), WHITE, m_font);
		}
	}
}

// レトロゲーム風の画面加工を行う
void GameDrawer::filterRetroDisp(int screen) {
	int fixThin = THIN * m_exX;
	SetDrawScreen(m_tmpScreenR);
	SetDrawBright(255, 0, 0);
	DrawGraph(0, 0, screen, TRUE);

	SetDrawScreen(m_tmpScreenG);
	SetDrawBright(0, 255, 0);
	DrawGraph(fixThin, fixThin / 2, screen, TRUE);
	GraphBlend(m_tmpScreenR, m_tmpScreenG, 255, DX_GRAPH_BLEND_SCREEN);

	SetDrawScreen(m_tmpScreenB);
	SetDrawBright(0, 0, 255);
	DrawGraph(fixThin / 2, fixThin, screen, TRUE);
	GraphBlend(m_tmpScreenR, m_tmpScreenB, 255, DX_GRAPH_BLEND_SCREEN);

	SetDrawScreen(screen);
	SetDrawBright(255, 255, 255);
	DrawGraph(0, 0, m_tmpScreenR, TRUE);
	GraphBlend(screen, m_screenEffectHandle, 100, DX_GRAPH_BLEND_OVERLAY);

	DrawBox(0, 0, fixThin, GAME_HEIGHT, BLACK, TRUE);
	DrawBox(0, 0, GAME_WIDE, fixThin, BLACK, TRUE);
	DrawBox(GAME_WIDE - fixThin, 0, GAME_WIDE, GAME_HEIGHT, BLACK, TRUE);
	DrawBox(0, GAME_HEIGHT - fixThin, GAME_WIDE, GAME_HEIGHT, BLACK, TRUE);
}
