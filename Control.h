#ifndef CONTROL_H_INCLUDED
#define CONTROL_H_INCLUDED


class Button;

#include <string>

/*
* キーコンフィグ
*/
class KeyConfig {
private:
	// セーブデータのファイル名
	const char* CONFIG_PATH = "keyConfig.dat";

	// キーの種類数
	static const int NEED_KEY_SUM = 10;

	// 設定画面用
	double m_exX, m_exY;
	int m_font;
	int m_smallFont;
	Button* m_buttons[NEED_KEY_SUM];
	Button* m_defaultButton;
	int m_focusConfigIndex; // 今編集しているコンフィグのインデックス

	//// セーブデータ ////
	// 各キーの修正後の番号
	const char* KEY_NAME[NEED_KEY_SUM] = {
		"左移動",
		"右移動",
		"しゃがみ",
		"必殺技",
		"ジャンプ",
		"射撃",
		"斬撃",
		"カメラ切り替え",
		"ポーズ",
		"スキップ"
	};
	int m_keyConfigs[NEED_KEY_SUM];

public:
	KeyConfig();
	~KeyConfig();

	void init(); // キーコンフィグをデフォルトにする (m_useMouseは除く)

	bool save();
	bool load();

	void play(int mouseX, int mouseY); // 設定画面処理
	void draw(int mouseX, int mouseY); // 設定画面描画

	// ゲッタ
	inline int getLeftMoveKey() const { return m_keyConfigs[0]; }
	inline int getRightMoveKey() const { return m_keyConfigs[1]; }
	inline int getSquatKey() const { return m_keyConfigs[2]; }
	inline int getSkillKey() const { return m_keyConfigs[3]; }
	inline int getJumpKey() const { return m_keyConfigs[4]; }
	inline int getBulletKey() const { return m_keyConfigs[5]; }
	inline int getSlashKey() const { return m_keyConfigs[6]; }
	inline int getCameraKey() const { return m_keyConfigs[7]; }
	inline int getPauseKey() const { return m_keyConfigs[8]; }
	inline int getSkipKey() const { return m_keyConfigs[9]; }

	// 一時停止キーのキー名を取得
	const std::string getPauseKeyName() const;
	const std::string getSkipKeyName() const;

	// keyの入力状態を返す
	int getUserInput(int key);

	void refresh();

};


//////キーボードからの入力を格納し、返す関数群///////////

int updateKey();

void mouseLimit(int& hand_x, const int hand_y);
//左クリックの状態
void mouseClick();
int leftClick();
int rightClick();

// Wキー（上キー）
int controlW();

// Sキー（下キー）
int controlS(const KeyConfig* keyConfig);

// Aキー（左キー）
int controlA(const KeyConfig* keyConfig);

// Dキー（右キー）
int controlD(const KeyConfig* keyConfig);

// Eキー（キャラチェンジ）
int controlE();

// Fキー（スキル発動）
int controlF(const KeyConfig* keyConfig);

// Qキー（一時停止）
int controlQ(const KeyConfig* keyConfig);

// Zキー（イベントスキップ）
int controlZ(const KeyConfig* keyConfig);

//FPS表示のオンオフ
int controlDebug();

//スペースキー
int controlSpace(const KeyConfig* keyConfig);

// 左Shiftキー
int controlLeftShift(const KeyConfig* keyConfig);

// 右Shiftキー
int controlRightShift();

//ESCキー：ゲーム終了
int controlEsc();


#endif