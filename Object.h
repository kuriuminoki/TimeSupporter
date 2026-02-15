#ifndef OBJECT_H_INCLUDED
#define OBJECT_H_INCLUDED

#include <string>
#include <vector>

class Character;
class CharacterController;
class AttackInfo;
class GraphHandle;
class GraphHandles;
class Animation;
class Item;


/*
* オブジェクトの基底クラス
*/
class Object {
protected:
	// id
	static int objectId;

	int m_id;

	std::vector<int> m_atariIdList;

	// 左上の座標
	int m_x1, m_y1;

	// 右下の座標+1
	int m_x2, m_y2;

	// オブジェクトの画像
	GraphHandle* m_handle;

	// HP -1なら無敵
	int m_hp;

	// (破壊時)着弾時に爆発するか
	bool m_bomb;

	// ダメージ状態（描画用）
	int m_damageCnt;
	const int DAMAGE_CNT_SUM = 5;

	// 削除フラグ trueならWorldに消してもらう
	bool m_deleteFlag;

	// エフェクト
	GraphHandles* m_effectHandles_p;

	// サウンド
	int m_soundHandle_p;

public:
	Object();
	Object(int x1, int y1, int x2, int y2, int hp = -1);
	virtual ~Object() { }

	virtual Object* createCopy() = 0;
	void setParam(Object* object);

	void debugObject(int x, int y, int color) const;
	virtual void debug(int x, int y, int color) const = 0;

	// ゲッタ
	inline int getId() const { return m_id; }
	inline bool getDeleteFlag() const { return m_deleteFlag; }
	inline bool getAbleDelete() const { return m_hp != -1 ? true : false; }
	inline int getX1() const { return m_x1; }
	inline int getX2() const { return m_x2; }
	inline int getY1() const { return m_y1; }
	inline int getY2() const { return m_y2; }
	inline int getCenterX() const { return (m_x1 + m_x2) / 2; }
	inline int getCenterY() const { return (m_y1 + m_y2) / 2; }
	inline int getSoundHandle() const { return m_soundHandle_p; }
	inline int getHp() const { return m_hp; }
	inline int getBomb() const { return m_bomb; }
	inline int getDamageCnt() const { return m_damageCnt; }
	virtual const char* getFileName() const { return ""; }
	virtual bool getTextDisp() const { return false; }


	// セッタ
	inline void setId(int id) { m_id = id; }
	inline void setAtariIdList(std::vector<int>& list) { for (unsigned int i = 0; i < list.size(); i++) { m_atariIdList.push_back(list[i]); } }
	inline void setDeleteFlag(bool deleteFlag) { m_deleteFlag = deleteFlag; }
	void setX1(int x1) { m_x1 = x1; }
	void setY1(int y1) { m_y1 = y1; }
	void setX2(int x2) { m_x2 = x2; }
	void setY2(int y2) { m_y2 = y2; }
	void setHp(int hp) { m_hp = hp; }
	void setDamageCnt(int damageCnt) { m_damageCnt = damageCnt; }
	void setEffectHandles(GraphHandles* effectHandles_p) { m_effectHandles_p = effectHandles_p; }
	inline void setGraphHandle(GraphHandle* handle) { m_handle = handle; }
	void setSoundHandle(int soundHandle_p) { m_soundHandle_p = soundHandle_p; }
	virtual inline void setTextDisp(const bool textDisp) {}

	// slopeかどうか
	virtual bool slopeFlag() const { return false; }

	// 座標XにおけるY1座標（傾きから算出する）
	virtual int getY(int x) const { return m_y1; }

	// HPを減らす
	bool decreaseHp(int damageValue, int id);

	// IDのゲッタ
	virtual inline int getCharacterId() const { return -1; }
	virtual inline int getGroupId() const { return -1; }
	virtual inline int getStopCharacterId() const { return -1; }
	virtual inline int getStopCnt() const { return 0; }

	// 攻撃力 攻撃オブジェクト用
	virtual inline int getDamage() const { return 0; }

	// 扉用
	virtual inline int getAreaNum() const { return -1; }
	virtual inline int getTextNum() const { return -1; }

	// 画像を返す　ないならnullptr
	virtual GraphHandle* getHandle() const { return m_handle; }

	// 画像を敷き詰めて表示するならtrue
	virtual bool lineUpType() const { return false; }

	// 画像の大きさを自動調節する
	virtual bool extendGraph() const { return true; }

