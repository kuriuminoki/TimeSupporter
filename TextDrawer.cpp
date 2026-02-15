#include "TextDrawer.h"
#include "Text.h"
#include "GraphHandle.h"
#include "Animation.h"
#include "AnimationDrawer.h"
#include "Button.h"
#include "DrawTool.h"
#include "Define.h"
#include "DxLib.h"
#include <string>


using namespace std;


ConversationDrawer::ConversationDrawer(Conversation* conversation) {

	m_conversation = conversation;

	m_animationDrawer = new AnimationDrawer(nullptr);

	getGameEx(m_exX, m_exY);

	// フォントデータ
	m_textHandle = CreateFontToHandle(nullptr, (int)(TEXT_SIZE * m_exX), 3);
	m_nameHandle = CreateFontToHandle(nullptr, (int)(NAME_SIZE * m_exX), 5);

	// 吹き出し画像
	m_frameHandle = LoadGraph("picture/textMaterial/frame.png");
}

ConversationDrawer::~ConversationDrawer() {
	delete m_animationDrawer;
	// フォントデータ削除
	DeleteFontToHandle(m_textHandle);
	DeleteFontToHandle(m_nameHandle);
	// 画像データ削除
	DeleteGraph(m_frameHandle);
}

void ConversationDrawer::draw() {

	// アニメのみを表示してテキストは表示しないならtrue
	bool animeOnly = false;

	// 座標等の準備
	string text = m_conversation->getText();
	string name = m_conversation->getSpeakerName();
	GraphHandle* graph = m_conversation->getGraph();
	// キャラの顔画像は正方形を想定
	int graphSize = 0;
	GetGraphSize(graph->getHandle(), &graphSize, &graphSize);
	if (graph != nullptr) {
		graphSize = (int)(graphSize * m_exX);
	}
	// フキダシのフチの幅
	const int TEXT_GRAPH_EDGE = (int)(35 * m_exX);
	// 端の余白
	const int EDGE_X = (int)(48 * m_exX);
	const int EDGE_DOWN = (int)(48 * m_exX);
	// 上端
	const int Y1 = GAME_HEIGHT - EDGE_DOWN - graphSize - (TEXT_GRAPH_EDGE * 2);

	// アニメ
	const Animation* anime = m_conversation->getAnime();
	if (anime != nullptr) {
		int bright = m_conversation->getAnimeBright();
		SetDrawBright(bright, bright, bright);
		int alpha = m_conversation->getAnimeAlpha();
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
		if(alpha == 255){ DrawBox(0, 0, GAME_WIDE, GAME_HEIGHT, BLACK, TRUE); }
		m_animationDrawer->setAnimation(anime);
		m_animationDrawer->drawAnimation();
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0 );
		SetDrawBright(255, 255, 255);
		if (m_conversation->animePlayNow() && m_conversation->getFinishCnt() == 0) { animeOnly = true; }
	}

	// アニメ以外
	if (!animeOnly) {
		// 会話終了時
		if (m_conversation->getFinishCnt() > 0) {
			int finishCnt = (int)(m_conversation->getFinishCnt() * 8 * m_exY);
			if ((Y1 + finishCnt) <= (GAME_HEIGHT - EDGE_DOWN - finishCnt)) { 
				// フキダシ
				DrawExtendGraph(EDGE_X, Y1 + finishCnt, GAME_WIDE - EDGE_X, GAME_HEIGHT - EDGE_DOWN - finishCnt, m_frameHandle, TRUE);
			}
		}
		// 会話開始時
		else if (m_conversation->getStartCnt() > 0) {
			int startCnt = (int)(m_conversation->getStartCnt() * 8 * m_exY);
			if ((Y1 + startCnt) <= (GAME_HEIGHT - EDGE_DOWN - startCnt)) { 
				// フキダシ
				DrawExtendGraph(EDGE_X, Y1 + startCnt, GAME_WIDE - EDGE_X, GAME_HEIGHT - EDGE_DOWN - startCnt, m_frameHandle, TRUE);
			}
		}
		// 会話中
		else {
			int dx = m_conversation->getTextAction().getDx();
			int dy = m_conversation->getTextAction().getDy();

			// フキダシ
			DrawExtendGraph(EDGE_X + dx, Y1 + dy, GAME_WIDE - EDGE_X + dx, GAME_HEIGHT - EDGE_DOWN + dy, m_frameHandle, TRUE);

			// 発言者の名前、セリフ顔画像
			int now = 0;
			int i = 0;
			const int CHAR_EDGE = (int)(30 * m_exX);
			if (m_conversation->getNoFace()) { // 顔画像がない場合
				int x = EDGE_X + TEXT_GRAPH_EDGE + graphSize / 2 + dx;
				// 名前
				DrawStringToHandle(x, GAME_HEIGHT - EDGE_DOWN - (int)(NAME_SIZE * m_exX) - TEXT_GRAPH_EDGE + dy, name.c_str(), BLACK, m_nameHandle);
				// セリフ
				drawText(x, Y1 + TEXT_GRAPH_EDGE + dy, (int)(TEXT_SIZE * m_exX) + CHAR_EDGE, text, BLACK, m_textHandle);
			}
			else { // 顔画像がある場合
				// 名前
				DrawStringToHandle(EDGE_X + TEXT_GRAPH_EDGE + graphSize + (int)(NAME_SIZE * m_exX) + dx, GAME_HEIGHT - EDGE_DOWN - (int)(NAME_SIZE * m_exX) - TEXT_GRAPH_EDGE + dy, name.c_str(), BLACK, m_nameHandle);
				// セリフ
				drawText(EDGE_X + TEXT_GRAPH_EDGE * 2 + graphSize + dx, Y1 + TEXT_GRAPH_EDGE + dy, (int)(TEXT_SIZE * m_exX) + CHAR_EDGE, text, BLACK, m_textHandle);
				// キャラの顔画像
				if (graph != nullptr) {
					graph->draw(EDGE_X + TEXT_GRAPH_EDGE + graphSize / 2 + dx, Y1 + TEXT_GRAPH_EDGE + graphSize / 2 + dy, m_exX);
				}
			}
		}
	}

	// Zキー長押しでスキップの表示
	drawSkip(m_conversation->getSkipCnt(), m_exX, m_exY, m_textHandle);
	
	// 画面右下のクリック要求アイコン
	bool textFinish = m_conversation->finishText() && m_conversation->getFinishCnt() == 0 && m_conversation->nextTextAble();
	bool eventFinish = !(m_conversation->animePlayNow()) || (m_conversation->getEventAnime()->getAnime()->getFinishFlag());
	if (textFinish && eventFinish) {
		int dy = (int)(((m_conversation->getCnt() / 3) % 20 - 10) * m_exY);
		m_conversation->getTextFinishGraph()->draw(GAME_WIDE - EDGE_X - (int)(100 * m_exX), GAME_HEIGHT - EDGE_DOWN - (int)(50 * m_exY) + dy - TEXT_GRAPH_EDGE, m_conversation->getTextFinishGraph()->getEx());
		const Button* yesButton = m_conversation->getYesButton();
		const Button* noButton = m_conversation->getNoButton();
		int mouseX, mouseY;
		GetMousePoint(&mouseX, &mouseY);
		if (yesButton != nullptr) { yesButton->draw(mouseX, mouseY); }
		if (noButton != nullptr) { noButton->draw(mouseX, mouseY); }
	}

	// クリックエフェクト
	const vector<Animation*> animations = m_conversation->getAnimations();
	size_t size = animations.size();
	for (unsigned int i = 0; i < size; i++) {
		// AnimationをDrawerにセット
		m_animationDrawer->setAnimation(animations[i]);

		// カメラを使ってキャラを描画
		m_animationDrawer->drawAnimation();
	}

}

void ConversationDrawer::drawText(int x, int y,int height, const std::string text, int color, int font) {
	int now = 0;
	int i = 0;
	const int size = (int)(text.size());
	// セリフ
	while (now < size) {

		// 次は何文字目まで表示するか
		int next = now + min(MAX_TEXT_LEN, size - now);

		// 次の行の先頭が「、」か「。」ならそれも含める
		if (next - now >= 0 && size >= next + 2) {
			string nextStrHead = text.substr(next, 2);
			if (nextStrHead == "、" || nextStrHead == "。" || nextStrHead == "？" || nextStrHead == "！") {
				next += 2;
			}
		}

		string disp = text.substr(now, next - now);
		size_t br = disp.find("｜"); // 改行の記号
		if (br != string::npos) {
			disp = disp.substr(0, br);
			now += (int)br + 2;
		}
		else {
			now = next;
		}

		// セリフを描画
		if (disp.size() == 0) { continue; }
		DrawStringToHandle(x, y + (i * height), disp.c_str(), BLACK, m_textHandle);

		// 次の行
		i++;
	}
}