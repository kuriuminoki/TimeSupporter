#ifndef ANIMATION_H_INCLUDED
#define ANIMATION_H_INCLUDED

#include <queue>
#include <string>
#include <vector>

class GraphHandle;
class GraphHandles;
class SoundPlayer;
class AnimationDrawer;
class Character;
class Conversation;
class ConversationDrawer;

class Animation {
private:
	// 画像ハンドル deleteはこのクラスでしない
	GraphHandles* m_handles_p;

	// 座標
	int m_x, m_y;

	// 速度
	int m_vx, m_vy;

	// 速度を使う
	bool m_movable;

	// 座標を追うキャラ（m_x, m_yを使うなら設定不要）
	const Character* m_character_p;
	int m_characterId;
	
	// カウント
	int m_cnt;

	// 終了までのカウント
	int m_finishCnt;

	// 画像１枚の表示時間
	int m_frameCnt;

	// アニメーションが終了
	bool m_finishFlag;

	// ループ再生
	bool m_loopFlag;

public:
	Animation(int x, int y, int frameCnt, GraphHandles* graphHandles);

	// ゲッタ
	inline int getX() const { return m_x; }
	inline int getY() const { return m_y; }
	inline int getVx() const { return m_vx; }
	inline int getVy() const { return m_vy; }
	inline const Character* getCharacter() const { return m_character_p; }
	inline int getCharacterId() const { return m_characterId; }
	inline bool getFinishFlag() const { return m_finishFlag; }
	inline int getCnt() const { return m_cnt; }

	// セッタ
	inline void setX(int x) { m_x = x; }
	inline void setY(int y) { m_y = y; }
	inline void setVx(int vx) { m_vx = vx; }
	inline void setVy(int vy) { m_vy = vy; }
	void setCharacter(Character* character);
	inline void setMovable(bool movable) { m_movable = movable; }
	inline void setCnt(int cnt) { m_cnt = cnt; }
	inline void setFinishCnt(int finishCnt) { m_finishCnt = finishCnt; }
	inline void setFinishFlag(bool finishFlag) { m_finishFlag = finishFlag; }
	inline void setLoopFlag(bool loopFlag) { m_loopFlag = loopFlag; }

	// 初期化
	void init();

	// アニメーションの切り替え
	void changeGraph(GraphHandles* nextGraph, int frameCnt = -1);

	// カウント
	void count();

	// 描画用
	GraphHandle* getHandle() const;

	// 今何枚目か
	int getAnimeNum() const { return m_cnt / m_frameCnt; }
};


// 動画の基底クラス
class Movie {
protected:

	std::string OWNER_NAME;

	// 解像度の変更に対応
	double m_ex;
	// テキストやフォントのサイズの倍率
	double m_exX;
	double m_exY;

	// フォント（テキスト）
	int m_textHandle;
	const int TEXT_SIZE = 50;

	int m_frameWide, m_frameHeight;

	// 終了したらtrue
	bool m_finishFlag;

	// Zキーの長押し時間
	int m_skipCnt;

	// 開始からの経過時間
	int m_cnt;

	// 画像を入れて動かすメイン画像
	int m_centerX, m_centerY;
	Animation* m_animation;
	AnimationDrawer* m_animationDrawer;

	// サウンドプレイヤー
	SoundPlayer* m_soundPlayer_p;

	// BGMのパス
	std::string m_bgmPath;

	// もともと流していたBGM
	std::string m_originalBgmPath;

public:
	Movie(SoundPlayer* soundPlayer_p);
	virtual ~Movie();

	// ゲッタ
	inline bool getFinishFlag() const { return m_finishFlag; }
	inline bool getSkipCnt() const { return m_skipCnt; }
	inline Animation* getAnimation() const { return m_animation; }
	inline int getCnt() const { return m_cnt; }

	// 再生
	virtual void play();

	// 描画
	virtual void draw() const;

protected:
	void drawframe() const;
};


// 1章のED
class ChapterOneED:
	public Movie
{
private:

	int m_bright;
	bool m_toDark;

	int m_nowHaikei;

	bool m_elinaFlag;

	bool m_saeruFlag;

	// 画像
	GraphHandles* m_walkSaeru;
	int m_haikei[4];
	std::string m_credit[7];
	int m_elina;
	int m_deadSaeru;

public:
	ChapterOneED(SoundPlayer* soundPlayer_p);
	~ChapterOneED();

	// 再生
	void play();

	// 描画
	void draw() const;
};


// 各章のED (共通部分)
class ChapterEDCommon :
	public Movie
{
protected:

	Conversation* m_conversation;
	ConversationDrawer* m_conversationDrawer;

	// 画像
	GraphHandles* m_nextHandles;

	// 文字列
	std::string m_words[5];
	std::string m_chapterTitle;
	std::string m_chapterNumStr;

	int m_bright;

	const int HARUJION_TIME = 3770;
	const int CHAPTER_TIME = 4100;
	const int END_TIME = 5400;

public:
	ChapterEDCommon(SoundPlayer* soundPlayer_p, int chapterNum);
	~ChapterEDCommon();

	// 再生
	void play();

	// 描画
	void draw() const;

	virtual void nextMoviePlay() = 0;
};


// 2章のED
class Chapter2ED :
	public ChapterEDCommon
{
private:
	int m_kuroeHandle;
	int m_kuroeFaceHandle;
	double m_kuroeEx;

public:
	Chapter2ED(SoundPlayer* soundPlayer_p);
	~Chapter2ED();

	// 再生
	void play();

	void nextMoviePlay();

	// 描画
	void draw() const;
};


// 3章のED
class Chapter3ED :
	public ChapterEDCommon
{
private:

public:
	Chapter3ED(SoundPlayer* soundPlayer_p);
	~Chapter3ED();

	// 再生
	void play();

	void nextMoviePlay();

	// 描画
	void draw() const;
};


// 4章のED
class Chapter4ED :
	public ChapterEDCommon
{
private:

public:
	Chapter4ED(SoundPlayer* soundPlayer_p);
	~Chapter4ED();

	// 再生
	void play();

	void nextMoviePlay();

	// 描画
	void draw() const;
};


// 5章のED
class Chapter5ED :
	public ChapterEDCommon
{
private:

public:
	Chapter5ED(SoundPlayer* soundPlayer_p);
	~Chapter5ED();

	// 再生
	void play();

	void nextMoviePlay();

	// 描画
	void draw() const;
};


// 6章のED
class Chapter6ED :
	public ChapterEDCommon
{
private:

public:
	Chapter6ED(SoundPlayer* soundPlayer_p);
	~Chapter6ED();

	// 再生
	void play();

	void nextMoviePlay();

	// 描画
	void draw() const;
};


#endif