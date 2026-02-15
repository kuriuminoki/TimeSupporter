#ifndef SOUND_H_INCLUDED
#define SOUND_H_INCLUDED

#include <queue>
#include <string>

// カメラのパンを調整する用
int adjustPanSound(int x, int cameraX);

class SoundPlayer {
private:
	// 座標 パンの設定に使う
	int m_cameraX;

	// 音量
	int m_volume;

	// BGMの名前
	std::string m_bgmName;

	// BGM
	int m_bgmHandle;

	// 再生予定の効果音
	std::queue<std::pair<int, int> > m_soundQueue;

public:
	SoundPlayer();
	~SoundPlayer();

	// ゲッタ
	inline int getVolume() const { return m_volume; }
	inline int getCameraX() const { return m_cameraX; }
	const char* getBgmName() const { return m_bgmName.c_str(); }

	// セッタ
	void setVolume(int volume);
	inline void setCameraX(int cameraX) { m_cameraX = cameraX; }

	// BGMをセット（変更）
	void setBGM(std::string bgmName);

	// 今流している音楽をチェックする
	inline bool sameBGM(std::string bgmName) const { return m_bgmName == bgmName; }

	// BGMを再生
	void playBGM();

	// BGMをストップ
	void stopBGM();

	// BGMが再生していないか調べる
	int checkBGMplay();

	// 効果音の再生待機列へプッシュ
	void pushSoundQueue(int soundHandle, int panPal = 0);

	// 効果音の再生待機列をクリア
	void clearSoundQueue();

	// 効果音を鳴らす
	void play();
};

#endif