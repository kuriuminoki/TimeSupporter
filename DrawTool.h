#ifndef DRAW_TOOL_H_INCLUDED
#define DRAW_TOL_H_INCLUDED

class KeyConfig;

// Zキー長押しでスキップの表示
void drawSkip(int cnt, double exX, double exY, int fontHandle, const KeyConfig* keyConfig_p);

#endif