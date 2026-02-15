#include "Define.h"
#include "Timer.h"


Timer::Timer() { 
	resetTime();
}

int Timer::getSec() const {
	return m_time / FPS_N;
}
