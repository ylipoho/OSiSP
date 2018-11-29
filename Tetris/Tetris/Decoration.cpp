#include <windows.h>
#include <stdlib.h> 
#include <stdio.h>

#include "Decoration.h"

HMENU menu, hPopupMenu;
#define FONT_NAME L"Times New Roman"

void CreateMenu(HWND hWnd)	// создание меню
{
	HMENU menu = CreateMenu();
	HMENU hPopupMenu = CreatePopupMenu();
	AppendMenu(menu, MF_POPUP, (UINT_PTR)hPopupMenu, L"New game");
	AppendMenu(hPopupMenu, MF_STRING, 11, L"Easy");
	AppendMenu(hPopupMenu, MF_STRING, 12, L"Hard");

	AppendMenu(menu, MF_STRING, 2, L"Music");

	AppendMenu(menu, MF_STRING, 3, L"Records");

	SetMenu(hWnd, menu);
}


HFONT InitializeSmallFont(LOGFONT logFont)
{
	logFont.lfHeight = 40;
	logFont.lfWidth = 0;
	logFont.lfEscapement = 0;
	logFont.lfOrientation = 0;
	logFont.lfWeight = FW_BOLD;
	logFont.lfItalic = 0;
	logFont.lfUnderline = 0;
	logFont.lfStrikeOut = 0;
	logFont.lfCharSet = ANSI_CHARSET;
	logFont.lfOutPrecision = OUT_DEFAULT_PRECIS;
	logFont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	logFont.lfQuality = PROOF_QUALITY;
	logFont.lfPitchAndFamily = VARIABLE_PITCH | FF_MODERN;
	wcscpy_s(logFont.lfFaceName, FONT_NAME);
	HFONT hFont = CreateFontIndirect(&logFont);
	return hFont;
}

HFONT InitializeBigFont(LOGFONT logFont)
{
	logFont.lfHeight = 70;
	logFont.lfWidth = 0;
	logFont.lfEscapement = 0;
	logFont.lfOrientation = 0;
	logFont.lfWeight = FW_BOLD;
	logFont.lfItalic = 0;
	logFont.lfUnderline = 0;
	logFont.lfStrikeOut = 0;
	logFont.lfCharSet = ANSI_CHARSET;
	logFont.lfOutPrecision = OUT_DEFAULT_PRECIS;
	logFont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	logFont.lfQuality = PROOF_QUALITY;
	logFont.lfPitchAndFamily = VARIABLE_PITCH | FF_MODERN;
	wcscpy_s(logFont.lfFaceName, FONT_NAME);
	HFONT hFont = CreateFontIndirect(&logFont);
	return hFont;
}

COLORREF ChooseBrushColor(int number)
{
	COLORREF color;
	switch (number)
	{
	case 0:
		color = I_COLOR;
		break;
	case 1:
		color = J_COLOR;
		break;
	case 2:
		color = L_COLOR;
		break;
	case 3:
		color = O_COLOR;
		break;
	case 4:
		color = S_COLOR;
		break;
	case 5:
		color = T_COLOR;
		break;
	case 6:
		color = Z_COLOR;
		break;
	}
	return color;
}
