#ifndef CONTROLLER_RECORDER_H_INCLUDED
#define CONTROLLER_RECORDER_H_INCLUDED


#include <vector>


class CharacterController;



/*
* 操作の記録（レコード）
*/
class ControllerRecord {
private:

	// 入力
	int m_input;

	// 入力開始の時刻
	int m_startTime;

	// 入力された時間の長さ（例えば1Fだけの入力なら1）
	int m_timeLength;

	// 攻撃用
	int m_gx, m_gy;

public:
	ControllerRecord(int startTime, int input);

	// ゲッタ
	int getInput() { return m_input; }
	int getStartTime() { return m_startTime; }
	int getTimeLength() { return m_timeLength; }
	int getGx() { return m_gx; }
	int getGy() { return m_gy; }

	// セッタ
	void setGx(int gx) { m_gx = gx; }
	void setGy(int gy) { m_gy = gy; }

	// 入力の終了時刻（終了時刻ちょうどでは入力なし）
	int getEndTime() { return m_startTime + m_timeLength; }

	// ある時刻での入力チェック
	int checkInput(int time) { 
		if (m_startTime <= time && time < m_startTime + m_timeLength) {
			return m_input;
		}
		return 0;
	}

	// 時間を加算
	void addTime() { m_timeLength++; }

	// 終了時刻をセット（終了時刻ちょうどでは入力なし）
	void setEndTime(int time);
};



/*
* CharacterControllerによる操作を記録するクラス。
* 過去の動きの再現に使える。
*/
class ControllerRecorder {
private:

	// これ以降はレコードがないことを表すRecordの時間
	const int END_RECORD_TIME = -1;

	// 体内時計
	int m_time;

	// 現在見ているvectorのインデックス
	int m_index;

	// レコード（記録）
	std::vector<ControllerRecord*> m_records;

public:
	ControllerRecorder(int startTime);
	~ControllerRecorder();

	// 時間を最初に戻す
	void init();

	// 時間を進める
	void addTime();

	// レコードの存在チェック
	bool existRecord();

	// 入力を取得
	int checkInput();

	// 操作のレコードを追記or追加する（1F分）
	void writeRecord(int input);

	// time以降のレコードを削除する
	void discardRecord();

	// 攻撃目標を設定
	void setGoal(int gx, int gy);

	// 攻撃目標を取得
	void getGoal(int& gx, int& gy);
};


#endif