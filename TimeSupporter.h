#ifndef TIME_SUPPORTER_H_INCLUDED
#define TIME_SUPPORTER_H_INCLUDE


#include <vector>


class Button;


class SelectStagePage {
private:
	static const int CHAPTER_SUM = 7;
	static const int STAGE_PER_CHAPTER = 4;
	int m_completeStageSum; // セーブデータと同期させる
	int m_focusChapter; // クリックしてフォーカス中にしているチャプター番号(0~)
	int m_focusStage; // カーソルを合わせているステージ番号(0~)

	// 背景画像
	int m_backgroundGraph;

	// フォント
	int m_font;
	int m_fontSize;

	// 1920を基準としたGAME_WIDEの倍率
	double m_exX;
	// 1080を基準としたGAME_HEIGHTの倍率
	double m_exY;

	std::vector<Button*> m_chapterButton;
	std::vector<Button*> m_stageButton;
public:
	SelectStagePage(int completeStageSum);
	~SelectStagePage();

	bool play(int handX, int handY);

	void draw(int handX, int handY) const;

	inline int getFocusStage() const { return m_focusStage; }

	void setCompleteStageSum(int completeStageSum) { m_completeStageSum = completeStageSum; }

private:
	int selectableChapterSum() const;
	int selectableStageSum() const;
};


# endif