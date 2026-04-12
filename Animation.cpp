#include "Animation.h"
#include "AnimationDrawer.h"
#include "Character.h"
#include "Control.h"
#include "Text.h"
#include "TextDrawer.h"
#include "GraphHandle.h"
#include "Sound.h"
#include "DrawTool.h"
#include "Define.h"
#include "DxLib.h"

#include <sstream>
#include <string>

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

	if (m_animation != nullptr) {
		m_animationDrawer->setAnimation(m_animation);
	}
}

void Movie::draw() const {
	if (m_animation != nullptr) {
		m_animationDrawer->drawAnimation();
	}
}

void Movie::drawframe() const {
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

void ChapterOneED::draw() const {
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


/*
* 各章のED(共通部分)
*/
ChapterEDCommon::ChapterEDCommon(SoundPlayer* soundPlayer_p, int chapterNum):
	Movie(soundPlayer_p)
{
	ostringstream oss;
	if (chapterNum == 6) {
		oss << "NEXT: FINAL CHAPTER";
	}
	else {
		oss << "NEXT: CHAPTER " << chapterNum + 1;
	}
	m_chapterNumStr = oss.str();

	// 最初の画像設定
	m_nextHandles = nullptr;
	m_animation = nullptr;

	m_conversation = nullptr;
	m_conversationDrawer = nullptr;

	// BGM
	m_bgmPath = "sound/music/各章ED.mp3";

	m_bright = 255;

}
ChapterEDCommon::~ChapterEDCommon() {
	if (m_nextHandles != nullptr) {
		delete m_nextHandles;
	}
	if (m_conversation != nullptr) {
		delete m_conversation;
	}
	if (m_conversationDrawer != nullptr) {
		delete m_conversationDrawer;
	}
}

// 再生
void ChapterEDCommon::play() {
	Movie::play();

	// 会話イベント再生
	if (m_conversation != nullptr && m_cnt < HARUJION_TIME) {
		m_conversation->play();
		m_soundPlayer_p->play();
	}

	if (m_cnt >= HARUJION_TIME - 255 && m_cnt < HARUJION_TIME) {
		// 暗くなっていき、次回予告ムービーへ入る準備
		m_bright--;
	}
	else if (m_cnt >= HARUJION_TIME) {
		// 暗くなっていき、次回予告ムービーが始まる
		m_bright = min(255, m_bright + 2);
	}

	// 次回予告ムービー
	if (m_cnt >= HARUJION_TIME && m_cnt < CHAPTER_TIME) {
		nextMoviePlay();
	}

	// 終了
	if (m_cnt == END_TIME) {
		m_finishFlag = true;
	}
}

// 描画
void ChapterEDCommon::draw() const {

	if (m_conversation != nullptr && m_cnt < HARUJION_TIME) {
		m_conversationDrawer->draw(true);
	}
	else if(m_cnt < CHAPTER_TIME) {
		Movie::draw();
	}

	const int INTERVAL = 50;
	const int DURATION = 350;
	const int xs[5] = {200, 50, 300, 600, 500};
	const int ys[5] = {500, 700, 100, 350, 950};
	if (m_cnt > CHAPTER_TIME) {
		for (int i = 0; i < 5; i++) {
			// 4200~ 4250~ 4300~ 4350~ 4400~ ~4750
			if (m_cnt > CHAPTER_TIME + 100 + (INTERVAL * i) && m_cnt < CHAPTER_TIME + 100 + (INTERVAL * i) + DURATION) {
				if (GetRand(100) < 80) {
					DrawFormatStringToHandle(xs[i] * m_exX, ys[i] * m_exY, GREEN, m_textHandle, m_words[i].c_str());
				}
			}
		}
	}
	if (m_cnt > 4760) {
		if (m_cnt % 4 < 2 || m_cnt > 5000) {
			DrawFormatStringToHandle(100 * m_exX, 450 * m_exY, GREEN, m_textHandle, m_chapterNumStr.c_str());
		}
	}
	if (m_cnt > 5000) {
		DrawFormatStringToHandle(100 * m_exX, 520 * m_exY, GREEN, m_textHandle, m_chapterTitle.c_str());
	}

	drawframe();

	// Zキー長押しでスキップの表示
	drawSkip(m_skipCnt, m_exX, m_exY, m_textHandle);
}


/*
* 2章ED
*/
Chapter2ED::Chapter2ED(SoundPlayer* soundPlayer_p):
	ChapterEDCommon(soundPlayer_p, 2)
{
	m_words[0] = "平気よ。ロボットは人より弱いわ。";
	m_words[1] = "やったー！楽しみにしてるね！！";
	m_words[2] = "そろそろ状況を変えないとな・・・";
	m_words[3] = "オレは任務を遂行するだけだ。";
	m_words[4] = "できるだけ、感情を捨てたらいいんだよ。";
	m_chapterTitle = "黒い過去";

	string path = "picture/movie/chapter2ed/";

	// 最初の画像設定
	m_nextHandles = new GraphHandles((path + "サエル次回").c_str(), 2, m_ex * 3, 0, true);
	m_centerX = GAME_WIDE / 2;
	m_centerY = GAME_HEIGHT / 2;
	m_animation = new Animation(m_centerX, m_centerY, 10, m_nextHandles);

	// 会話
	m_conversation = new Conversation(116, soundPlayer_p, 400);
	m_conversationDrawer = new ConversationDrawer(m_conversation);

	m_kuroeHandle = LoadGraph("picture/event/クロエイト登場.png");
	m_kuroeFaceHandle = LoadGraph("picture/event/クロエイトの顔.png");
	m_kuroeEx = 0.5 * m_exX;

	m_bright = 0;
}

Chapter2ED::~Chapter2ED() {
	DeleteGraph(m_kuroeHandle);
	DeleteGraph(m_kuroeFaceHandle);
}

// 再生
void Chapter2ED::play() {
	if (m_bright < 255 && m_cnt < 300) {
		m_bright++;
	}
	ChapterEDCommon::play();

	if (m_cnt >= 3000 && m_cnt < 3255) {
		m_bright--;
	}
	else if (m_cnt >= 3255 && m_cnt < 3510) {
		m_bright++;
	}
	if (m_cnt == 3255) {
		m_kuroeEx = 0.5 * m_exX;
	}

	if (m_cnt < HARUJION_TIME) {
		m_kuroeEx += 0.0001;
	}
	if (m_cnt >= 3255 && m_cnt <= HARUJION_TIME) {
		m_kuroeEx += 0.0001;
	}
}

// 次回予告ムービー
void Chapter2ED::nextMoviePlay() {
	if (m_cnt == HARUJION_TIME) {
		m_animation->setVy(10);
		m_animation->setLoopFlag(true);
		m_animation->setMovable(true);
	}
	if (m_cnt == 4053) {
		m_animation->setVy(0);
		m_animation->setLoopFlag(false);
	}
}

// 描画
void Chapter2ED::draw() const {
	SetDrawBright(m_bright, m_bright, m_bright);
	if (m_cnt < HARUJION_TIME) {
		DrawRotaGraph(GAME_WIDE / 2, GAME_HEIGHT / 2, m_kuroeEx, 0.0, m_kuroeHandle, TRUE);
	}
	ChapterEDCommon::draw();
	SetDrawBright(m_bright, m_bright, m_bright);
	if (m_cnt >= 3255 && m_cnt <= HARUJION_TIME) {
		DrawRotaGraph(GAME_WIDE / 2, GAME_HEIGHT / 2, m_kuroeEx, 0.0, m_kuroeFaceHandle, TRUE);
	}
	SetDrawBright(255, 255, 255);
}


/*
* 3章ED
*/
Chapter3ED::Chapter3ED(SoundPlayer* soundPlayer_p) :
	ChapterEDCommon(soundPlayer_p, 3)
{
	m_words[0] = "金持ちの嬢ちゃんだろ？大っ嫌いだな。";
	m_words[1] = "全部、集めることができるか？";
	m_words[2] = "んあ、あの手が凄かった男ね。";
	m_words[3] = "まあ、いいんじゃないかな。";
	m_words[4] = "お前を助けに来たんだよ。";
	m_chapterTitle = "テイク・ワン";

	string path = "picture/movie/chapter3ed/";

	// 最初の画像設定
	m_nextHandles = new GraphHandles((path + "クロエイト次回").c_str(), 2, m_ex * 2.5, 0, true);
	m_centerX = GAME_WIDE / 2 - 800 * m_exX;
	m_centerY = GAME_HEIGHT / 2 + 1400 * m_exX;
	m_animation = new Animation(m_centerX, m_centerY, 10, m_nextHandles);

	// 会話
	m_conversation = new Conversation(124, soundPlayer_p, 300);
	m_conversation->setStartCnt(0);
	m_conversationDrawer = new ConversationDrawer(m_conversation);
}

Chapter3ED::~Chapter3ED() {
}

// 再生
void Chapter3ED::play() {

	ChapterEDCommon::play();

}

// 次回予告ムービー
void Chapter3ED::nextMoviePlay() {
	if (m_cnt == HARUJION_TIME) {
		m_animation->setVx(5 * m_exX);
		m_animation->setVy(-5 * m_exX);
		m_animation->setLoopFlag(true);
		m_animation->setMovable(true);
	}
	if (m_cnt >= HARUJION_TIME && m_cnt < 4053) {
		m_animation->setCnt(0);
	}
	if (m_cnt == 4053) {
		m_animation->setVx(0);
		m_animation->setVy(0);
		m_animation->setLoopFlag(false);
	}
}

// 描画
void Chapter3ED::draw() const {
	SetDrawBright(m_bright, m_bright, m_bright);
	ChapterEDCommon::draw();
	SetDrawBright(255, 255, 255);
}


/*
* 4章ED
*/
Chapter4ED::Chapter4ED(SoundPlayer* soundPlayer_p) :
	ChapterEDCommon(soundPlayer_p, 4)
{
	m_words[0] = "・・・とてもそうは思えなかったよ？";
	m_words[1] = "ああ・・・なるほどな。";
	m_words[2] = "ハッ、なめんじゃねーよ。";
	m_words[3] = "私のことはノアと呼んでください。";
	m_words[4] = "機械なんて叩けば直るでしょ。";
	m_chapterTitle = "明日、あなたと、ＮＯＴ　ＯＲ？";

	string path = "picture/movie/chapter4ed/";

	// 最初の画像設定
	m_nextHandles = new GraphHandles((path + "テイク次回").c_str(), 2, m_ex * 2.5, 0, true);
	m_centerX = -2000 * m_exX;
	m_centerY = 100 * m_exX;
	m_animation = new Animation(m_centerX, m_centerY, 10, m_nextHandles);

	// 会話
	m_conversation = new Conversation(132, soundPlayer_p, 320);
	m_conversation->setStartCnt(0);
	m_conversationDrawer = new ConversationDrawer(m_conversation);
}

Chapter4ED::~Chapter4ED() {
}

// 再生
void Chapter4ED::play() {

	ChapterEDCommon::play();

}

// 次回予告ムービー
void Chapter4ED::nextMoviePlay() {
	if (m_cnt == HARUJION_TIME) {
		m_animation->setVx(5 * m_exX);
		m_animation->setVy(3 * m_exX);
		m_animation->setLoopFlag(false);
		m_animation->setMovable(true);
	}
	if (m_cnt >= HARUJION_TIME && m_cnt < HARUJION_TIME + 190) {
		m_animation->setCnt(0);
	}
	if (m_cnt == HARUJION_TIME + 200) {
		m_animation->setVx(55 * m_exX);
		m_animation->setVy(33 * m_exX);
	}
	if (m_cnt == 4050) {
		m_animation->setVx(5 * m_exX);
		m_animation->setVy(3 * m_exX);
		m_animation->setLoopFlag(false);
	}
}

// 描画
void Chapter4ED::draw() const {
	SetDrawBright(m_bright, m_bright, m_bright);
	ChapterEDCommon::draw();
	SetDrawBright(255, 255, 255);
}


/*
* 5章ED
*/
Chapter5ED::Chapter5ED(SoundPlayer* soundPlayer_p) :
	ChapterEDCommon(soundPlayer_p, 5)
{
	m_words[0] = "――それで、嬉しいの？";
	m_words[1] = "ハッ、なんだよ調子狂うぜ。";
	m_words[2] = "救われたことに、違いはない。";
	m_words[3] = "私が治療をします。";
	m_words[4] = "それがバカだって言ってるのよ！！";
	m_chapterTitle = "世界の中心はエリーナ";

	string path = "picture/movie/chapter5ed/";

	// 最初の画像設定
	m_nextHandles = new GraphHandles((path + "ノア次回").c_str(), 3, m_ex * 1.5, 0, true);
	m_centerX = GAME_WIDE / 2;
	m_centerY = GAME_HEIGHT / 2;
	m_animation = new Animation(m_centerX, m_centerY, 30, m_nextHandles);

	// 会話
	m_conversation = new Conversation(140, soundPlayer_p, 187);
	m_conversationDrawer = new ConversationDrawer(m_conversation);

	m_bright = 0;
}

Chapter5ED::~Chapter5ED() {
}

// 再生
void Chapter5ED::play() {

	if (m_bright < 255 && m_cnt < 300) {
		m_bright++;
	}
	ChapterEDCommon::play();

}

// 次回予告ムービー
void Chapter5ED::nextMoviePlay() {
	if (m_cnt == HARUJION_TIME) {
		m_animation->setVy(4);
		m_animation->setLoopFlag(false);
		m_animation->setMovable(true);
	}
	if (m_cnt >= HARUJION_TIME && m_cnt < 4000) {
		m_animation->setCnt(0);
	}
	if (m_cnt == 4000) {
		m_animation->setVy(0);
		m_animation->setLoopFlag(false);
	}
}

// 描画
void Chapter5ED::draw() const {
	SetDrawBright(m_bright, m_bright, m_bright);
	ChapterEDCommon::draw();
	SetDrawBright(255, 255, 255);
}


/*
* 6章ED
*/
Chapter6ED::Chapter6ED(SoundPlayer* soundPlayer_p) :
	ChapterEDCommon(soundPlayer_p, 6)
{
	m_words[0] = "ああ、それは約束する。";
	m_words[1] = "偉そうにすんな！！調子に乗ってんじゃねえよ！！";
	m_words[2] = "それは傲慢だと、言ってて思わないか？";
	m_words[3] = "私に敗北を渡すのは許さないから。";
	m_words[4] = "サエル、君がしたいようにすればいいよ。";
	m_chapterTitle = "タイム・サポーター";

	string path = "picture/movie/chapter6ed/";

	// 最初の画像設定
	m_nextHandles = new GraphHandles((path + "エリーナ次回").c_str(), 2, m_ex * 1.8, 0, true);
	m_centerX = GAME_WIDE;
	m_centerY = GAME_HEIGHT / 2;
	m_animation = new Animation(m_centerX, m_centerY, 5, m_nextHandles);

	// 会話
	m_conversation = new Conversation(148, soundPlayer_p, 310);
	m_conversation->setStartCnt(0);
	m_conversationDrawer = new ConversationDrawer(m_conversation);

	m_bright = 0;
}

Chapter6ED::~Chapter6ED() {
}

// 再生
void Chapter6ED::play() {

	if (m_bright < 255 && m_cnt < 300) {
		m_bright++;
	}
	ChapterEDCommon::play();

}

// 次回予告ムービー
void Chapter6ED::nextMoviePlay() {
	if (m_cnt == HARUJION_TIME) {
		m_animation->setVy(10);
		m_animation->setLoopFlag(false);
		m_animation->setMovable(true);
	}
	if (m_cnt >= HARUJION_TIME && m_cnt < 3900) {
		m_animation->setCnt(0);
	}
	if (m_cnt == 3900) {
		m_animation->setVy(0);
		m_animation->setVx(-16);
		m_animation->setLoopFlag(false);
	}
	if (m_cnt == 4050) {
		m_animation->setVx(0);
	}
}

// 描画
void Chapter6ED::draw() const {
	SetDrawBright(m_bright, m_bright, m_bright);
	ChapterEDCommon::draw();
	SetDrawBright(255, 255, 255);
}
