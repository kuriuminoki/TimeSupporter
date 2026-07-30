#include "DxLib.h"
#include "Control.h"
#include "Button.h"
#include "Define.h"

#include <string>
#include <sstream>

using namespace std;


//////////キーボードの入力状態を格納する////////////////

int Key[256]; // キーが押されているフレーム数を格納する

			  // キーの入力状態を更新する
int updateKey() {
	char tmpKey[256]; // 現在のキーの入力状態を格納する
	GetHitKeyStateAll(tmpKey); // 全てのキーの入力状態を得る
	for (int i = 0; i < 256; i++) {
		if (tmpKey[i] != 0) { // i番のキーコードに対応するキーが押されていたら
			Key[i]++;     // 加算
		}
		else {              // 押されていなければ
			Key[i] = 0;   // 0にする
		}
	}
	return 0;
}

void mouseLimit(int& hand_x, const int hand_y) {
	if (hand_x < 0 && hand_y > 0 && hand_y < 480) {
		hand_x = 0;
		SetMousePoint(0, hand_y);//マウスカーソルの位置
	}
	else if (hand_x > 640 && hand_y > 0 && hand_y < 480) {
		hand_x = 640;
		SetMousePoint(640, hand_y);//マウスカーソルの位置
	}
}

///////////マウスの入力状態を返す//////////////
static int left_cnt = 0;
static int right_cnt = 0;
void mouseClick() {
	if ((GetMouseInput() & MOUSE_INPUT_LEFT) != 0) { left_cnt++; }
	else { left_cnt = 0; }
	if ((GetMouseInput() & MOUSE_INPUT_RIGHT) != 0) { right_cnt++; }
	else { right_cnt = 0; }
}
int leftClick() {
	return left_cnt;
}
int rightClick() {
	return right_cnt;
}


// Wキー（上キー）
int controlW() {
	return Key[KEY_INPUT_W];
}

// Sキー（下キー）
int controlS(const KeyConfig* keyConfig) {
	return Key[keyConfig->getSquatKey()];
}

// Aキー（左キー）
int controlA(const KeyConfig* keyConfig) {
	return Key[keyConfig->getLeftMoveKey()];
}

// Dキー（右キー）
int controlD(const KeyConfig* keyConfig) {
	return Key[keyConfig->getRightMoveKey()];
}

// Eキー（キャラチェンジ）
int controlE() {
	return Key[KEY_INPUT_E];
}

// Fキー（スキル発動）
int controlF(const KeyConfig* keyConfig) {
	return Key[keyConfig->getSkillKey()];
}

// Qキー（一時停止）
int controlQ(const KeyConfig* keyConfig) {
	return Key[keyConfig->getPauseKey()];
}

// Zキー（イベントスキップ）
int controlZ(const KeyConfig* keyConfig) {
	return Key[keyConfig->getSkipKey()];
}

//デバッグモード起動用
int controlDebug() {
	if (Key[KEY_INPUT_P] == 1) { // Pキーが押されていたら
		return TRUE;
	}
	return FALSE;
}

//spaceキー
int controlSpace(const KeyConfig* keyConfig)
{
	return Key[keyConfig->getJumpKey()];
}

//左Shiftキー
int controlLeftShift(const KeyConfig* keyConfig)
{
	return Key[keyConfig->getCameraKey()];
}

//右Shiftキー
int controlRightShift()
{
	return Key[KEY_INPUT_RSHIFT];
}

//ゲーム終了用
int controlEsc() {
	if (Key[KEY_INPUT_ESCAPE] == 1) { //ESCキーが1カウント押されていたら
		return TRUE;
	}
	return FALSE;
}


// 何かしらキーを入力されたらそれを返す 入力がないなら-1
int getAnyInputKey() {
	for (int i = 0; i < 256; i++) {
		if (Key[i] == 1) {
			return i;
		}
	}
	return -1;
}


