#ifndef CHARACTER_H_INCLUDED
#define CHARACTER_H_INCLUDED


#include<string>
#include<map>
#include<vector>

class Object;
class BulletObject;
class SlashObject;
class GraphHandle;
class GraphHandles;
class CharacterGraphHandle;
class FaceGraphHandle;
class SoundPlayer;


class CharacterInfo {
private:
	// キャラの名前
	std::string m_name;

	// キャラの初期体力
	int m_maxHp;

	// 画像の拡大率
	double m_handleEx;

	// 走るスピード
	int m_moveSpeed;

	// ジャンプ時のY方向の初速
	int m_jumpHeight;

	// 射撃時に撃つ方向を向くか
	bool m_sameBulletDirection;

	// ジャンプ時の音
	int m_jumpSound;

	// 受け身時の音
	int m_passiveSound;

	// 着地時の音
	int m_landSound;

public:
	// デフォルト値で初期化
	CharacterInfo();
	// csvファイルを読み込み、キャラ名で検索しパラメータ取得
	CharacterInfo(const char* characterName);

	~CharacterInfo();

	// ゲッタのみを持つ、セッタは持たない
	inline std::string name() const { return m_name; }
	inline int maxHp() const { return m_maxHp; }
	inline double handleEx() const { return m_handleEx; }
	inline int moveSpeed() const { return m_moveSpeed; }
	inline int jumpHeight() const { return m_jumpHeight; }
	inline bool sameBulletDirection() const { return m_sameBulletDirection; }
	inline int jumpSound() const { return m_jumpSound; }
	inline int passiveSound() const { return m_passiveSound; }
	inline int landSound() const { return m_landSound; }

	// バージョン変更
	void changeVersion(int version);

private:

	void setParam(std::map<std::string, std::string>& data);

};


class AttackInfo {
private:
	// 弾丸のHP
	int m_bulletHp;

	// 弾丸のダメージ
	int m_bulletDamage;

	// 弾丸の大きさ(半径)
	int m_bulletRx, m_bulletRy;

	// 弾丸のスピード
	int m_bulletSpeed;

	// 弾丸の連射力
	int m_bulletRapid;

	// 弾丸の飛距離
	int m_bulletDistance;

	// 弾丸の吹っ飛び(X方向の初速)
	int m_bulletImpactX;

	// 弾丸の吹っ飛び(Y方向の初速)
	int m_bulletImpactY;

	// 着弾時に爆発するか
	bool m_bulletBomb;

	// 斬撃のHP
	int m_slashHp;

	// 斬撃のダメージ
	int m_slashDamage;

	// 斬撃の大きさ(長方形の辺の長さ)
	int m_slashLenX, m_slashLenY;

	// 斬撃の全体フレーム
	int m_slashCountSum;

	// 斬撃の後隙
	int m_slashInterval;

	// 斬撃の吹っ飛び(X方向の初速)
	int m_slashImpactX;

	// 斬撃の吹っ飛び(Y方向の初速)
	int m_slashImpactY;

	// 射撃攻撃のエフェクト
	GraphHandles* m_bulletEffectHandles;

	// 斬撃攻撃のエフェクト
	GraphHandles* m_slashEffectHandles;

	// 射撃攻撃が当たったときのサウンド
	int m_bulletSoundHandle;

	// 弾発射音
	int m_bulletStartSoundHandle;

	// 斬撃攻撃が当たったときのサウンド
	int m_slashSoundHandle;

	// 斬撃開始サウンド
	int m_slashStartSoundHandle;

public:
	// デフォルト値で初期化
	AttackInfo();
	// csvファイルを読み込み、キャラ名で検索しパラメータ取得
	AttackInfo(const char* characterName, double drawEx);

	~AttackInfo();
	
