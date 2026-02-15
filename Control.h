#ifndef CONTROL_H_INCLUDED
#define CONTROL_H_INCLUDED
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
int controlS();

// Aキー（左キー）
int controlA();

// Dキー（右キー）
int controlD();

// Eキー（キャラチェンジ）
int controlE();

// Fキー（スキル発動）
int controlF();

// Qキー（一時停止）
int controlQ();

// Zキー（イベントスキップ）
int controlZ();

//FPS表示のオンオフ
int controlDebug();

//スペースキー
int controlSpace();

// 左Shiftキー
int controlLeftShift();

// 右Shiftキー
int controlRightShift();

//ESCキー：ゲーム終了
int controlEsc();

#endif