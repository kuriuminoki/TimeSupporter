#include "Button.h"
#include "Control.h"
#include "Define.h"
#include "TimeSupporter.h"
#include "World.h"
#include "DxLib.h"


#include <sstream>


using namespace std;


SelectStagePage::SelectStagePage(int completeStageSum, int completeExSum, int exp) {
	m_focusChapter = -1;
	m_focusStage = -1;
	m_completeStageSum = completeStageSum;
	m_completeExSum = completeExSum;
	m_exp = exp;

	getGameEx(m_exX, m_exY);
	m_fontSize = (int)(50 * m_exX);
	m_font = CreateFontToHandle(nullptr, m_fontSize, 3);
	m_smallFont = CreateFontToHandle(nullptr, m_fontSize / 2, 3);

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
		m_chapterButton.push_back(new Button(oss.str().c_str(), 100 * m_exX, (100 + (70 * i)) * m_exY, 1200 * m_exX, 60 * m_exY, WHITE, RED, m_font, BLACK));
	}
	for (int i = 0; i < STAGE_PER_CHAPTER; i++) {
		ostringstream oss1;
		oss1 << "ステージ" << i + 1;
		m_stageButton.push_back(new Button(oss1.str().c_str(), 300 * m_exX, (770 + (70 * i)) * m_exY, 480 * m_exX, 60 * m_exY, WHITE, RED, m_font, BLACK));
		ostringstream oss2;
		oss2 << "ステージ" << i + 1 << " (裏)";
		m_typeStageButton.push_back(new Button(oss2.str().c_str(), 800 * m_exX, (770 + (70 * i)) * m_exY, 480 * m_exX, 60 * m_exY, WHITE, RED, m_font, BLUE));
		oss2 << "EX" << i + 1;
		m_hardStageButton.push_back(new Button(oss2.str().c_str(), 120 * m_exX, (770 + (70 * i)) * m_exY, 150 * m_exX, 60 * m_exY, LIGHT_RED, RED, m_font, BLACK));
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
		delete m_typeStageButton[i];
		delete m_hardStageButton[i];
	}
	DeleteFontToHandle(m_font);
	DeleteFontToHandle(m_smallFont);
	for (int i = 0; i < CHAPTER_SUM; i++) {
		DeleteGraph(m_chapterGraphHandles[i]);
	}
}


bool SelectStagePage::play(int handX, int handY) {
	if (m_focusChapter != -1) {
		int dispStageSum = selectableStageSum(m_completeStageSum);
		int dispExSum = m_completeStageSum == 28 ? selectableStageSum(m_completeExSum) : 0;
		for (int i = 0; i < dispStageSum; i++) {
			if (m_stageButton[i]->overlap(handX, handY)) {
				m_focusStage = m_focusChapter * STAGE_PER_CHAPTER + i;
				m_focusKind = STAGE_KIND::NORMAL;
				break;
			}
			else if (m_typeStageButton[i] != nullptr && m_typeStageButton[i]->overlap(handX, handY) && m_focusChapter > 0 && m_focusChapter < 6) {
				m_focusStage = m_focusChapter * STAGE_PER_CHAPTER + i;
				m_focusKind = STAGE_KIND::TYPE;
				break;
			}
			else if (m_hardStageButton[i]->overlap(handX, handY) && i < dispExSum) {
				m_focusStage = m_focusChapter * STAGE_PER_CHAPTER + i;
				m_focusKind = STAGE_KIND::HARD;
				break;
			}
			else {
				m_focusStage = -1;
			}
		}
	}

	if (leftClick() == 1) {

		if (m_focusStage != -1) {
			return true;
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
		DrawRotaGraph(GAME_WIDE - 300 * m_exX + m_chapterGraphDx * m_exX, GAME_HEIGHT - 500 * m_exY, 0.5 * m_exX, 0.0, m_chapterGraphHandles[m_focusChapter], TRUE);
	}

	int distChapterSum = selectableChapterSum();
	for (int i = 0; i < distChapterSum; i++) {
		m_chapterButton[i]->draw(handX, handY);
	}
	if (m_focusChapter != -1) {
		int dispStageSum = selectableStageSum(m_completeStageSum);
		int dispExSum = m_completeStageSum == 28 ? selectableStageSum(m_completeExSum) : 0;
		for (int i = 0; i < dispStageSum; i++) {
			int stage = i + 1 + m_focusChapter * STAGE_PER_CHAPTER;
			ostringstream oss1;
			oss1 << "ステージ" << stage;
			if (stage <= 9) { oss1 << " "; }
			m_stageButton[i]->setString(oss1.str().c_str());
			m_stageButton[i]->draw(handX, handY);
			if (i < dispExSum) {
				ostringstream oss3;
				oss3 << "EX" << stage;
				if (stage <= 9) { oss3 << " "; }
				m_hardStageButton[i]->setString(oss3.str().c_str());
				m_hardStageButton[i]->draw(handX, handY);
			}
			if (m_focusChapter > 0 && m_focusChapter < 6) {
				ostringstream oss2;
				oss2 << "ステージ" << stage;
				if (stage <= 9) { oss2 << " "; }
				oss2 << " (裏)";
				m_typeStageButton[i]->setString(oss2.str().c_str());
				m_typeStageButton[i]->draw(handX, handY);
			}
		}
	}
	if (m_focusStage != -1) {
		DrawBox(0, 600 * m_exY, 1300 * m_exX, 750 * m_exY, GRAY, TRUE);
		if (m_focusKind == STAGE_KIND::NORMAL || m_focusKind == STAGE_KIND::HARD) {
			int now = 0;
			int i = 0;
			string desc = STAGE_SUBTITLES[m_focusStage];
			if (m_focusKind == STAGE_KIND::HARD) {
				desc = "高難易度のモード。" + desc;
			}
			const int size = (int)(desc.size());
			// セリフ
			while (now < size) {
				string disp = desc.substr(now, desc.size() - now);
				size_t br = disp.find("。"); // 改行の記号
				if (br != string::npos) {
					disp = disp.substr(0, (int)br + 2);
				}

				// セリフを描画
				DrawFormatStringToHandle(20 * m_exX, (610 + (i * 30)) * m_exY, WHITE, m_smallFont, disp.c_str());

				// 次の行
				i++;
				now += (int)br + 2;
			}

		}
		else if (m_focusKind == STAGE_KIND::TYPE) {
			DrawFormatStringToHandle(20 * m_exX, 610 * m_exY, WHITE, m_smallFont, "レベルアップ用のステージ。");
		}
	}
	DrawFormatStringToHandle(10 * m_exX, 10 * m_exY, WHITE, m_font, "ステージ選択");
	ostringstream oss1;
	oss1 << "EXP: " << m_exp;
	DrawFormatStringToHandle(800 * m_exX, 10 * m_exY, YELLOW, m_font, oss1.str().c_str());
}


int SelectStagePage::selectableChapterSum() const {
	return min(m_completeStageSum / STAGE_PER_CHAPTER + 1, CHAPTER_SUM);
}

int SelectStagePage::selectableStageSum(int completeStageSum) const {
	if (m_focusChapter == -1) {
		return 0;
	}
	if ((m_focusChapter + 1) * 4 <= completeStageSum) {
		return STAGE_PER_CHAPTER;
	}
	if (m_focusChapter * 4 > completeStageSum) {
		return 0;
	}
	return completeStageSum % STAGE_PER_CHAPTER + 1;
}