	// ゲッタのみを持つ、セッタは持たない
	int bulletHp() const { return m_bulletHp; }
	int bulletDamage() const { return m_bulletDamage; }
	int bulletRx() const { return m_bulletRx; }
	int bulletRy() const { return m_bulletRy; }
	int bulletSpeed() const { return m_bulletSpeed; }
	int bulletRapid() { return m_bulletRapid; }
	int bulletDistance() const { return m_bulletDistance; }
	int bulletImpactX() const { return m_bulletImpactX; }
	int bulletImpactY() const { return m_bulletImpactY; }
	bool bulletBomb() const { return m_bulletBomb; }
	int slashHp() const { return m_slashHp; }
	int slashDamage() const { return m_slashDamage; }
	int slashLenX() const { return m_slashLenX; }
	int slashLenY() const { return m_slashLenY; }
	int slashCountSum() const { return m_slashCountSum; }
	int slashInterval() const { return m_slashInterval; }
	int slashImpactX() const { return m_slashImpactX; }
	int slashImpactY() const { return m_slashImpactY; }
	GraphHandles* bulletEffectHandle() const { return m_bulletEffectHandles; }
	GraphHandles* slashEffectHandle() const { return m_slashEffectHandles; }
	int bulletSoundeHandle() const { return m_bulletSoundHandle; }
	int slashSoundHandle() const { return m_slashSoundHandle; }
	int bulletStartSoundeHandle() const { return m_bulletStartSoundHandle; }
	int slashStartSoundHandle() const { return m_slashStartSoundHandle; }

	// バージョン変更
	void changeVersion(const char* characterName, int version);

private:

	void setParam(std::map<std::string, std::string>& data);

};


/*
* プレイヤーやエネミーの基底クラス
*/
class Character {
public:

	// キャラを揺らして描画するならtrue
	const bool SHAKING_FLAG = false;

	const int SKILL_MAX = 100;

	const int DEFAULT_ANIME_SPEED = 3;

	const int DEFAULT_SLASH_ENERGY_TIME = 60;
	const int DEFAULT_BULLET_ENERGY_TIME = 10;

protected:
	static int characterId;

	AttackInfo* m_slidingInfo;

	bool m_duplicationFlag;

	// やられたらTrue。HP==0になっても画面に非表示にするのは次のフレームからにするため使う
	bool m_deadFlag;

	// ID
	int m_id;

	// グループID 味方識別用
	int m_groupId;

	// Infoのバージョン
	int m_version;

	// 残り体力
	int m_hp;

	// ダメージを受ける前の体力
	int m_prevHp;

	// HPバーを表示する残り時間
	int m_dispHpCnt;

	// スキルゲージ 最大100
	int m_skillGage;

	// 無敵ならtrue
	bool m_invincible;

	// X座標、Y座標
	int m_x, m_y;

	// 左を向いている
	bool m_leftDirection;

	// 一時的に動けない状態（ハートのスキル発動など）
	bool m_freeze;

	// ボスならtrue
	bool m_bossFlag;

	// 描画用
	int m_drawCnt;

	// キャラの情報
	CharacterInfo* m_characterInfo;

	// 攻撃の情報
	AttackInfo* m_attackInfo;

	// キャラ画像
	CharacterGraphHandle* m_graphHandle;

	// 顔画像
	FaceGraphHandle* m_faceHandle;

	// 獲得したお金 Worldに渡したら0にする
	int m_money;

	// フリーズ中ならその残り時間を保持
	int m_stopCnt;

public:
	// コンストラクタ
	Character();
	Character(int hp, int x, int y, int groupId);
	virtual ~Character();

	virtual Character* createCopy() = 0;
	void setParam(Character* character);

	// デバッグ
	void debugCharacter(int x, int y, int color) const;
	virtual void debug(int x, int y, int color) const = 0;

