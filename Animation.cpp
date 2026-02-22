#include "Animation.h"
#include "AnimationDrawer.h"
#include "Character.h"
#include "Control.h"
#include "GraphHandle.h"
#include "Sound.h"
#include "DrawTool.h"
#include "Define.h"
#include "DxLib.h"

#include<string>

using namespace std;


/*
* アニメーションのクラス
*/
Animation::Animation(int x, int y, int frameCnt, GraphHandles* graphHandles) {
	m_x = x;
	m_y = y;
	m_vx = 0;
	m_vy = 0;
	m_movable = false;
	m_handles_p = graphHandles;
	m_frameCnt = frameCnt;
	m_loopFlag = false;
	m_character_p = nullptr;
	m_characterId = -1;
	init();
}

void Animation::setCharacter(Character* character) {
	m_character_p = character;
	m_characterId = m_character_p->getId();
}

// 初期化
void Animation::init() {
	m_cnt = 0;
	m_finishCnt = m_frameCnt * m_handles_p->getSize();
	m_finishFlag = false;
}

// アニメーションの切り替え
void Animation::changeGraph(GraphHandles* nextGraph, int frameCnt) {
	m_handles_p = nextGraph;
	if (frameCnt > 0) { m_frameCnt = frameCnt; }
	init();
}


// カウント
void Animation::count() { 
	if (m_character_p != nullptr) {
		m_x = m_character_p->getCenterX();
		m_y = m_character_p->getCenterY();
	}
	else if (m_movable) {
		m_x += m_vx;
		m_y += m_vy;
	}
	if (m_cnt == m_finishCnt) {
		if (m_loopFlag) {
			init();
		}
		else {
			m_finishFlag = true;
		}
	}
	else {
		m_cnt++;
	}
};

// 描画用
GraphHandle* Animation::getHandle() const {
	return m_handles_p->getGraphHandle((m_cnt - 1) / m_frameCnt);
}



/*
* 動画の基底クラス
*/
Movie::Movie(SoundPlayer* soundPlayer_p) {
	OWNER_NAME = "Kuriu Minoki";
	getGameEx(m_exX, m_exY);
	m_ex = min(m_exX, m_exY);
	m_finishFlag = false;
	m_cnt = 0;
	m_animation = nullptr;
	m_animationDrawer = new AnimationDrawer(m_animation);
	m_soundPlayer_p = soundPlayer_p;
	m_bgmPath = "";
	m_originalBgmPath = m_soundPlayer_p->getBgmName();

	m_frameWide = (GAME_WIDE - (int)(GAME_WIDE_DEFAULT * m_ex)) / 2;
	m_frameHeight = (GAME_HEIGHT - (int)(GAME_HEIGHT_DEFAULT * m_ex)) / 2;

	// フォントデータ
	m_textHandle = CreateFontToHandle(nullptr, (int)(TEXT_SIZE * m_exX), 3);
}

Movie::~Movie() {
	if (m_animation != nullptr) {
		delete m_animation;
	}
	m_soundPlayer_p->setBGM(m_originalBgmPath);
	// フォントデータ削除
	DeleteFontToHandle(m_textHandle);
}

void Movie::play() {

	if (m_cnt == 0) {
		// 音楽開始
		if (m_bgmPath != "") {
			m_soundPlayer_p->setBGM(m_bgmPath.c_str());
			m_soundPlayer_p->playBGM();
		}
		m_soundPlayer_p->clearSoundQueue();
	}

	m_cnt++;

	// メイン画像
	if (m_animation != nullptr) {
		m_animation->count();
	}

	// Zキー長押しで終了
	if (controlZ() > 0) {
		if (m_skipCnt++ == FPS_N) {
			m_finishFlag = true;
		}
	}
	else { m_skipCnt = 0; }
}

void Movie::draw() {
	if (m_animation != nullptr) {
		m_animationDrawer->setAnimation(m_animation);
		m_animationDrawer->drawAnimation();
	}
}

void Movie::drawframe() {
	if (m_frameWide > 0) {
		DrawBox(0, 0, m_frameWide + 1, GAME_HEIGHT, BLACK, TRUE);
		DrawBox(GAME_WIDE - m_frameWide - 1, 0, GAME_WIDE, GAME_HEIGHT, BLACK, TRUE);
	}
	if (m_frameHeight > 0) {
		DrawBox(0, 0, GAME_WIDE, m_frameHeight + 1, BLACK, TRUE);
		DrawBox(0, GAME_HEIGHT - m_frameHeight - 1, GAME_WIDE, GAME_HEIGHT, BLACK, TRUE);
	}

	// デバッグ用
	//DrawFormatString(0, GAME_HEIGHT - 100, BLACK, "COUNT = %d", m_cnt);
	//DrawFormatString(0, GAME_HEIGHT - 50, WHITE, "COUNT = %d", m_cnt);
}