	// テキストを返す ないならnullptr
	virtual inline std::string getText() const { return ""; }

	// テキストをセット
	virtual inline void setText(const char* text) {  }

	virtual inline void setStopCnt(int stopCnt) {  }

	// オブジェクト描画（画像がないときに使う）
	virtual void drawObject(int x1, int y1, int x2, int y2) const {}

	// キャラとの当たり判定
	virtual bool atari(CharacterController* characterController) = 0;

	// 単純に四角の落下物と衝突しているか
	virtual bool atariDropBox(int x1, int y1, int x2, int y2, int& vx, int& vy);

	// キャラがオブジェクトに入り込んでいるときの処理
	virtual void penetration(CharacterController* characterController) {}

	// 他オブジェクトからの当たり判定
	virtual bool atariFromObject(Object* object) { return false; }

	// 他オブジェクトに対する当たり判定
	virtual bool atariToObject(Object* object) { return false; }

	// 動くオブジェクト用 毎フレーム行う
	virtual void action() {}

	// 攻撃オブジェクト用 エネルギー放出
	virtual Item* createAttackEnergy() { return nullptr; }

	// アニメーション作成
	virtual Animation* createAnimation(int x, int y, int flameCnt) { return nullptr; }
};


/*
* 四角形のオブジェクト　床や壁
*/
class BoxObject :
	public Object 
{
private:
	// 段差とみなして自動で乗り越えられる高さ
	const int STAIR_HEIGHT = 200;

	// オブジェクトの色
	int m_color;

	// ファイルネームを保存しておく
	std::string m_fileName;

public:
	BoxObject(int x1, int y1, int x2, int y2, const char* fileName, int color, int hp = -1);

	~BoxObject();

	Object* createCopy();

	void debug(int x, int y, int color) const;

	// 画像を敷き詰めて表示するならtrue
	bool lineUpType() const { return true; }

	// オブジェクト描画（画像がないときに使う）
	void drawObject(int x1, int y1, int x2, int y2) const;

	// キャラとの当たり判定
	// 当たっているならキャラを操作する。
	bool atari(CharacterController* characterController);

	// キャラがオブジェクトに入り込んでいるときの処理
	void penetration(CharacterController* characterController);

	// 攻撃オブジェクトとの当たり判定
	bool atariFromObject(Object* object);

	// 動くオブジェクト用 毎フレーム行う
	void action();
};

/*
* 直角三角形のオブジェクトつまり坂
*/
class TriangleObject :
	public Object
{
private:
	// オブジェクトの色
	int m_color;

	// ファイルネームを保存しておく
	std::string m_fileName;

	// 左向きに下がっている坂
	bool m_leftDown;

public:

	TriangleObject(int x1, int y1, int x2, int y2, const char* fileName, int color, bool leftDown, int hp = -1);

	~TriangleObject();

	Object* createCopy();

	void debug(int x, int y, int color) const;

	// slopeかどうか
	bool slopeFlag() const { return true; }

	// 座標XにおけるY1座標（傾きから算出する）
	int getY(int x) const;

	// オブジェクト描画（画像がないときに使う）
	void drawObject(int x1, int y1, int x2, int y2) const;

	// キャラとの当たり判定
	// 当たっているならキャラを操作する。
	bool atari(CharacterController* characterController);

	// 単純に四角の落下物と衝突しているか
	bool atariDropBox(int x1, int y1, int x2, int y2, int& vx, int& vy);

	// キャラがオブジェクトに入り込んでいるときの処理
	void penetration(CharacterController* characterController);

	// 攻撃オブジェクトとの当たり判定
	bool atariFromObject(Object* object);

	// 動くオブジェクト用 毎フレーム行う
	void action();
};

