#ifndef TITLE_H_INCLUDED
#define TITLE_H_INCLUDED

#include <string>
#include <vector>


class SoundPlayer;
class Button;
class TitleOption;
class Movie;
class AnimationDrawer;
class GameData;
class ControlBar;
class TitleBackGround;


/*
* セーブデータ選択画面
*/
class SelectSaveData {
private:
	
	// セーブデータの数
	static const int GAME_DATA_SUM = 3;
	
	// セーブデータ
	GameData* m_gameData[GAME_DATA_SUM];

	// フォント
	int m_font;

	// セーブデータの選択ボタン
	Button* m_dataButton[GAME_DATA_SUM];
	
	// セーブデータ削除ボタン
	Button* m_dataInitButton[GAME_DATA_SUM];
	int m_initCnt; // 長押しの時間

	// 使用するセーブデータが決まっていないとき
	static const int NOT_DECIDE_DATA = -1;

	// 使用するセーブデータのインデックス
	int m_useSaveDataIndex;

	// 開始するループ番号
	ControlBar* m_startLoop[GAME_DATA_SUM];

	// 背景
	TitleBackGround* m_haikei;

	// チャプター名
	std::vector<std::string> m_chapterNames;

public:

	SelectSaveData(int* screen);

	~SelectSaveData();

	int getSoundVolume();

	// セーブデータが1つでも存在するか
	bool saveDataExist();

	// 最大のLoop
	int getLatestLoop();

	// セーブデータ選択画面の処理
	bool play(int handX, int handY);

	// 描画
	void draw(int handX, int handY);

	// 使用するセーブデータのディレクトリ名
	const char* useDirName();

	// 始めるループ 指定がないなら-1
	int startLoop();

	// 全セーブデータ共通のデータをセーブ(タイトル画面のオプション用)
	void saveCommon(int soundVolume);

};


/*
* タイトル画面
*/
class Title {
private:

	// マウスカーソルの位置
	int m_handX, m_handY;

	// サウンドプレイヤー
	SoundPlayer* m_soundPlayer;

	// タイトルの画像
	int m_titleGraph;

	// オプション画面
	TitleOption* m_option;

	// OPムービー
	Movie* m_movie;

	// セーブデータ選択画面
	SelectSaveData* m_selectSaveData;

	// 今どの画面
	enum TITLE_STATE {
		OP,
		TITLE,
		SELECT,
		OPTION
	};
	TITLE_STATE m_state;

	// ボタン
	int m_font;
	Button* m_selectButton;
	Button* m_optionButton;

	// 戻るボタン
	Button* m_cancelButton;

public:

	Title(int* screen);

	~Title();

	// タイトル画面の処理 継続、再起動、ゲーム開始
	enum TITLE_RESULT {
		CONTINUE,
		REBOOT,
		START
	};
	TITLE_RESULT play();

	// 描画
	void draw();

	// 使用するセーブデータのフォルダ名
	inline const char* useSaveFile() { 
		return m_selectSaveData->useDirName();
	}

	// 始めるループ番号 指定がないなら-1
	inline int startLoop() {
		return m_selectSaveData->startLoop();
	}

};


#endif