// オープニング
ChapterOneED::ChapterOneED(SoundPlayer* soundPlayer_p):
	Movie(soundPlayer_p)
{
	string path = "picture/movie/chapter1ed/";

	// 画像
	m_walkSaeru = new GraphHandles((path + "歩き").c_str(), 4, m_ex * 3, 0, true);
	m_haikei[0] = LoadGraph((path + "昼.png").c_str());
	m_haikei[1] = LoadGraph((path + "雨.png").c_str());
	m_haikei[2] = LoadGraph((path + "夕方.png").c_str());
	m_haikei[3] = LoadGraph((path + "夜.png").c_str());
	m_elina = LoadGraph((path + "エリーナ.png").c_str());
	m_deadSaeru = LoadGraph((path + "倒れるサエル.png").c_str());

	// クレジット
	m_credit[0] = "企画：" + OWNER_NAME;
	m_credit[1] = "キャラクターデザイン：" + OWNER_NAME;
	m_credit[2] = "一章エンディングテーマ";
	m_credit[3] = "システムプランナー：" + OWNER_NAME;
	m_credit[4] = "システムグラフィック／ムービー：" + OWNER_NAME;
	m_credit[5] = "プログラム：" + OWNER_NAME;
	m_credit[6] = "プロデュース：" + OWNER_NAME;

	// 最初の画像設定
	m_centerX = GAME_WIDE / 2;
	m_centerY = GAME_HEIGHT / 2;
	m_animation = new Animation(m_centerX, m_centerY, 20, m_walkSaeru);
	m_animation->setLoopFlag(true);

	// BGM
	m_bgmPath = "sound/music/1章ED.mp3";

	m_bright = 0;
	m_toDark = false;
	m_nowHaikei = 0;
	m_elinaFlag = false;
	m_saeruFlag = false;
}

ChapterOneED::~ChapterOneED() {

	delete m_walkSaeru;
	for (int i = 0; i < 4; i++) {
		DeleteGraph(m_haikei[i]);
	}

	// 音楽を止める
	m_soundPlayer_p->deleteBGM();
}

void ChapterOneED::play() {

	static int internalCnt = 0;
	if (m_cnt == 0) { internalCnt = 0; }

	// カウント
	Movie::play();

	if (m_toDark) {
		if (m_bright > 0) { m_bright = max(0, m_bright - 2); }
	}
	else {
		if (m_bright < 255) { m_bright = min(255, m_bright + 2); }
	}

	// 画像を設定
	if (m_cnt == 1400) {
		m_toDark = true;
	}
	else if (m_cnt < 2800 && m_bright == 0) {
		m_toDark = false;
		m_nowHaikei++; // 雨
	}
	else if (m_cnt == 2800) {
		m_toDark = true;
	}
	else if (m_cnt < 4200 && m_bright == 0) {
		m_toDark = false;
		m_nowHaikei++; // 夕方
	}
	else if (m_cnt == 4200) {
		m_toDark = true;
	}
	else if (m_cnt < 5400 && m_bright == 0) {
		m_toDark = false;
		m_nowHaikei++; // 夜
	}
	else if (m_cnt == 5400) {
		m_toDark = true;
	}
	else if (m_cnt < 5900 && m_bright == 0) {
		m_toDark = false;
		m_nowHaikei = 0; // 昼
		m_saeruFlag = true;
	}
	else if (m_cnt == 5900) {
		m_toDark = true;
	}
	else if (m_cnt < 6550 && m_bright == 0) {
		m_toDark = false;
		m_nowHaikei = 0; // 昼のまま
		m_elinaFlag = true;
	}

	// 終了
	if (m_cnt == 6550) {
		m_finishFlag = true;
	}
}