/*
* 直進する弾のオブジェクト
* gx, gyに向かって飛んでいき、一定距離移動したら消滅
*/
class BulletObject :
	public Object
{
protected:

	// この攻撃を出したキャラのＩＤ 自滅防止用
	int m_characterId;

	// この攻撃が当たらないグループのID チームキル防止用
	int m_groupId;

	// オブジェクトの色
	int m_color;

	// 半径
	int m_rx;
	int m_ry;

	// スピード
	int m_v;
	int m_vx;
	int m_vy;

	// 目標地点
	int m_gx;
	int m_gy;

	// 残りの飛距離
	int m_d;

	// ダメージ
	int m_damage;

	// エネルギー放出用のカウント
	int m_energyCnt;
	// エネルギーが残る時間。 0以下なら放出しない
	int m_energyEraseTime;

public:
	// x, y, gx, gyは弾の中心座標
	BulletObject(int x, int y, int color, int gx, int gy, int energyEraseTime, AttackInfo* attackInfo);
	BulletObject(int x, int y, int color, int gx, int gy, int energyEraseTime);
	BulletObject(int x, int y, GraphHandle* handle, int gx, int gy, int energyEraseTime, AttackInfo* attackInfo);

	Object* createCopy();
	void setBulletParam(BulletObject* obejct);

	void debug(int x, int y, int color) const;

	// オブジェクト描画（画像がないときに使う）
	void drawObject(int x1, int y1, int x2, int y2) const;

	// ゲッタ
	inline int getCharacterId() const { return m_characterId; }
	inline int getGroupId() const { return m_groupId; }

	// 画像ハンドルを返す
	GraphHandle* getHandle() const;

	// セッタ
	// キャラクターIDをセット
	inline void setCharacterId(int id) { m_characterId = id; }
	inline void setGroupId(int id) { m_groupId = id; }
	inline void setColor(int color) { m_color = color; }
	inline void setRx(int rx) { m_rx = rx; }
	inline void setRy(int ry) { m_ry = ry; }
	inline void setV(int v) { m_v = v; }
	inline void setVx(int vx) { m_vx = vx; }
	inline void setVy(int vy) { m_vy = vy; }
	inline void setGx(int gx) { m_gx = gx; }
	inline void setGy(int gy) { m_gy = gy; }
	inline void setD(int d) { m_d = d; }
	inline void setDamage(int damage) { m_damage = damage; }
	inline void setEnergyCnt(int energyCnt) { m_energyCnt = energyCnt; }

	// 攻撃力 攻撃オブジェクト用
	inline int getDamage() const { return m_damage; }

	// キャラとの当たり判定
	// 当たっているならキャラを操作する。
	bool atari(CharacterController* characterController);

	// 攻撃オブジェクトとの当たり判定
	bool atariToObject(Object* object);

	// 動くオブジェクト用 毎フレーム行う
	void action();

	// 攻撃オブジェクト用 エネルギー放出
	Item* createAttackEnergy();

	// アニメーション作成
	Animation* createAnimation(int x, int y, int flameCnt);
};

/*
* 放物線を描く弾
*/
class ParabolaBullet :
	public BulletObject
{
public:
	static const int G = 2;

	ParabolaBullet(int x, int y, int color, int gx, int gy, int energyEraseTime, AttackInfo* attackInfo);
	ParabolaBullet(int x, int y, GraphHandle* handle, int gx, int gy, int energyEraseTime, AttackInfo* attackInfo);
	ParabolaBullet(int x, int y, GraphHandle* handle, int gx, int gy, int energyEraseTime);

	Object* createCopy();

	void debug(int x, int y, int color) const;

	inline void setGraphHandle(GraphHandle* handle) { m_handle = handle; }

	// 画像ハンドルを返す
	GraphHandle* getHandle() const;

	// 動くオブジェクト用 毎フレーム行う
	void action();

	// 画像の大きさを自動調節する
	bool extendGraph() const { return false; }
};

