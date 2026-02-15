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
}


SelectStagePage::~SelectStagePage() {
	for (int i = 0; i < CHAPTER_SUM; i++) {
		delete m_chapterButton[i];
	}
	for (int i = 0; i < STAGE_PER_CHAPTER; i++) {
		delete m_stageButton[i];
	}
	DeleteFontToHandle(m_font);
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
		for (int i = 0; i < distChapterSum; i++) {
			if (m_chapterButton[i]->overlap(handX, handY)) {
				m_focusChapter = i;
				break;
			}
			// 何もないところをクリックした場合チャプターフォーカスをやめる
			m_focusChapter = -1;
		}

	}

	return false;
}

void SelectStagePage::draw(int handX, int handY) const {
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