	// ゲッタ
	inline bool getDeadFlag() const { return m_deadFlag; }
	inline int getId() const { return m_id; }
	inline int getGroupId() const { return m_groupId; }
	inline int getVersion() const { return m_version; }
	inline int getHp() const { return m_hp; }
	inline int getPrevHp() const { return m_prevHp; }
	inline int getSkillGage() const { return m_skillGage; }
	inline int getMaxSkillGage() const { return SKILL_MAX; }
	inline int getDispHpCnt() const { return m_dispHpCnt; }
	inline bool getInvincible() const { return m_invincible; }
	inline int getX() const { return m_x; }
	inline int getY() const { return m_y; }
	inline bool getLeftDirection() const { return m_leftDirection; }
	inline bool getFreeze() const { return m_freeze; }
	inline bool getBossFlag() const { return m_bossFlag; }
	FaceGraphHandle* getFaceHandle() const { return m_faceHandle; }
	inline CharacterGraphHandle* getCharacterGraphHandle() const { return m_graphHandle; }
	inline AttackInfo* getAttackInfo() const { return m_attackInfo; }
	inline CharacterInfo* getCharacterInfo() const { return m_characterInfo; }
	inline int getMoney() const { return m_money; }
	inline int getStopCnt() const { return m_stopCnt; }

	// セッタ
	inline void setDeadFlag(bool deadFlag) { m_deadFlag = deadFlag; }
	inline void setHp(int hp) { m_hp = (hp > m_characterInfo->maxHp()) ? m_characterInfo->maxHp() : hp; m_prevHp = m_hp; }
	inline void setPrevHp(int prevHp) { 
		m_prevHp = (prevHp < m_hp) ? m_hp : prevHp;
		if (m_prevHp == m_hp && m_dispHpCnt > 0) { m_dispHpCnt--; }
	}
	inline void setSkillGage(int skillGage) { 
		m_skillGage = skillGage > SKILL_MAX ? SKILL_MAX : (skillGage < 0 ? 0 : skillGage);
	}
	inline void setInvincible(bool invincible) { m_invincible = invincible; }
	inline void setX(int x) { m_x = x; }
	inline void setY(int y) { m_y = y; }
	inline void setId(int id) { m_id = id; }
	inline void setGroupId(int id) { m_groupId = id; }
	inline void setFreeze(bool freeze) { m_freeze = freeze; }
	inline void setBossFlag(bool bossFlag) { m_bossFlag = bossFlag; }
	inline void setStopCnt(int stopCnt) { m_stopCnt = stopCnt; }
	// キャラの向き変更は、画像の反転も行う
	void setLeftDirection(bool leftDirection);
	inline void setDuplicationFlag(bool flag) { m_duplicationFlag = flag; }
	inline void setAttackInfo(AttackInfo* attackInfo) { m_attackInfo = attackInfo; }
	inline void setCharacterInfo(CharacterInfo* characterInfo) { m_characterInfo = characterInfo; }
	inline void setMoney(int money) { m_money = money; }

	// CharacterInfoからキャラのスペックを取得
	inline std::string getName() const { return m_characterInfo->name(); }
	inline int getMaxHp() const { return m_characterInfo->maxHp(); }
	inline int getMoveSpeed() const { return m_characterInfo->moveSpeed(); }
	inline int getJumpHeight() const { return m_characterInfo->jumpHeight(); }
	inline int getJumpSound() const { return m_characterInfo->jumpSound(); }
	inline int getPassiveSound() const { return m_characterInfo->passiveSound(); }
	inline int getLandSound() const { return m_characterInfo->landSound(); }

	// AttackInfoから攻撃のスペックを取得
	inline int getBulletRapid() const { return m_attackInfo->bulletRapid(); }
	inline int getSlashCountSum() const { return m_attackInfo->slashCountSum(); }
	inline int getSlashInterval() const { return m_attackInfo->slashInterval(); }

	// 当たり判定の範囲を取得
	void getAtariArea(int* x1, int* y1, int* x2, int* y2) const;
	void getDamageArea(int* x1, int* y1, int* x2, int* y2) const;

	// Infoのバージョンを変更する
	void changeInfoVersion(int version);

	// 画像の情報を取得
	int getCenterX() const;
	int getCenterY() const;
	int getWide() const;
	int getHeight() const;
	int getAtariCenterX() const;
	int getAtariCenterY() const;
	// 今描画する画像を取得
	GraphHandle* getGraphHandle() const;
	void getHandleSize(int& wide, int& height) const;

