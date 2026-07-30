#include "Define.h"
#include "Control.h"
#include <sstream>

using namespace std;

void drawSkip(int cnt, double exX, double exY, int fontHandle, const KeyConfig* keyConfig_p) {
	static const int SKIP_WIDE = 750;
	static const int SKIP_HEIGHT = 100;
	if (cnt > 0) {
		int x2 = GAME_WIDE - 10;
		int x1 = x2 - (int)(SKIP_WIDE * exX);
		int x15 = (int)(x1 + cnt * (SKIP_WIDE / 60) * exX);
		int y1 = 10;
		int y2 = y1 + (int)(SKIP_HEIGHT * exY);
		DrawBox(x1, y1, x15, y2, BLACK, TRUE);
		DrawBox(x15, y1, x2, y2, GRAY, TRUE);
		ostringstream oss;
		oss << keyConfig_p->getSkipKeyName() << "長押しでスキップ";
		DrawStringToHandle(x1 + 5, y1 + 5, oss.str().c_str(), WHITE, fontHandle);
	}
}