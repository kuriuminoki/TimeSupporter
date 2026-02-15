#include "Button.h"
#include "Control.h"
#include "Define.h"
#include "TimeSupporter.h"
#include "DxLib.h"


#include <sstream>


using namespace std;


SelectStagePage::SelectStagePage(int completeStageSum) {
	m_focusChapter = -1;
	m_focusStage = -1;
	m_completeStageSum = completeStageSum;

	getGameEx(m_exX, m_exY);
	m_fontSize = (int)(50 * m_exX);
	m_font = CreateFontToHandle(nullptr, m_fontSize, 3);

	m_chapterGraphDx = CHAPTER_GRAPH_MAX_DX;

	const char* CHAPTER_TITLE[7] = {
		"サポーティング　　　　　　　　",
		"量産型クズ　　　　　　　　　　",
		"黒い過去　　　　　　　　　　　",
		"テイク・ワン　　　　　　　　　",
		"明日、あなたと、ＮＯＴ　ＯＲ？",
		"世界の中心はエリーナ　　　　　",
		"タイム・サポーター　　　　　　"
	};
	for (int i = 0; i < CHAPTER_SUM; i++) {
		ostringstream oss;
		oss << "チャプター" << i + 1 << "：" << CHAPTER_TITLE[i];
		m_chapterButton.push_back(new Button(oss.str().c_str(), 100, 100 + (70 * i), 1200, 60, WHITE, RED, m_font, BLACK));
	}
	for (int i = 0; i < STAGE_PER_CHAPTER; i++) {
		ostringstream oss;
		oss << "ステージ" << i + 1;
		m_stageButton.push_back(new Button(oss.str().c_str(), 200, 700 + (70 * i), 500, 60, WHITE, RED, m_font, BLACK));
	}
	for (int i = 0; i < CHAPTER_SUM; i++) {
		ostringstream oss;
		oss << "picture/system/chapter" << i + 1 << ".png";
		m_chapterGraphHandles.push_back(LoadGraph(oss.str().c_str()));
	}
}


SelectStagePage::~SelectStagePage() {
	for (int i = 0; i < CHAPTER_SUM; i++) {
		delete m_chapterButton[i];
	}
	for (int i = 0; i < STAGE_PER_CHAPTER; i++) {
		delete m_stageButton[i];
	}
	DeleteFontToHandle(m_font);
	for (int i = 0; i < CHAPTER_SUM; i++) {
		DeleteGraph(m_chapterGraphHandles[i]);
	}
}


bool SelectStagePage::play(int handX, int handY) {
	m_focusStage = -1;

	if (leftClick() == 1) {

		if (m_focusChapter != -1) {
			int dispStageSum = selectableStageSum();
			for (int i = 0; i < dispStageSum; i++) {
				if (m_stageButton[i]->overlap(handX, handY)) {
					m_focusStage = m_focusChapter * STAGE_PER_CHAPTER + i;
					return true;
				}
			}
		}

		int distChapterSum = selectableChapterSum();
		bool clickAnyButtonFlag = false;
		for (int i = 0; i < distChapterSum; i++) {
			if (m_chapterButton[i]->overlap(handX, handY)) {
				if (m_focusChapter != -1) {
					m_chapterButton[m_focusChapter]->changeFlag(true, WHITE);
				}
				m_focusChapter = i;
				m_chapterButton[i]->changeFlag(false, GRAY);
				m_chapterGraphDx = CHAPTER_GRAPH_MAX_DX;
				clickAnyButtonFlag = true;
				break;
			}
		}
		if (!clickAnyButtonFlag && m_focusChapter != -1) {
			// 何もないところをクリックした場合チャプターフォーカスをやめる
			m_chapterButton[m_focusChapter]->changeFlag(true, WHITE);
			m_focusChapter = -1;
			m_chapterGraphDx = CHAPTER_GRAPH_MAX_DX;
		}

	}

	if (m_focusChapter != -1) {
		m_chapterGraphDx = max(0, m_chapterGraphDx / 2);
	}

	return false;
}

void SelectStagePage::draw(int handX, int handY) const {

	if (m_focusChapter != -1) {
		DrawRotaGraph(GAME_WIDE - 300 + m_chapterGraphDx, GAME_HEIGHT - 500, 0.5 * m_exX, 0.0, m_chapterGraphHandles[m_focusChapter], TRUE);
	}

	int distChapterSum = selectableChapterSum();
	for (int i = 0; i < distChapterSum; i++) {
		m_chapterButton[i]->draw(handX, handY);
	}
	if (m_focusChapter != -1) {
		int dispStageSum = selectableStageSum();
		for (int i = 0; i < dispStageSum; i++) {
			m_stageButton[i]->draw(handX, handY);
		}
	}
	DrawFormatStringToHandle(10, 10, WHITE, m_font, "ステージ選択");
}


int SelectStagePage::selectableChapterSum() const {
	return min(m_completeStageSum / STAGE_PER_CHAPTER + 1, CHAPTER_SUM);
}

int SelectStagePage::selectableStageSum() const {
	if (m_focusChapter == -1) {
		return 0;
	}
	if (m_focusChapter * 4 <= m_completeStageSum) {
		return STAGE_PER_CHAPTER;
	}
	return m_completeStageSum % STAGE_PER_CHAPTER + 1;
}