/*
* 近距離攻撃のオブジェクト
* 一定時間経過したら消滅
*/
class SlashObject :
	public Object
{
private:
	// この攻撃を出したキャラのＩＤ 自滅やチームキル防止用
	int m_characterId;

	// この攻撃が当たらないグループのID チームキル防止用
	int m_groupId;

	// ダメージ
	int m_damage;

	// 何フレーム目か
	int m_cnt;

	// 全体フレーム
	int m_slashCountSum;

	// 斬撃の吹っ飛び(X方向の初速)
	int m_slashImpactX;

	// 斬撃の吹っ飛び(Y方向の初速)
	int m_slashImpactY;

	// ストップする残り時間
	int m_stopCnt;

	// エネルギーが残る時間。 0以下なら放出しない
	int m_energyEraseTime;

public:
	// 座標、画像、生存時間、AttackInfo
	SlashObject(int x1, int y1, int x2, int y2, GraphHandle* handle, int slashCountSum, int energyEraseTime, const AttackInfo* attackInfo);

	SlashObject(int x1, int y1, int x2, int y2, GraphHandle* handle, int slashCountSum, int energyEraseTime);

	// 大きさを指定しない場合。画像からサイズ取得。生存時間、AttackInfo
	SlashObject(int x, int y, GraphHandle* handle, int slashCountSum, int energyEraseTime, AttackInfo* attackInfo);

	Object* createCopy();

	void setSlashParam(SlashObject* object);

	void debug(int x, int y, int color) const;

	// ゲッタ
	inline int getGroupId() const { return m_groupId; }
	inline int getCharacterId() const { return m_characterId; }
	inline int getStopCharacterId() const { return m_characterId; }
	inline int getStopCnt() const { return m_stopCnt; }

	// セッタ
	inline void setCharacterId(int id) { m_characterId = id; }
	inline void setGroupId(int id) { m_groupId = id; }
	inline void setDamage(int damage) { m_damage = damage; }
	inline void setGraphHandle(GraphHandle* handle) { m_handle = handle; }
	inline void setCnt(int cnt) { m_cnt = cnt; }
	inline void setSlashImpactX(int slashImpactX) { m_slashImpactX = slashImpactX; }
	inline void setSlashImpactY(int slashImpactY) { m_slashImpactY = slashImpactY; }
	inline void setStopCnt(int stopCnt) { m_stopCnt = stopCnt; }

	// 攻撃力 攻撃オブジェクト用
	inline int getDamage() const { return m_damage; }

	// キャラとの当たり判定
	// 当たっているならキャラを操作する。
	bool atari(CharacterController* characterController);

	// 攻撃オブジェクトとの当たり判定
	bool atariToObject(Object* object);

	// 動くオブジェクト用 毎フレーム行う
	void action();

	// 攻撃オブジェクト用 エネルギー放出
	Item* createAttackEnergy();

	// アニメーション作成
	Animation* createAnimation(int x, int y, int flameCnt);
};


/*
* 爆発の当たり判定
*/
class BombObject :
	public Object
{
public:

	// この攻撃を出したキャラのＩＤ 自滅やチームキル防止用
	int m_characterId;

	// この攻撃が当たらないグループのID チームキル防止用
	int m_groupId;

	// 中心の攻撃力
	int m_damage;

	// 中心の座標
	int m_x, m_y;
	
	int m_dx, m_dy;

	int m_distance;

	// 爆発のアニメ
	Animation* m_animation;

	const int BOMB_IMPACT = 30;

public:

	BombObject(int x, int y, int dx, int dy, int damage, Animation* bombAnimation);

	~BombObject();

	// ゲッタ
	inline int getCharacterId() const { return m_characterId; }
	inline int getGroupId() const { return m_groupId; }

	// セッタ
	void setCharacterId(int characterId) { m_characterId = characterId; }
	void setGroupId(int groupId) { m_groupId = groupId; }

	Object* createCopy();

	void debug(int x, int y, int color) const;

	// キャラとの当たり判定
	// 当たっているならキャラを操作する。
	bool atari(CharacterController* characterController);

	// 攻撃オブジェクトとの当たり判定
	bool atariToObject(Object* object);

	// 動くオブジェクト用 毎フレーム行う
	void action();

private:

	double calcDamageRate(int x, int y);

	bool ableDamage();

};


// 扉オブジェクト
class DoorObject :
	public Object 
{
protected:
	// ファイルネームを保存しておく
	std::string m_fileName;

	// 行先のエリア番号
	int m_areaNum;

	// チュートリアルのテキスト
	std::string m_text;
	bool m_textDisp;

	std::string m_defaultText;

	int m_textNum;

public:
	DoorObject(int x1, int y1, int x2, int y2, const char* fileName, int areaNum);
	~DoorObject();

	Object* createCopy();

	void debug(int x, int y, int color) const;

	// ゲッタ
	inline int getAreaNum() const { return m_areaNum; }
	inline std::string getText() const { return m_textDisp ? m_defaultText : m_text; }
	const char* getFileName() const { return m_fileName.c_str(); }
	inline int getTextNum() const { return m_textNum; }
	inline bool getTextDisp() const { return m_textDisp; }

	// セッタ
	inline void setText(const char* text) { m_text = text; }
	inline void setTextDisp(const bool textDisp) { m_textDisp = textDisp; }

	// キャラとの当たり判定
	virtual bool atari(CharacterController* characterController);
};


// 当たり判定のないオブジェクト
class StageObject :
	public DoorObject
{
public:

	StageObject(int x1, int y1, int x2, int y2, const char* fileName, int textNum);
	~StageObject();

	Object* createCopy();

};

#endif