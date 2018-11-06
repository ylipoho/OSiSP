#include <windows.h>  
#include <stdlib.h>  
#include <tchar.h>
#include <fstream>

#define ROWS 10
#define COLUMNS 10
#define MAX_ROWS 20
#define MAX_COLUMNS 20
#define MIN_ROWS 2
#define MIN_COLUMNS 2
#define FONT_NAME L"Times New Roman"

HDC hdc; 
int windowWidth = 1000, windowHeight = 500;
int columns = COLUMNS, rows = ROWS;
int mouseX, mouseY;

PAINTSTRUCT ps; 
HBRUSH backgroundBrush, lineBrush;
COLORREF backgroundColor = RGB(241, 230, 220);
COLORREF lineColor = RGB(166, 62, 20);
RECT backgroundRect, partRect;
HMENU menu, hPopupMenu1, hPopupMenu2;
FILE *file;


int table[ROWS][COLUMNS];
LOGFONT logFont;
TEXTMETRIC textMetric;
HFONT hFont;  

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int CALLBACK WinMain(
	_In_ HINSTANCE hInstance,
	_In_ HINSTANCE hPrevInstance,
	_In_ LPSTR     lpCmdLine,
	_In_ int       nCmdShow
)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = _T("WindowClass");
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));

	RegisterClassEx(&wcex);

	HWND hWnd = CreateWindow(
		_T("WindowClass"),
		_T("Window Application"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		HWND_DESKTOP,
		NULL,
		hInstance,
		NULL
	);

	if (!hWnd)
	{
		MessageBox(NULL,
			_T("Call to CreateWindow failed!"),
			_T("Win32 Guided Tour"),
			NULL);

		return 1;
	}
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}

void CreateMenu(HWND hWnd)        
{
	menu = CreateMenu();
	hPopupMenu1 = CreatePopupMenu(); 
	hPopupMenu2 = CreatePopupMenu();
	AppendMenu(menu, MF_POPUP, (UINT_PTR)hPopupMenu1, _T("Row"));
	AppendMenu(hPopupMenu1, MF_STRING, 11, _T("Add"));
	AppendMenu(hPopupMenu1, MF_STRING, 12, _T("Delete"));

	AppendMenu(menu, MF_POPUP, (UINT_PTR)hPopupMenu2, _T("Column"));
	AppendMenu(hPopupMenu2, MF_STRING, 21, _T("Add"));
	AppendMenu(hPopupMenu2, MF_STRING, 22, _T("Delete"));
	SetMenu(hWnd, menu);
}

BOOL ReadFile()
{
	std::ifstream in("C:\\Users\\admin\\source\\repos\\OSISPSecondLab\\NumsTable.txt");
	if (!in.is_open())
		return false;
	for (int i = 0; i < ROWS; i++)
		for (int j = 0; j < COLUMNS; j++)
			in >> table[i][j];
	in.close();
	return true;
}

VOID InitializeFont()
{
	logFont.lfHeight = 20; 
	logFont.lfWidth = 0; 
	logFont.lfEscapement = 0;
	logFont.lfOrientation = 0; 
	logFont.lfWeight = FW_NORMAL;
	logFont.lfItalic = 0; 
	logFont.lfUnderline = 0; 
	logFont.lfStrikeOut = 0; 
	logFont.lfCharSet = ANSI_CHARSET;
	logFont.lfOutPrecision = OUT_DEFAULT_PRECIS;
	logFont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	logFont.lfQuality = PROOF_QUALITY;
	logFont.lfPitchAndFamily = VARIABLE_PITCH | FF_MODERN; 
	wcscpy_s(logFont.lfFaceName, FONT_NAME);
	hFont = CreateFontIndirect(&logFont);
}
// DONE ==========================================================^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam; // сбщ, посылаемое окну, когда размер или позиция окнасобираются измениться
	// Значение lParam. Указывает на структуру MINMAXINFO, которая содержит значения позиции и габариты разворачивания 
	//по умолчанию и устанавливаемые по умолчанию минимальные и максимальные размеры. Прикладная программа может отменить 
	//значения параметров по умолчанию элементовов этой структуры.

	switch (message)
	{
	case WM_CREATE:
		CreateMenu(hWnd);
		InitializeFont();
		ReadFile();
		backgroundBrush = CreateSolidBrush(RGB(240, 250, 235));
		lineBrush = CreateSolidBrush(RGB(62, 98, 94));
		
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		wchar_t buf[10];
		swprintf_s(buf, L"%d", table[1][2]);
		TextOut(hdc, 10, 10, buf, _tcsclen(buf));
		EndPaint(hWnd, &ps);
		break;
	case WM_SIZE:
		windowHeight = HIWORD(lParam);
		windowWidth = LOWORD(lParam);
		break;
	case WM_GETMINMAXINFO:
		break;
	case WM_VSCROLL:
		break;
	case WM_COMMAND: // DONE ==========================================================vvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
		switch (wParam)
		{
		case 11:
			if (rows<MAX_ROWS)
				rows++;
			break;
		case 12:
			if (rows>MIN_ROWS)
				rows--;
			break;
		case 21:
			if (columns<MAX_COLUMNS)
				columns++;
			break;
		case 22:
			if (columns>MIN_COLUMNS)
				columns--;
			break;
		}
		InvalidateRect(hWnd, NULL, TRUE);
		break;
	case WM_DESTROY:
		DeleteObject(backgroundBrush);
		DeleteObject(lineBrush);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}
	return 0;
}


