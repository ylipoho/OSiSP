#pragma once

#define I_COLOR RGB(255, 98, 0)
#define J_COLOR RGB(166, 63, 0)
#define L_COLOR RGB(255, 159, 0)
#define O_COLOR RGB(15, 79, 168)
#define S_COLOR RGB(67, 128, 211)
#define T_COLOR RGB(0, 107, 83)
#define Z_COLOR RGB(0, 164, 128)

void CreateMenu(HWND hWnd);
HFONT InitializeSmallFont(LOGFONT logFont);
HFONT InitializeBigFont(LOGFONT logFont);
COLORREF ChooseBrushColor(int number);