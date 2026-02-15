#include "Sound.h"
#include "Define.h"
#include "DxLib.h"
#include <algorithm>

using namespace std;


// 音量調節
void changeSoundVolume(int volume, int soundHandle) {
	ChangeVolumeSoundMem(255 * (volume) / 100, soundHandle);
}

// カメラのパンを調整する。
int adjustPanSound(int x, int cameraX) {
	int d = x - cameraX;
	int panPal = 0;
	if (d > 0) { // 右で音が鳴る
		panPal = 255 * d / (GAME_WIDE / 2);
		panPal = min(panPal, 255);
	}
	else {
		panPal = 255 * d / (GAME_WIDE / 2);
		panPal = max(panPal, -255);
	}
	return panPal;
}


SoundPlayer::SoundPlayer() {
	m_volume = 50;
	m_bgmName = "";
	m_bgmHandle = -1;
	m_cameraX = 0;
}

SoundPlayer::~SoundPlayer() {
	DeleteSoundMem(m_bgmHandle);
}

void SoundPlayer::setVolume(int volume) {
	if (volume < 0) { m_volume = 0; }
	else {
		m_volume = min(volume, 100);
	}
	changeSoundVolume(m_volume, m_bgmHandle);
}

// BGMをセット（変更）
void SoundPlayer::setBGM(std::string bgmName) {
	if (bgmName == m_bgmName || bgmName == "" || bgmName == "null") { return; }
	DeleteSoundMem(m_bgmHandle);
	m_bgmName = bgmName;
	m_bgmHandle = LoadSoundMem(bgmName.c_str());
	// 音量調整
	changeSoundVolume(m_volume, m_bgmHandle);
}

// BGMを再生
void SoundPlayer::playBGM() {
	if (!checkBGMplay()) {
		PlaySoundMem(m_bgmHandle, DX_PLAYTYPE_LOOP, FALSE);
	}
}

// BGMをストップ
void SoundPlayer::stopBGM() {
	StopSoundMem(m_bgmHandle);
}

// BGMが再生していないか調べる
int SoundPlayer::checkBGMplay() {
	return CheckSoundMem(m_bgmHandle);
}

// 効果音の再生待機列へプッシュ
void SoundPlayer::pushSoundQueue(int soundHandle, int panPal) {
	m_soundQueue.push(make_pair(soundHandle, panPal));
}

// 効果音の再生待機列をクリア
void SoundPlayer::clearSoundQueue() {
	queue<pair<int, int> >().swap(m_soundQueue);
}

// 効果音を鳴らす
void SoundPlayer::play() {
	while (!m_soundQueue.empty()) {
		int handle = m_soundQueue.front().first;
		changeSoundVolume(m_volume, handle);
		ChangeNextPlayPanSoundMem(m_soundQueue.front().second, handle);
		PlaySoundMem(handle, DX_PLAYTYPE_BACK);
		m_soundQueue.pop();
	}
}