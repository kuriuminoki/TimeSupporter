#ifndef GRAPH_HANDLE_H_INCLUDED
#define GRAPH_HANDLE_H_INCLUDED

#include <map>
#include <string>

class Camera;
class CsvReader;

/*
* 画像データ(ハンドル、画像固有の拡大率、向き)をまとめて扱うためのクラス
*/
class GraphHandle {
private:
	// 画像ハンドル
	int m_handle;

	// (この画像固有の)拡大率
	double m_ex;

	// 向き
	double m_angle;

	// 透明度を有効にするか
	bool m_trans;

	// 反転するか
	bool m_reverseX;
	bool m_reverseY;

public:
	GraphHandle(const char* filePath, double ex = 1.0, double angle = 0.0, bool trans = false, bool reverseX = false, bool reverseY = false);
	~GraphHandle();

	// ゲッタ
	inline int getHandle() const { return m_handle; }
	inline double getEx() const { return m_ex; }
	inline double getAngle() const { return m_angle; }

	// セッタ
	inline void setEx(double ex) { m_ex = ex; }
	inline void setAngle(double angle) { m_angle = angle; }
	inline void setTrans(bool trans) { m_trans = trans; }
	inline void setReverseX(bool reverse) { m_reverseX = reverse; }
	inline void setReverseY(bool reverse) { m_reverseY = reverse; }

	// 描画する
	void draw(int x, int y, double ex = 1.0) const;

	// 範囲を指定して変形描画する
	void extendDraw(int x1, int y1, int x2, int y2) const;

	// 範囲を指定して敷き詰めるように描画する
	void lineUpDraw(int x1, int y1, int x2, int y2, const Camera* camera) const;
};


/*
* GraphHandleを配列としてまとめて扱うクラス
*/
class GraphHandles {
private:
	GraphHandle** m_handles;
	int m_handleSum;

public:
	//  filePathの末尾には".png"をつけないこと。
	GraphHandles(const char* filePath, int handleSum, double ex = 1.0, double angle = 0.0, bool trans = false, bool reverseX = false, bool reverseY = false);
	~GraphHandles();

	// ゲッタ
	inline GraphHandle* getGraphHandle(int i = 0) const { return m_handles[i]; }
	inline int getHandle(int i) { return m_handles[i]->getHandle(); }
	inline int getSize() const { return m_handleSum; }

	// セッタ
	void setEx(double ex);
	void setAngle(double angle);
	void setTrans(bool trans);
	void setReverseX(bool reverse);
	void setReverseY(bool reverse);

	// 描画する
	void draw(int x, int y, int index);
};


/*
* 当たり判定の情報
*/
class AtariArea {
private:
	// 当たり判定
	bool m_defaultWide, m_defaultHeight;
	int m_wide, m_height;
	int m_x1, m_y1, m_x2, m_y2;
	bool m_x1none, m_y1none, m_x2none, m_y2none;

public:
	AtariArea(CsvReader* csvReader, const char* graphName, const char* prefix);

	bool getDefaultWide() const { return m_defaultWide; }
	bool getDefaultHeight() const { return m_defaultHeight; }
	bool getWide() const { return m_wide; }
	bool getHeight() const { return m_height; }
	bool getX1() const { return m_x1; }
	bool getY1() const { return m_y1; }
	bool getX2() const { return m_x2; }
	bool getY2() const { return m_y2; }

	void getArea(int* x1, int* y1, int* x2, int* y2, int wide, int height) const;

};


/*
* 当たり判定の情報付きのGraphHandles
*/
class GraphHandlesWithAtari {
private:

	GraphHandles* m_graphHandles;

	// 当たり判定
	AtariArea* m_atariArea;
	AtariArea* m_damageArea;

public:

	GraphHandlesWithAtari(GraphHandles* graphHandles, const char* graphName, CsvReader* csvReader);
	~GraphHandlesWithAtari();

	GraphHandles* getGraphHandles() const { return m_graphHandles; }

	void getAtari(int* x1, int* y1, int* x2, int* y2, int index) const;
	void getDamage(int* x1, int* y1, int* x2, int* y2, int index) const;

};


/*
* キャラの眼の瞬きを処理するクラス
*/
class CharacterEyeClose {
private:

