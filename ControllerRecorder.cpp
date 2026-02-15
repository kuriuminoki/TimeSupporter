#include "ControllerRecorder.h"
#include <algorithm>


using namespace std;


/*
* レコード
*/
ControllerRecord::ControllerRecord(int startTime, int input) {
	m_startTime = startTime;
	m_timeLength = 1;
	m_input = input;
	m_gx = 0; m_gy = 0;
}


// 終了時刻をセット（終了時刻ちょうどでは入力なし）
void ControllerRecord::setEndTime(int time) {
	if (time > m_startTime) {
		m_timeLength = time - m_startTime;
	}
	else {
		m_startTime = time;
		m_timeLength = 0;
	}
}



/*
* レコーダー
*/
ControllerRecorder::ControllerRecorder(int startTime) {
	m_time = startTime;
	m_index = 0;
	if (m_time > 0) {
		ControllerRecord* r = new ControllerRecord(0, false);
		r->setEndTime(startTime);
		m_records.push_back(r);
		m_index++;
	}
}


// デストラクタはレコードを全削除
ControllerRecorder::~ControllerRecorder() {
	for (unsigned int i = 0; i < m_records.size(); i++) {
		delete m_records[i];
	}
}


// 時間を進める
void ControllerRecorder::addTime() {
	m_time++;
	if (m_records.size() > 0 && m_index < m_records.size() && m_records[m_index]->getEndTime() <= m_time) {
		m_index = min((int)m_records.size() + 1, m_index + 1);
	}
}


// 時間を最初に戻す
void ControllerRecorder::init() {
	m_time = 0;
	m_index = 0;
}


// レコードの存在チェック
bool ControllerRecorder::existRecord() {
	if (m_records.size() == 0) {
		return false;
	}
	// 最新レコードの終了時刻と現在時刻を比較
	return m_records.back()->getEndTime() > m_time;
}


// 入力を取得
int ControllerRecorder::checkInput() {
	if (!existRecord()) { return false; }
	return m_records[m_index]->checkInput(m_time);
}


// 操作のレコードを追記or追加する（1F分）
void ControllerRecorder::writeRecord(int input) {
	if (m_records.size() != 0 && m_records.back()->getInput() == input) {
		m_records.back()->addTime();
	}
	else {
		m_records.push_back(new ControllerRecord(m_time, input));
		m_index++;
	}
}


// m_time以降のレコードを削除する
void ControllerRecorder::discardRecord() {
	while (m_records.size() > 0 && m_records.back()->getStartTime() > m_time) {
		delete m_records.back();
		m_records.pop_back();
	}
	if (m_records.size() > 0) {
		m_records.back()->setEndTime(m_time);
	}
}

// 攻撃目標を設定
void ControllerRecorder::setGoal(int gx, int gy) {
	if (!existRecord()) { return; }
	m_records.back()->setGx(gx);
	m_records.back()->setGy(gy);
}

// 攻撃目標を取得
void ControllerRecorder::getGoal(int& gx, int& gy) {
	if (!existRecord()) { return; }
	gx = m_records[m_index]->getGx();
	gy = m_records[m_index]->getGy();
}