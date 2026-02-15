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

	Animation* createCopy();

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
	virtual void draw();

protected:
	void drawframe();
};


// オープニング用
class PartOneCharacter {
private:

	// 画像
	GraphHandle* m_character;

	// 座標
	int m_initX, m_initY;
	int m_x, m_y;

	// 速度
	int m_vx, m_vy;

	double m_ex;

public:
	PartOneCharacter(GraphHandle* character, int initX, int initY, int vx, double ex);

	void play();
	void draw();
};


// オープニング
class OpMovie:
	public Movie
{
private:

	// 画像
	// タイトル
	GraphHandles* m_titleH;
	GraphHandles* m_title;
	GraphHandles* m_titleChara;
	GraphHandles* m_titleBlue;
	GraphHandles* m_titleOrange;
	GraphHandles* m_titleHeart;
	GraphHandles* m_heartHide;
	GraphHandles* m_heartHide2;

	// part1
	GraphHandles* m_darkHeart;
	GraphHandles* m_heartEye;
	std::vector<PartOneCharacter*> m_partOneCharacters;
	GraphHandle* m_archive1;
	GraphHandle* m_aigis1;
	GraphHandle* m_assault1;
	GraphHandle* m_vermelia1;
	GraphHandle* m_exlucina1;
	GraphHandle* m_msadi1;
	GraphHandle* m_elnino1;
	GraphHandle* m_onyx1;
	GraphHandle* m_courir1;
	GraphHandle* m_cornein1;
	GraphHandle* m_koharu1;
	GraphHandle* m_siesta1;
	GraphHandle* m_hierarchy1;
	GraphHandle* m_troy1;
	GraphHandle* m_ancient1;
	GraphHandle* m_valkiria1;
	GraphHandle* m_chocola1;
	GraphHandle* m_titius1;
	GraphHandle* m_fred1;
	GraphHandle* m_french1;
	GraphHandle* m_mascara1;
	GraphHandle* m_yuri1;
	GraphHandle* m_rabbi1;

	// part2
	GraphHandles* m_heartAndMem;
	GraphHandles* m_heartCry1;
	GraphHandles* m_heartCry2;
	GraphHandles* m_memSad1;
	GraphHandles* m_memSad2;
	GraphHandles* m_eyeFocus;

	// キャラ
	GraphHandles* m_archive;
	GraphHandles* m_aigis;
	GraphHandles* m_assault;
	GraphHandles* m_vermelia;
	GraphHandles* m_exlucina;
	GraphHandles* m_msadi;
	GraphHandles* m_elnino;
	GraphHandles* m_onyx;
	GraphHandles* m_courir;
	GraphHandles* m_cornein;
	GraphHandles* m_koharu;
	GraphHandles* m_siesta;
	GraphHandles* m_chocola;
	GraphHandles* m_titius;
	GraphHandles* m_heart;
	GraphHandles* m_fred;
	GraphHandles* m_french;
	GraphHandles* m_mascara;
	GraphHandles* m_memoryA;
	GraphHandles* m_memoryB;
	GraphHandles* m_yuri;
	GraphHandles* m_rabbi;

	// サビ
	GraphHandles* m_orange;
	GraphHandles* m_duplications;
	Animation* m_orangeAnime;
	Animation* m_duplicationsAnime;
	GraphHandles* m_heartframe;
	GraphHandles* m_rmem;
	GraphHandles* m_heartSabi;
	GraphHandles* m_tvSiesta;
	GraphHandles* m_tvHierarchy;
	GraphHandles* m_tvValkiria;
	GraphHandles* m_tvTroy;
	GraphHandles* m_tvHeart;
	GraphHandles* m_tvShine;
	GraphHandles* m_tvRshine;
	GraphHandles* m_titleFinal;

	// キャラを順に表示する用 (graph, cntSum)
	std::queue<std::pair<GraphHandles*, int> > characterQueue;

public:
	OpMovie(SoundPlayer* soundPlayer_p);
	~OpMovie();

	// 再生
	void play();

	// 描画
	void draw();

private:
	void pushPartOneCharacter(int index, bool front, GraphHandle* character);
};

// オープニング (mp4)
class OpMovieMp4 :
	public Movie
{
private:

	int m_mp4;

public:
	OpMovieMp4(SoundPlayer* soundPlayer_p);
	~OpMovieMp4();

	// 再生
	void play();

	// 描画
	void draw();
};

#endif