	// 眼を閉じる時間
	static const int EYE_CLOSE_MIN_TIME = 5;
	static const int EYE_CLOSE_MAX_TIME = 10;
	
	// 眼を閉じる残り時間
	int m_cnt;

public:

	CharacterEyeClose();

	// 瞬きスタートでtrue
	bool closeFlag();

private:

	// 眼を閉じる時間をカウント
	void count();

	// 瞬き中ならtrue
	inline bool closeNow() { return m_cnt > 0; }

};


/*
* キャラの画像を管理するクラス
*/
class CharacterGraphHandle {
private:
	// 表示される画像
	GraphHandlesWithAtari* m_dispGraphHandle_p;
	int m_dispGraphIndex;

	double m_ex;

	int m_wide, m_height;

	// 当たり判定
	int m_atariX1, m_atariY1, m_atariX2, m_atariY2;

	// 瞬き
	CharacterEyeClose m_characterEyeClose;

	// キャラのパーツの画像
	// 斬撃攻撃画像
	GraphHandlesWithAtari* m_slashHandles;

	// 空中斬撃攻撃画像
	GraphHandlesWithAtari* m_airSlashEffectHandles;

	// 射撃攻撃画像
	GraphHandlesWithAtari* m_bulletHandles;

	// キャラ本体の画像
	// 立ち画像
	GraphHandlesWithAtari* m_standHandles;

	// しゃがみ画像
	GraphHandlesWithAtari* m_squatHandles;

	// しゃがみ射撃画像
	GraphHandlesWithAtari* m_squatBulletHandles;

	// 立ち射撃画像
	GraphHandlesWithAtari* m_standBulletHandles;

	// 立ち斬撃画像
	GraphHandlesWithAtari* m_standSlashHandles;

	// 走り画像
	GraphHandlesWithAtari* m_runHandles;

	// 走り射撃画像
	GraphHandlesWithAtari* m_runBulletHandles;

	// 着地画像
	GraphHandlesWithAtari* m_landHandles;

	// 上昇画像
	GraphHandlesWithAtari* m_jumpHandles;

	// 下降画像
	GraphHandlesWithAtari* m_downHandles;

	// ジャンプ前画像
	GraphHandlesWithAtari* m_preJumpHandles;

	// ダメージ画像
	GraphHandlesWithAtari* m_damageHandles;

	// ブースト画像
	GraphHandlesWithAtari* m_boostHandles;

	// 空中射撃画像
	GraphHandlesWithAtari* m_airBulletHandles;

	// 空中斬撃画像
	GraphHandlesWithAtari* m_airSlashHandles;

	// ステップ
	GraphHandlesWithAtari* m_stepHandles;

	// スライディング
	GraphHandlesWithAtari* m_slidingHandles;

	// 瞬き画像
	GraphHandlesWithAtari* m_closeHandles;

	// やられ画像
	GraphHandlesWithAtari* m_deadHandles;

	// ボスの初期アニメーション
	GraphHandlesWithAtari* m_initHandles;

	// 追加画像
	GraphHandlesWithAtari* m_special1Handles;

public:
	// デフォルト値で初期化
	CharacterGraphHandle();
	// csvファイルを読み込み、キャラ名で検索しパラメータ取得
	CharacterGraphHandle(const char* characterName, double drawEx);
	// 画像を削除
	~CharacterGraphHandle();

	// 表示する画像を返す
	inline GraphHandlesWithAtari* getDispGraphHandle() { return m_dispGraphHandle_p; }
	inline GraphHandle* getHandle() { return m_dispGraphHandle_p->getGraphHandles()->getGraphHandle(m_dispGraphIndex); }
	inline int getDispGraphIndex() const { return m_dispGraphIndex; }
	inline int getWide() const { return m_wide; }
	inline int getHeight() const { return m_height; }
	void getAtari(int* x1, int* y1, int* x2, int* y2) const;
	void getDamage(int* x1, int* y1, int* x2, int* y2) const;

