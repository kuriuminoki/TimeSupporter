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

	if (!m_conversation->getInitFlag()) {
		return;
	}

	// アニメのみを表示してテキストは表示しないならtrue
	bool animeOnly = false;

	// 座標等の準備
	string text = m_conversation->getText();
	string name = m_conversation->getSpeakerName();
	// フキダシのフチの幅
	const int TEXT_GRAPH_EDGE = (int)(35 * m_exX);
	// 端の余白
	const int EDGE_X = (int)(48 * m_exX);
	const int EDGE_DOWN = (int)(48 * m_exX);
	// 上端
	const int Y1 = GAME_HEIGHT - EDGE_DOWN - (300 * m_exX) - (TEXT_GRAPH_EDGE * 2);

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
		// 背景
		int backGroud = m_conversation->getBackGround();
		if (backGroud != -1 && anime == nullptr) {
			int wide = 0, height = 0;
			double ex = 1.0;
			GetGraphSize(backGroud, &wide, &height);
			ex = max((double)GAME_WIDE / wide, (double)GAME_HEIGHT / height);
			DrawRotaGraph(GAME_WIDE / 2, GAME_HEIGHT / 2, ex, 0.0, backGroud, TRUE);
		}
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

			// キャラの顔画像
			drawCharacter(m_conversation->getSpeakerGraph(), m_conversation->getSpeakerPosition(), 255);
			drawCharacter(m_conversation->getListenerGraph(), m_conversation->getListenerPosition(), 100);

			// 発言者の名前、セリフ顔画像
			int now = 0;
			int i = 0;
			const int CHAR_EDGE = (int)(30 * m_exX);
			// フキダシ
			DrawExtendGraph(EDGE_X + dx, Y1 + dy, GAME_WIDE - EDGE_X + dx, GAME_HEIGHT - EDGE_DOWN + dy, m_frameHandle, TRUE);
			int x = EDGE_X + TEXT_GRAPH_EDGE + dx;
			// 名前
			DrawBox(EDGE_X + dx + 1, Y1 + 1 + dy, GAME_WIDE - EDGE_X - 1 + dx, Y1 + TEXT_GRAPH_EDGE + NAME_SIZE + dy, BLACK, TRUE);
			DrawStringToHandle(x, Y1 + TEXT_GRAPH_EDGE - 10 * m_exY + dy, name.c_str(), WHITE, m_nameHandle);
			if (!m_conversation->getNarrationFlag()) {
				int triangleHeight = 50 * m_exY;
				int triangleWide = 80 * m_exX;
				int positionX = x;
				switch (m_conversation->getSpeakerPosition()) {
				case CHARACTER_POSITION::LEFT:
					break;
				case CHARACTER_POSITION::CENTER:
					positionX += GAME_WIDE / 3;
					break;
				case CHARACTER_POSITION::RIGHT:
					positionX += GAME_WIDE * 3 / 5;
					break;
				}
				DrawTriangle(positionX + dx, Y1 + dy, positionX + triangleWide + dx, Y1 + dy, positionX + (GAME_WIDE / 30) + (triangleWide / 2) + dx, Y1 - triangleHeight + dy, BLACK, TRUE);
			}
			// セリフ
			int height = (int)(TEXT_SIZE * m_exX);
			int textDx = 50 * m_exX;
			drawText(x + textDx, Y1 + TEXT_GRAPH_EDGE + height + dy, height + CHAR_EDGE, text, BLACK, m_textHandle);
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


void ConversationDrawer::drawCharacter(GraphHandle* graph, CHARACTER_POSITION position, int bright) {
	if (graph == nullptr) { return; }
	SetDrawBright(bright, bright, bright);
	int graphWide = 0, graphHeight = 0;
	double ex = 0.5;
	int dy = 200 * ex;
	GetGraphSize(graph->getHandle(), &graphWide, &graphHeight);

	int positionX = 0;
	switch (position) {
	case CHARACTER_POSITION::LEFT:
		positionX = graphWide / 2 * ex;
		graph->setReverseX(false);
		break;
	case CHARACTER_POSITION::CENTER:
		positionX = GAME_WIDE / 2;
		graph->setReverseX(false);
		break;
	case CHARACTER_POSITION::RIGHT:
		positionX = GAME_WIDE - graphWide / 2 * ex;
		graph->setReverseX(true);
		break;
	}

	graph->draw(positionX * m_exX, GAME_HEIGHT - graphHeight / 2 * ex * m_exY + dy, m_exX * ex);
	SetDrawBright(255, 255, 255);
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
