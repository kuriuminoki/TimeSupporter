#ifndef TEXT_H_INCLUDED
#define TEXT_H_INCLUDED


#include <string>
#include <vector>


class Animation;
class SoundPlayer;
class World;
class GraphHandle;
class GraphHandles;
class Button;


/*
* イベント中に挿入される画像
*/
class EventAnime {

private:

	// アニメのスピード
	int m_speed = 5;

	// 挿絵
	GraphHandles* m_handles;
	Animation* m_animation;

	// 画像の明るさ
	int m_bright;

	// 暗くなっていく方向 明るくなるならfalse
	bool m_toDark;

	bool m_finishFlag;

	bool m_heavyFlag;
	bool m_lightFlag;
	int m_alpha;

public:

	EventAnime(const char* filePath, int sum, int speed = -1);

	~EventAnime();

	// ゲッタ
	inline const bool getToDark() const { return m_toDark; }
	inline const int getBright() const { return m_bright; }
	inline const Animation* getAnime() const { return m_animation; }
	inline const int getAlpha() const { return m_alpha; }

	// セッタ
	inline void setToDark(bool toDark) { m_toDark = toDark; }
	inline void setBright(int bright) { m_bright = bright; }
	inline void setFinishFlag(bool flag) { m_finishFlag = flag; }
	inline void setHeavyTrue() { m_heavyFlag = true; m_alpha = 0; }
	inline void setLightTrue() { m_lightFlag = true; m_alpha = 255; }

	// アニメイベントが終わったか
	bool getFinishAnimeEvent() const;

	// falseの間は操作不可
	void play();

};


/*
* フキダシのアクション
*/
class TextAction {
private:

	// ジャンプ中
	bool m_jumpFlag;

	// 座標のずれ
	int m_dx, m_dy;

	// 速度
	int m_vy;

	// 振動する残り時間
	int m_quakeCnt;

	// 振動によるずれ
	int m_quakeDx, m_quakeDy;

public:

	TextAction();

	// ゲッタ
	int getDx() const { return m_dx + m_quakeDx; }
	int getDy() const { return m_dy + m_quakeDy; }

	// セッタ
	void setVy(int vy) { m_vy = vy; m_jumpFlag = true; }
	void setQuakeCnt(int cnt) { m_quakeCnt = cnt; }

	void init();

	void play();

};


/*
* 会話イベント
*/
class Conversation {
private:

	// 終了時、少しだけ待機時間
	const int FINISH_COUNT = 30;
	// 0 -> FINISH_COUNTで発言終了
	int m_finishCnt;
	// FINISH_CINT -> 0で発言開始
	int m_startCnt;

	// Zキーの長押し時間
	int m_skipCnt;

	// イベント終了したか
	bool m_finishFlag;

	// 文字表示の速さ 1が最速
	const unsigned int TEXT_SPEED = 5;
	unsigned int m_textSpeed;

	// 次のテキストへ行けるようになるまでの時間
	const unsigned int NEXT_TEXT_ABLE = 30;

	// テキストを飛ばせるようになるまでの時間
	const unsigned int MOVE_FINAL_ABLE = 10;

	// ファイルポインタ
	int m_fp;

	// 条件分岐用
	bool m_if; // trueならifブロック内を実行中
	std::vector<std::string> m_marks; // マーク
	int m_font;
	Button* m_yesButton;
	Button* m_noButton;
	bool m_selectFlag; // 今選択待ち

	// 世界
	World* m_world_p;

	// 世界のサウンドプレイヤー
	SoundPlayer* m_soundPlayer_p;

	// アニメイベント
	EventAnime* m_eventAnime;

	// 発言者の名前
	std::string m_speakerName;

	// 発言者の顔画像がない
	bool m_noFace;

	// 発言者の顔画像
	GraphHandles* m_speakerGraph_p;

	// 発言
	std::string m_text;

	// 今何文字目まで発言したか
	unsigned int m_textNow;

	// カウント
	unsigned int m_cnt;

	// 文字表示効果音
	int m_displaySound;

	// 決定効果音
	int m_nextSound;

	// BGMを変更しても戻せるよう
	std::string m_originalBgmPath;

	// クリックエフェクト等のアニメーション このクラスがデリートする
	GraphHandles* m_clickGraph;
	std::vector<Animation*> m_animations;

	// 発言終了時の印画像
	GraphHandle* m_textFinishGraph;

	// フキダシのアクション
	TextAction m_textAction;

	// 効果音
	int m_sound;

	// 顔画像の表示方法
	enum FaceDrawMode {
		NORMAL,	// 普通にアニメを繰り返し
		FREEZE,	// アニメを動かさない
		ONCE	// 一度だけ再生
	};
	FaceDrawMode m_faceDrawMode;

public:
	Conversation(int textNum, World* world, SoundPlayer* soundPlayer);
	~Conversation();

	// ゲッタ
	std::string getText() const;
	inline std::string getFullText() const { return m_text; }
	int getTextSize() const;
	GraphHandle* getGraph() const;
	inline bool getNoFace() const { return m_noFace; }
	inline 	std::string getSpeakerName() const { return m_speakerName; }
	inline int getFinishCnt() const { return m_finishCnt; }
	inline int getSkipCnt() const { return m_skipCnt; }
	inline int getStartCnt() const { return m_startCnt; }
	inline bool getFinishFlag() const { return m_finishFlag; }
	inline int getTextNow() const { return m_textNow; }
	inline int getCnt() const { return m_cnt; }
	inline const Animation* getAnime() const { 
		if (m_eventAnime == nullptr) { return nullptr; }
		return m_eventAnime->getAnime();
	}
	inline int getAnimeBright() const { return m_eventAnime->getBright(); }
	inline int getAnimeAlpha() const { return m_eventAnime->getAlpha(); }
	inline const std::vector<Animation*> getAnimations() const { return m_animations; }
	inline const GraphHandle* getTextFinishGraph() const { return m_textFinishGraph; }
	inline const EventAnime* getEventAnime() const { return m_eventAnime; }
	inline const TextAction getTextAction() const { return m_textAction; }
	inline const Button* getYesButton() const { return m_selectFlag ? m_yesButton : nullptr; }
	inline const Button* getNoButton() const { return m_selectFlag ? m_noButton : nullptr; }

	// セッタ
	void setWorld(World* world);

	// 今アニメ再生中か
	bool animePlayNow() const { return m_eventAnime == nullptr ? false : !m_eventAnime->getFinishAnimeEvent(); }

	// 会話を行う
	bool play();

	// 最後までセリフ表示したか
	bool finishText() const;

	// 次のセリフへ移行できるか
	bool nextTextAble() const { return m_cnt > NEXT_TEXT_ABLE; }

private:
	void loadNextBlock();
	void setNextText(const int size, char* buff);
	void setSpeakerGraph(const char* faceName);
};


#endif