string getKeyStr(int index) {
	if (index == KEY_INPUT_A) { return "Aキー"; }
	if (index == KEY_INPUT_B) { return "Bキー"; }
	if (index == KEY_INPUT_C) { return "Cキー"; }
	if (index == KEY_INPUT_D) { return "Dキー"; }
	if (index == KEY_INPUT_E) { return "Eキー"; }
	if (index == KEY_INPUT_F) { return "Fキー"; }
	if (index == KEY_INPUT_G) { return "Gキー"; }
	if (index == KEY_INPUT_H) { return "Hキー"; }
	if (index == KEY_INPUT_I) { return "Iキー"; }
	if (index == KEY_INPUT_J) { return "Jキー"; }
	if (index == KEY_INPUT_K) { return "Kキー"; }
	if (index == KEY_INPUT_L) { return "Lキー"; }
	if (index == KEY_INPUT_M) { return "Mキー"; }
	if (index == KEY_INPUT_N) { return "Nキー"; }
	if (index == KEY_INPUT_O) { return "Oキー"; }
	if (index == KEY_INPUT_P) { return "Pキー"; }
	if (index == KEY_INPUT_Q) { return "Qキー"; }
	if (index == KEY_INPUT_R) { return "Rキー"; }
	if (index == KEY_INPUT_S) { return "Sキー"; }
	if (index == KEY_INPUT_T) { return "Tキー"; }
	if (index == KEY_INPUT_U) { return "Uキー"; }
	if (index == KEY_INPUT_V) { return "Vキー"; }
	if (index == KEY_INPUT_W) { return "Wキー"; }
	if (index == KEY_INPUT_X) { return "Xキー"; }
	if (index == KEY_INPUT_Y) { return "Yキー"; }
	if (index == KEY_INPUT_Z) { return "Zキー"; }
	if (index == KEY_INPUT_LEFT) { return "←キー"; }
	if (index == KEY_INPUT_UP) { return "↑キー"; }
	if (index == KEY_INPUT_RIGHT) { return "→キー"; }
	if (index == KEY_INPUT_DOWN) { return "↓キー"; }
	if (index == KEY_INPUT_RETURN) { return "ENTERキー"; }
	if (index == KEY_INPUT_SPACE) { return "SPACEキー"; }
	if (index == KEY_INPUT_LSHIFT) { return "SHIFTキー"; }
	if (index == KEY_INPUT_LCONTROL) { return "CONTROLキー"; }
	if (index == KEY_INPUT_LALT) { return "ALTキー"; }
	if (index == KEY_INPUT_1) { return "1キー"; }
	if (index == KEY_INPUT_2) { return "2キー"; }
	if (index == KEY_INPUT_3) { return "3キー"; }
	if (index == KEY_INPUT_4) { return "4キー"; }
	if (index == KEY_INPUT_5) { return "5キー"; }
	if (index == KEY_INPUT_6) { return "6キー"; }
	if (index == KEY_INPUT_7) { return "7キー"; }
	if (index == KEY_INPUT_8) { return "8キー"; }
	if (index == KEY_INPUT_9) { return "9キー"; }
	if (index == KEY_INPUT_0) { return "0キー"; }
	return "";
}


KeyConfig::KeyConfig() {
	init();
	load();

	getGameEx(m_exX, m_exY);
	m_font = CreateFontToHandle(nullptr, (int)(50 * m_exX), 3);
	m_smallFont = CreateFontToHandle(nullptr, (int)(25 * m_exX), 3);
	for (int i = 0; i < NEED_KEY_SUM; i++) {
		ostringstream oss;
		if (i == 5) {
			oss << KEY_NAME[i] << ": 左クリック(長押しで連射) ※変更不可";
		}
		else if (i == 6) {
			oss << KEY_NAME[i] << ": 右クリック ※変更不可";
		}
		else {
			oss << KEY_NAME[i] << ": " << getKeyStr(m_keyConfigs[i]);
		}
		m_buttons[i] = new Button(oss.str(), (int)(700 * m_exX), (int)((70 * i + 200) * m_exY), (int)(700 * m_exX), (int)(50 * m_exY), WHITE, RED, m_smallFont, BLACK);
		if (i == 5 || i == 6) {
			m_buttons[i]->changeFlag(false, LIGHT_BLUE);
		}
		m_buttons[i]->setDispLeft(true);
	}
	m_defaultButton = new Button("操作設定をデフォルトに戻す", (int)(700 * m_exX), (int)(900 * m_exY), (int)(700 * m_exX), (int)(100 * m_exY), LIGHT_RED, RED, m_smallFont, BLACK);
	m_focusConfigIndex = -1;
}


KeyConfig::~KeyConfig() {
	save();
	DeleteFontToHandle(m_font);
	DeleteFontToHandle(m_smallFont);
	for (int i = 0; i < NEED_KEY_SUM; i++) {
		delete m_buttons[i];
	}
	delete m_defaultButton;
}


void KeyConfig::init() {
	m_keyConfigs[0] = KEY_INPUT_A;
	m_keyConfigs[1] = KEY_INPUT_D;
	m_keyConfigs[2] = KEY_INPUT_S;
	m_keyConfigs[3] = KEY_INPUT_F;
	m_keyConfigs[4] = KEY_INPUT_SPACE;
	m_keyConfigs[5] = -1;
	m_keyConfigs[6] = -1;
	m_keyConfigs[7] = KEY_INPUT_LSHIFT;
	m_keyConfigs[8] = KEY_INPUT_Q;
	m_keyConfigs[9] = KEY_INPUT_Z;
}


