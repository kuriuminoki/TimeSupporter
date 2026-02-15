#include"Button.h"
#include"Control.h"
#include"DxLib.h"
#include<string>
#include<sstream>

using namespace std;

/////ボタン/////////////
Button::Button(string tag, int x, int y, int wide, int height, int color, int color2, int font, int font_color) {
	m_graph_handle = -1;
	m_x = x; m_y = y;
	m_wide = wide; m_height = height;
	m_color = color;
	m_color2 = color2;
	m_font = font;
	m_font_color = font_color;
	setString(tag);
}

//ボタンの描画 下にラベルの文字列も表示できる
void Button::draw(int hand_x, int hand_y) const {
	if (overlap(hand_x, hand_y)) {
		DrawBox(m_x - 5, m_y - 5, m_x + m_wide + 5, m_y + m_height + 5, m_color2, TRUE);
	}
	DrawBox(m_x, m_y, m_x + m_wide, m_y + m_height, m_color, TRUE);
	//絵の描画
	if (!m_flag) { SetDrawBright(100, 100, 100); }
	DrawRotaGraph(m_x + (m_wide / 2), m_y + (m_height / 2), m_graph_ex, 0, m_graph_handle, TRUE, FALSE);
	SetDrawBright(255,255,255);
	//文字の描画
	DrawStringToHandle(m_dx, m_dy, m_tag.c_str(), m_font_color, m_font);
}

//ボタンのオン・オフ
void Button::changeFlag(bool f, int new_color) {
	m_flag = f;
	m_color = new_color;
}

//ボタンがマウスカーソルと重なっているか
bool Button::overlap(int hand_x, int hand_y) const {
	if (!m_flag) {
		return false;
	}
	if (hand_x >= m_x && hand_x <= m_x + m_wide && hand_y >= m_y && hand_y <= m_y + m_height) {
		return true;
	}
	return false;
}

//ボタン内に文字ではなく絵を表示するようにセット
void Button::setGraph(int handle, int ex) {
	m_graph_handle = handle;
	m_graph_ex = ex;
	m_tag = "";
}

//タグをつけなおす
void Button::setString(std::string tag) {
	m_tag = tag;
	//文字列が取る幅を文字の大きさと文字列の長さから計算
	int m_font_size;//一文字の大きさ
	int m_string_size;//文字列がとる幅
	GetFontStateToHandle(nullptr, &m_font_size, nullptr, m_font);
	m_string_size = m_font_size * ((int)m_tag.size() / 2);
	m_dx = m_x + (m_wide - m_string_size) / 2 - ((int)m_tag.size() / 2);
	m_dy = m_y + (m_height - m_font_size) / 2;
}