	// 立ち画像をセット
	virtual void switchStand(int cnt = 0);
	// 立ち射撃画像をセット
	virtual void switchBullet(int cnt = 0);
	// 立ち斬撃画像をセット
	virtual void switchSlash(int cnt = 0);
	// しゃがみ画像をセット
	virtual void switchSquat(int cnt = 0);
	// しゃがみ射撃画像をセット
	virtual void switchSquatBullet(int cnt = 0);
	// 走り画像をセット
	virtual void switchRun(int cnt = 0);
	// 走り射撃画像をセット
	virtual void switchRunBullet(int cnt = 0);
	// 着地画像をセット
	virtual void switchLand(int cnt = 0);
	// 上昇画像をセット
	virtual void switchJump(int cnt = 0);
	// 降下画像をセット
	virtual void switchDown(int cnt = 0);
	// ジャンプ前画像をセット
	virtual void switchPreJump(int cnt = 0);
	// ダメージ画像をセット
	virtual void switchDamage(int cnt = 0);
	// ブースト画像をセット
	virtual void switchBoost(int cnt = 0);
	// ステップ画像をセット
	virtual void switchStep(int cnt = 0);
	// スライディング画像をセット
	virtual void switchSliding(int cnt = 0);
	// 空中射撃画像をセット
	virtual void switchAirBullet(int cnt = 0);
	// 空中斬撃画像をセット
	virtual void switchAirSlash(int cnt = 0);
	// やられ画像をセット
	virtual void switchDead(int cnt = 0);
	// ボスの初期アニメーションをセット
	virtual void switchInit(int cnt = 0);
	// 追加画像をセット
	virtual void switchSpecial1(int cnt = 0);

	// HP減少
	void damageHp(int value);

	// 移動する（座標を動かす）
	void moveRight(int d);
	void moveLeft(int d);
	void moveUp(int d);
	void moveDown(int d);

	// 射撃攻撃をする(キャラごとに違う)
	virtual std::vector<Object*>* bulletAttack(int cnt, int gx, int gy, SoundPlayer* soundPlayer) { return nullptr; }

	// 斬撃攻撃をする(キャラごとに違う) 左を向いているか、今何カウントか
	virtual std::vector<Object*>* slashAttack(bool leftDirection, int cnt, bool grand, SoundPlayer* soundPlayer) { return nullptr; }

	// スライディング攻撃
	std::vector<Object*>* slidingAttack(int sound, SoundPlayer* soundPlayer);

	// 射撃攻撃を持っているか
	bool haveBulletAttack() const { return m_attackInfo->bulletDamage() != 0; }
	// 斬撃攻撃を持っているか
	bool haveSlashAttack() const { return m_attackInfo->slashDamage() != 0; }

	// 特定の画像があるか
	bool haveStepGraph() const;
	bool haveSlidingGraph() const;
	bool haveDeadGraph() const;

	// HPが0でやられ画像がなく、画面から消えるべきか
	inline bool noDispForDead() const { return m_hp == 0 && !haveDeadGraph() && m_deadFlag; }

protected:
	void countDrawCnt(){ if (SHAKING_FLAG) { m_drawCnt++; } }

	// 作成した攻撃オブジェクトの最終調整
	void prepareBulletObject(BulletObject* bulletObject);
	void prepareSlashObject(SlashObject* slashObject);

	// キャラから発せられる効果音をキューに入れる
	void pushCharacterSoundQueue(int handle, SoundPlayer* soundPlayer);
};


Character* createCharacter(const char* characterName, int hp = 100, int x = 0, int y = 0, int groupId = 0);


