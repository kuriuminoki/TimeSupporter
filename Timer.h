#ifndef TIMER_H_INCLUDED
#define TIMER_H_INCLUDED

class Timer {
private:

	const int START_TIME = 0;

	int m_time;

public:

	Timer();

	inline int getTime() const { return m_time; }

	inline void setTime(int time) { m_time = time; }

	inline void advanceTime(int speed) { m_time += speed; }

	inline void resetTime() { m_time = START_TIME; }

	// ïbêîÇ≈ï‘Ç∑
	int getSec() const;
};

#endif