bool KeyConfig::save() {
	FILE* intFp = nullptr;

	string dir = "savedata/";
	if (fopen_s(&intFp, (dir + CONFIG_PATH).c_str(), "wb") != 0) {
		return false;
	}
	for (int i = 0; i < NEED_KEY_SUM; i++) {
		fwrite(&m_keyConfigs[i], sizeof(m_keyConfigs[i]), 1, intFp);
	}

	fclose(intFp);

	return true;
}


bool KeyConfig::load() {
	FILE* intFp = nullptr;

	string dir = "savedata/";
	if (fopen_s(&intFp, (dir + CONFIG_PATH).c_str(), "rb") != 0) {
		return false;
	}

	for (int i = 0; i < NEED_KEY_SUM; i++) {
		fread(&m_keyConfigs[i], sizeof(m_keyConfigs[i]), 1, intFp);
	}

	fclose(intFp);

	return true;
}


void KeyConfig::play(int mouseX, int mouseY) {
	if (leftClick() == 1) {
		for (int i = 0; i < NEED_KEY_SUM; i++) {
			if (m_buttons[i]->overlap(mouseX, mouseY)) {
				// 割り当て開始
				if (m_focusConfigIndex != -1) {
					m_buttons[m_focusConfigIndex]->changeFlag(true, WHITE);
				}
				m_focusConfigIndex = i;
				m_buttons[m_focusConfigIndex]->changeFlag(false, GRAY);
				ostringstream oss;
				oss << KEY_NAME[m_focusConfigIndex] << ": 割り当てるキーを入力してください。";
				m_buttons[m_focusConfigIndex]->setString(oss.str());
				break;
			}
			if (i == NEED_KEY_SUM - 1 && m_focusConfigIndex != -1) {
				// 何もないところをクリックしたら割り当て中止
				refresh();
			}
			if (m_defaultButton->overlap(mouseX, mouseY)) {
				init();
				refresh();
			}
		}
	}
	if (m_focusConfigIndex != -1) {
		int newIndex = getAnyInputKey();
		if (newIndex != -1) { // 何かが入力されたら、それを割り当てようとする
			for (int i = 0; i < NEED_KEY_SUM; i++) {
				if (m_keyConfigs[i] == newIndex && i != m_focusConfigIndex) {
					ostringstream oss;
					oss << KEY_NAME[m_focusConfigIndex] << ": そのキー使用済みです。";
					m_buttons[m_focusConfigIndex]->setString(oss.str());
					return;
				}
			}
			string keyName = getKeyStr(newIndex);
			if (!keyName.empty()) { // 空文字ならそのキーは使用できない
				m_keyConfigs[m_focusConfigIndex] = newIndex;
				refresh();
			}
			else {
				ostringstream oss;
				oss << KEY_NAME[m_focusConfigIndex] << ": そのキーは使用できません。";
				m_buttons[m_focusConfigIndex]->setString(oss.str());
			}
		}
	}
}


void KeyConfig::draw(int mouseX, int mouseY) {
	for (int i = 0; i < NEED_KEY_SUM; i++) {
		m_buttons[i]->draw(mouseX, mouseY);
	}
	m_defaultButton->draw(mouseX, mouseY);
	DrawStringToHandle((int)(700 * m_exX), (int)(150 * m_exY), "操作設定", WHITE, m_font);
}


const string KeyConfig::getPauseKeyName() const {
	return getKeyStr(m_keyConfigs[8]);
}


const string KeyConfig::getSkipKeyName() const {
	return getKeyStr(m_keyConfigs[9]);
}


int KeyConfig::getUserInput(int key) {
	if (key == MOUSE_INPUT_LEFT) {
		return left_cnt;
	}
	if (key == MOUSE_INPUT_RIGHT) {
		return right_cnt;
	}
	return Key[key];
}


void KeyConfig::refresh() {
	m_focusConfigIndex = -1;
	for (int i = 0; i < NEED_KEY_SUM; i++) {
		if (i == 5 || i == 6) { continue; }
		m_buttons[i]->changeFlag(true, WHITE);
		ostringstream oss;
		oss << KEY_NAME[i] << ": " << getKeyStr(m_keyConfigs[i]);
		m_buttons[i]->setString(oss.str());
	}
}