/*
* ハート（主人公）
*/
class Heart :
	public Character
{
protected:
	//// 走りアニメのスピード
	const int RUN_ANIME_SPEED = 6;
	
	//// ジャンプ前アニメのスピード
	const int RUN_PREJUMP_SPEED = 6;

	// 弾の色
	int m_bulletColor;
	
public:
	// コンストラクタ
	Heart(const char* name, int hp, int x, int y, int groupId);
	Heart(const char* name, int hp, int x, int y, int groupId, AttackInfo* attackInfo);

	// デストラクタ
	~Heart();

	Character* createCopy();

	// デバッグ
	void debug(int x, int y, int color) const;

	// 画像変更関数のオーバーライド
	// 立ち画像をセット
	void switchStand(int cnt = 0);

	// しゃがみ画像をセット
	void switchSquat(int cnt = 0);

	// 走り画像をセット
	void switchRun(int cnt = 0);

	// 走り射撃画像をセット
	void switchRunBullet(int cnt = 0);

	// ジャンプ前画像をセット
	void switchPreJump(int cnt = 0);

	// 立ち斬撃画像をセット
	void switchSlash(int cnt = 0);

	// 立ち射撃画像をセット
	void switchBullet(int cnt = 0);

	// 空中射撃画像をセット
	void switchAirBullet(int cnt = 0);

	// 空中斬撃画像をセット
	void switchAirSlash(int cnt = 0);

	// しゃがみ射撃画像をセット
	void switchSquatBullet(int cnt = 0);

	// やられ画像をセット
	void switchDead(int cnt = 0);

	// 射撃攻撃をする
	std::vector<Object*>* bulletAttack(int cnt, int gx, int gy, SoundPlayer* soundPlayer);

	// 斬撃攻撃をする
	std::vector<Object*>* slashAttack(bool leftDirection, int cnt, bool grand, SoundPlayer* soundPlayer);

};


/*
* シエスタ
*/
class Siesta :
	public Heart
{
public:
	// コンストラクタ
	Siesta(const char* name, int hp, int x, int y, int groupId);
	Siesta(const char* name, int hp, int x, int y, int groupId, AttackInfo* attackInfo);

	Character* createCopy();

	// 立ち射撃画像をセット
	void switchBullet(int cnt = 0);

	// しゃがみ射撃画像をセット
	void switchSquatBullet(int cnt = 0);

	// 射撃攻撃をする
	std::vector<Object*>* bulletAttack(int cnt, int gx, int gy, SoundPlayer* soundPlayer);

	// 斬撃攻撃をする
	std::vector<Object*>* slashAttack(bool leftDirection, int cnt, bool grand, SoundPlayer* soundPlayer);
};


/*
* ヒエラルキー
*/
class Hierarchy :
	public Heart
{
public:
	// コンストラクタ
	Hierarchy(const char* name, int hp, int x, int y, int groupId);
	Hierarchy(const char* name, int hp, int x, int y, int groupId, AttackInfo* attackInfo);

	Character* createCopy();

	// 立ち射撃画像をセット
	void switchBullet(int cnt);

	// 射撃攻撃をする
	std::vector<Object*>* bulletAttack(int cnt, int gx, int gy, SoundPlayer* soundPlayer);

	// 斬撃攻撃をする
	std::vector<Object*>* slashAttack(bool leftDirection, int cnt, bool grand, SoundPlayer* soundPlayer);
};


/*
* ヴァルキリア
*/
class Valkyria :
	public Heart
{
public:
	// コンストラクタ
	Valkyria(const char* name, int hp, int x, int y, int groupId);
	Valkyria(const char* name, int hp, int x, int y, int groupId, AttackInfo* attackInfo);

	Character* createCopy();

	// 立ち斬撃画像をセット
	void switchSlash(int cnt = 0);
	// ジャンプ前画像をセット
	void switchPreJump(int cnt = 0);

	// 射撃攻撃をする
	std::vector<Object*>* bulletAttack(int cnt, int gx, int gy, SoundPlayer* soundPlayer){ return nullptr; }

	// 残像を残す斬撃攻撃の関数
	std::vector<Object*>* slashZanzouAttack(bool leftDirection, int cnt, bool grand, SoundPlayer* soundPlayer, GraphHandles* slashGraphHandles);

	// 斬撃攻撃をする
	std::vector<Object*>* slashAttack(bool leftDirection, int cnt, bool grand, SoundPlayer* soundPlayer);
};