void ChapterOneED::draw() {
	// DrawBox(0, 0, GAME_WIDE, GAME_HEIGHT, WHITE, TRUE);

	SetDrawBright(m_bright, m_bright, m_bright);

	// 背景
	DrawRotaGraph(GAME_WIDE / 2, GAME_HEIGHT / 2, m_ex * 3, 0.0, m_haikei[m_nowHaikei], TRUE);

	if (m_saeruFlag) {
		DrawRotaGraph(GAME_WIDE / 2, GAME_HEIGHT / 2, m_ex * 3, 0.0, m_deadSaeru, TRUE);
		if (m_elinaFlag) {
			DrawRotaGraph(GAME_WIDE / 2, GAME_HEIGHT / 2, m_ex * 3, 0.0, m_elina, TRUE);
		}
	}
	else {
		Movie::draw();
	}

	if (m_cnt > 150 && m_cnt < 650) {
		DrawFormatStringToHandle(GAME_WIDE / 2 - TEXT_SIZE * m_exX * 5, 100 * m_exY, BLACK, m_textHandle, m_credit[0].c_str());
	}
	else if (m_cnt > 750 && m_cnt < 1250) {
		DrawFormatStringToHandle(GAME_WIDE / 2 - TEXT_SIZE * m_exX * 9, 100 * m_exY, BLACK, m_textHandle, m_credit[1].c_str());
	}
	else if (m_cnt > 1650 && m_cnt < 2150) {
		DrawFormatStringToHandle(GAME_WIDE / 2 - TEXT_SIZE * m_exX * 5, 100 * m_exY, BLACK, m_textHandle, m_credit[2].c_str());
		DrawFormatStringToHandle(GAME_WIDE / 2 - TEXT_SIZE * m_exX * 5, 150 * m_exY, BLACK, m_textHandle, "「still」");
		DrawFormatStringToHandle(GAME_WIDE / 2 - TEXT_SIZE * m_exX * 5, 200 * m_exY, BLACK, m_textHandle, "唄：ＫＥＩ（謎の人物Ｋ）");
		DrawFormatStringToHandle(GAME_WIDE / 2 - TEXT_SIZE * m_exX * 5, 250 * m_exY, BLACK, m_textHandle, "素材提供：魔王魂");
	}
	else if (m_cnt > 2250 && m_cnt < 2750) {
		DrawFormatStringToHandle(GAME_WIDE / 2 - TEXT_SIZE * m_exX * 9, 100 * m_exY, BLACK, m_textHandle, m_credit[3].c_str());
	}
	else if (m_cnt > 3100 && m_cnt < 3600) {
		DrawFormatStringToHandle(GAME_WIDE / 2 - TEXT_SIZE * m_exX * 12, 100 * m_exY, BLACK, m_textHandle, m_credit[4].c_str());
	}
	else if (m_cnt > 3700 && m_cnt < 4200) {
		DrawFormatStringToHandle(GAME_WIDE / 2 - TEXT_SIZE * m_exX * 6, 100 * m_exY, BLACK, m_textHandle, m_credit[5].c_str());
	}
	else if (m_cnt > 4500 && m_cnt < 5100) {
		DrawFormatStringToHandle(GAME_WIDE / 2 - TEXT_SIZE * m_exX * 7, 100 * m_exY, BLACK, m_textHandle, m_credit[6].c_str());
	}

	SetDrawBright(255, 255, 255);
	
	drawframe();

	// Zキー長押しでスキップの表示
	drawSkip(m_skipCnt, m_exX, m_exY, m_textHandle);
}


OpMovieMp4::OpMovieMp4(SoundPlayer* soundPlayer_p) :
	Movie(soundPlayer_p)
{
	m_mp4 = LoadGraph("picture/movie/DuplicationHeartOp.mp4");
	double rate = 10 * m_soundPlayer_p->getVolume() / 100.0;
	rate = log10(rate);
	int volume = (int)(rate * 3000);
	SetMovieVolumeToGraph(7000 + volume, m_mp4);
	PlayMovieToGraph(m_mp4);
}
OpMovieMp4::~OpMovieMp4() {
	DeleteGraph(m_mp4);
}

// 再生
void OpMovieMp4::play() {
	Movie::play();
	// 終了
	if (GetMovieStateToGraph(m_mp4) != 1) {
		m_finishFlag = true;
	}
}

// 描画
void OpMovieMp4::draw() {

	DrawRotaGraph(GAME_WIDE / 2, GAME_HEIGHT / 2, m_ex, 0, m_mp4, TRUE);

	drawframe();

	// Zキー長押しでスキップの表示
	drawSkip(m_skipCnt, m_exX, m_exY, m_textHandle);

	// 画面のちらつき防止
	WaitTimer(15);
}