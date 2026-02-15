#ifndef INCLUDED_BUTTON_H
#define INCLUDED_BUTTON_H


#include<string>


class Button {
private:

	bool m_flag = true;//機能しないボタンはfalse

	std::string m_tag = ""; // 表示するテキスト

	int m_x, m_y; //ボタンの位置

	int m_wide, m_height; // 四角の高さと幅

	int m_color; //四角の中の色

	int m_color2; //マウスが重なっているときに使う色

	int m_font_color; //文字の色

	int m_font; // テキストのフォント

	int m_dx, m_dy; //文字を表示する座標

	int m_graph_handle = -1; //絵

	int m_graph_ex; //絵の拡大率

public:

	// コンストラクタ
	Button(std::string, int x, int y, int wide, int height, int color, int color2, int font, int font_color);

	// ゲッタ
	inline bool getFlag() const { return m_flag; }
	inline int getHandle() const { return m_graph_handle; } //画像を取得
	inline int getWide() const { return m_wide; }
	inline int getHeight() const { return m_height; }

	// セッタ
	void setGraph(int handle, int ex);
	void setString(std::string new_string);//タグをつけなおす
	inline void setX(int x) { m_x = x; }
	inline void setColor(int colorHandle) { m_color = colorHandle; }

	// ボタンのon/off切り替え
	void changeFlag(bool f, int new_color);

	// マウスが重なっているか確認
	bool overlap(int hand_x, int hand_y) const;

	// 描画
	void draw(int hand_x, int hand_y) const;
};


#endif