/*
* トロイ
*/
class Troy :
	public Heart
{
public:
	// コンストラクタ
	Troy(const char* name, int hp, int x, int y, int groupId);
	Troy(const char* name, int hp, int x, int y, int groupId, AttackInfo* attackInfo);

	Character* createCopy();

	// 走り画像をセット
	void switchRun(int cnt = 0);
	// 走り射撃画像をセット
	void switchRunBullet(int cnt = 0);
	// 上昇画像をセット
	void switchJump(int cnt = 0);
	// 降下画像をセット
	void switchDown(int cnt = 0);
	// 空中射撃画像をセット
	void switchAirBullet(int cnt = 0);

	// 斬撃攻撃をする
	std::vector<Object*>* slashAttack(bool leftDirection, int cnt, bool grand, SoundPlayer* soundPlayer);
};


/*
* コハル
*/
class Koharu :
	public Heart
{
public:
	// コンストラクタ
	Koharu(const char* name, int hp, int x, int y, int groupId);
	Koharu(const char* name, int hp, int x, int y, int groupId, AttackInfo* attackInfo);

	Character* createCopy();

	// 射撃攻撃をする
	std::vector<Object*>* bulletAttack(int cnt, int gx, int gy, SoundPlayer* soundPlayer);

};


/*
* 普通の射撃のみをするキャラ
*/
class BulletOnly :
	public Heart
{
public:
	// コンストラクタ
	BulletOnly(const char* name, int hp, int x, int y, int groupId);
	BulletOnly(const char* name, int hp, int x, int y, int groupId, AttackInfo* attackInfo);

	Character* createCopy();

	// 上昇画像をセット
	void switchJump(int cnt = 0);

	// 斬撃攻撃をする(キャラごとに違う)
	std::vector<Object*>* slashAttack(bool leftDirection, int cnt, bool grand, SoundPlayer* soundPlayer) { return nullptr; }
};


/*
* 普通の斬撃のみをするキャラ
*/
class SlashOnly :
	public Heart
{
public:
	// コンストラクタ
	SlashOnly(const char* name, int hp, int x, int y, int groupId);
	SlashOnly(const char* name, int hp, int x, int y, int groupId, AttackInfo* attackInfo);

	Character* createCopy();

	// 上昇画像をセット
	void switchJump(int cnt = 0);
	// 降下画像をセット
	void switchDown(int cnt = 0);

	// 射撃攻撃をする
	std::vector<Object*>* bulletAttack(int cnt, int gx, int gy, SoundPlayer* soundPlayer) { return nullptr; }
};


/*
* ParabolaBulletのみを撃つキャラ
*/
class ParabolaOnly :
	public Heart
{
public:
	// コンストラクタ
	ParabolaOnly(const char* name, int hp, int x, int y, int groupId);
	ParabolaOnly(const char* name, int hp, int x, int y, int groupId, AttackInfo* attackInfo);

	Character* createCopy();

	// 射撃攻撃をする
	std::vector<Object*>* bulletAttack(int cnt, int gx, int gy, SoundPlayer* soundPlayer);
};


/*
* Boss1: サン
*/
class Sun :
	public Heart
{
public:
	// コンストラクタ
	Sun(const char* name, int hp, int x, int y, int groupId);
	Sun(const char* name, int hp, int x, int y, int groupId, AttackInfo* attackInfo);

	Character* createCopy();

	// ボスの初期アニメーションをセット
	void switchInit(int cnt);

	// 射撃攻撃をする
	std::vector<Object*>* bulletAttack(int cnt, int gx, int gy, SoundPlayer* soundPlayer);

	// 斬撃攻撃をする
	std::vector<Object*>* slashAttack(bool leftDirection, int cnt, bool grand, SoundPlayer* soundPlayer) { return nullptr; }

};


/*
* Boss2: アーカイブ
*/
class Archive :
	public Valkyria
{
public:
	// コンストラクタ
	Archive(const char* name, int hp, int x, int y, int groupId);
	Archive(const char* name, int hp, int x, int y, int groupId, AttackInfo* attackInfo);

	Character* createCopy();

	void switchJump(int cnt);
	void switchSlash(int cnt);

	// ジャンプ前画像をセット
	void switchPreJump(int cnt = 0) { Heart::switchPreJump(); }

	// 斬撃攻撃をする
	std::vector<Object*>* slashAttack(bool leftDirection, int cnt, bool grand, SoundPlayer* soundPlayer);
};


#endif