	// 画像のゲッタ
	inline GraphHandlesWithAtari* getSlashHandle() { return m_slashHandles; }
	inline GraphHandlesWithAtari* getAirSlashEffectHandle() { return m_airSlashEffectHandles; }
	inline GraphHandlesWithAtari* getBulletHandle() { return m_bulletHandles; }
	inline GraphHandlesWithAtari* getStandHandle() { return m_standHandles; }
	inline GraphHandlesWithAtari* getStandBulletHandle() { return m_standBulletHandles; }
	inline GraphHandlesWithAtari* getStandSlashHandle() { return m_standSlashHandles; }
	inline GraphHandlesWithAtari* getSquatHandle() { return m_squatHandles; }
	inline GraphHandlesWithAtari* getSquatBulletHandle() { return m_squatBulletHandles; }
	inline GraphHandlesWithAtari* getRunHandle() { return m_runHandles; }
	inline GraphHandlesWithAtari* getRunBulletHandle() { return m_runBulletHandles; }
	inline GraphHandlesWithAtari* getLandHandle() { return m_landHandles; }
	inline GraphHandlesWithAtari* getJumpHandle() { return m_jumpHandles; }
	inline GraphHandlesWithAtari* getDownHandle() { return m_downHandles; }
	inline GraphHandlesWithAtari* getPreJumpHandle() { return m_preJumpHandles; }
	inline GraphHandlesWithAtari* getDamageHandle() { return m_damageHandles; }
	inline GraphHandlesWithAtari* getBoostHandle() { return m_boostHandles; }
	inline GraphHandlesWithAtari* getAirBulletHandle() { return m_airBulletHandles; }
	inline GraphHandlesWithAtari* getAirSlashHandle() { return m_airSlashHandles; }
	inline GraphHandlesWithAtari* getStepHandle() { return m_stepHandles; }
	inline GraphHandlesWithAtari* getSlidingHandle() { return m_slidingHandles; }
	inline GraphHandlesWithAtari* getCloseHandle() { return m_closeHandles; }
	inline GraphHandlesWithAtari* getDeadHandle() { return m_deadHandles; }
	inline GraphHandlesWithAtari* getInitHandle() { return m_initHandles; }
	inline GraphHandlesWithAtari* getSpecial1Handle() { return m_special1Handles; }

	// 画像サイズをセット
	void setGraphSize();

	// 当たり判定をセット
	void setAtari();

	// 画像をセット、存在しない画像ならそのまま　サイズも決定
	void setGraph(GraphHandlesWithAtari* graphHandles, int index);

	// 立ち画像をセット
	void switchStand(int index = 0);
	// 立ち射撃画像をセット
	void switchBullet(int index = 0);
	// 立ち斬撃画像をセット
	void switchSlash(int index = 0);
	// しゃがみ画像をセット
	void switchSquat(int index = 0);
	// しゃがみ射撃画像をセット
	void switchSquatBullet(int index = 0);
	// 走り画像をセット
	void switchRun(int index = 0);
	// 走り射撃画像をセット
	void switchRunBullet(int index = 0);
	// 着地画像をセット
	void switchLand(int index = 0);
	// 上昇画像をセット
	void switchJump(int index = 0);
	// 降下画像をセット
	void switchDown(int index = 0);
	// ジャンプ前画像をセット
	void switchPreJump(int index = 0);
	// ダメージ画像をセット
	void switchDamage(int index = 0);
	// ブースト画像をセット
	void switchBoost(int index = 0);
	// 空中射撃画像をセット
	void switchAirBullet(int index = 0);
	// 空中斬撃画像をセット
	void switchAirSlash(int index = 0);
	// ステップ画像をセット
	void switchStep(int index = 0);
	// スライディング画像をセット
	void switchSliding(int index = 0);
	// 瞬き画像をセット
	void switchClose(int index = 0);
	// やられ画像をセット
	void switchDead(int index = 0);
	// ボスの初期アニメーションをセット
	void switchInit(int index = 0);
	// 追加画像をセット
	void switchSpecial1(int index = 0);
};


/*
* キャラの顔画像
*/
class FaceGraphHandle {
private:
	double m_ex;

	// 顔画像 <画像名, 画像ハンドル>
	std::map<std::string, GraphHandles*> m_faceHandles;

public:
	FaceGraphHandle();
	FaceGraphHandle(const char* characterName, double drawEx);
	~FaceGraphHandle();

	GraphHandles* getGraphHandle(const char* faceName);
};


#endif