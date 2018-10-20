#include <windows.h>  
#include <stdlib.h>  
#include <tchar.h>

#pragma comment(lib,"Msimg32.lib")

HDC hdc, memBit; // контексты устройств
int windowWidth = 1000, windowHeight = 500;
bool isMoved = false, isRectangle = true;
int x = 5, y = 5; // координаты верхнего угла спрата
int mouseX, mouseY; //координаты мыши
int deltaX = 0, deltaY = 0;
int initialSide = 50, side = 50, step = 5;
int wheelNum;

PAINTSTRUCT ps;
HBRUSH backgroundBrush, spriteBrush;
RECT backgroundRect, spriteRect;
HMENU menu, popupMenu;
HBITMAP hBitMap; 
BITMAP bitMap;
  
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

bool isOnSprite()
{
	return  ((mouseX >= x) && (mouseX <= x + side) && (mouseY >= y) && (mouseY <= y + side));
}

bool isOutsideWindow()
{
	return ((x <= 0) || (x + side >= windowWidth) || (y <= 0) || (y + side >= windowHeight));
}

void CreateMenu(HWND hWnd)
{
	menu = CreateMenu();
	popupMenu = CreatePopupMenu();
	AppendMenu(menu, MF_POPUP, (UINT_PTR)popupMenu, _T("Change Sprite"));
	AppendMenu(popupMenu, MF_STRING, 1, _T("Rectangle"));
	AppendMenu(popupMenu, MF_STRING, 2, _T("Windows Logo"));
	SetMenu(hWnd, menu);
}

void MakeRebound(HWND hWnd)
{
	isMoved = false;
	if (x <= 0)
	{
		x += 45;
	}
	else
	{
		if (x+side >= windowWidth)
		{
			x -= 45;
		}
		else
		{
			if (y <= 0)
			{
				y += 45;
			}
			else y -= 45;
		}
	}
	InvalidateRect(hWnd, NULL, TRUE);
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{ 
	case WM_CREATE:
		CreateMenu(hWnd);
		backgroundBrush = CreateSolidBrush(RGB(240, 250, 235));
		spriteBrush = CreateSolidBrush(RGB(62, 98, 94));
		hBitMap = (HBITMAP)LoadImage(NULL, _T("C:\\Users\\admin\\source\\repos\\OSISPFirstLab\\Windows-Logo.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		if (hBitMap == NULL)
			MessageBox(hWnd, _T("Not Found"), _T("Error"), MB_OK);
		GetObject(hBitMap, sizeof(bitMap), &bitMap);
		memBit = CreateCompatibleDC(hdc); // создание совместимого контекста устройств
		SelectObject(memBit, hBitMap);		// выбирает объект в заданный DC	
		break;
	case WM_SIZE:
		windowHeight = HIWORD(lParam);
		windowWidth = LOWORD(lParam);
		break;
	case WM_COMMAND:
		switch (wParam)
		{
		case 1: 
			isRectangle = true;
			break;
		case 2:
			isRectangle = false;
			break;
		}
		InvalidateRect(hWnd, NULL, TRUE);
		break;
	case WM_DESTROY:
		DeleteObject(backgroundBrush);
		DeleteObject(spriteBrush);
		DeleteObject(hBitMap);
		PostQuitMessage(0);
		break;
	case WM_PAINT: 
		hdc = BeginPaint(hWnd, &ps);
		SetRect(&backgroundRect, 0, 0, windowWidth, windowHeight);
		FillRect(hdc, &backgroundRect, backgroundBrush);
		if (isRectangle)
		{
			side = initialSide;
			SetRect(&spriteRect, x, y, x + side, y + side);
			FillRect(hdc, &spriteRect, spriteBrush);
		}
		else
		{
			TransparentBlt(hdc, x, y, bitMap.bmWidth, bitMap.bmHeight, memBit, 0, 0, bitMap.bmWidth, bitMap.bmHeight, RGB(255, 255, 255));
			side = bitMap.bmHeight;
		}
		EndPaint(hWnd, &ps);
		break;
	
	case WM_MOUSEWHEEL:
	{
		wheelNum = GET_WHEEL_DELTA_WPARAM(wParam); // макрос
		if (GetAsyncKeyState(VK_SHIFT))
		{
			if (wheelNum > 0) // left
				x -= step;
			else x += step; // right
		}
		else
		{
			if (wheelNum > 0) // up
				y -= step;
			else y += step; // down
		}
		InvalidateRect(hWnd, NULL, TRUE);
	}
	break;
	case WM_LBUTTONDOWN: 
		mouseX = LOWORD(lParam);
		mouseY = HIWORD(lParam);
		if (isOnSprite())
		{
			isMoved = true;
			deltaX = mouseX - x;
			deltaY = mouseY - y;
		}
		break;
	case WM_LBUTTONUP:
		isMoved = false;
		break;
	case WM_MOUSEMOVE: 
		if (isMoved)
		{
			mouseX = LOWORD(lParam);
			mouseY = HIWORD(lParam);
			x = mouseX - deltaX;
			y = mouseY - deltaY;
			InvalidateRect(hWnd, NULL, TRUE);
		}
		break;
	case WM_KEYDOWN:
	{
		switch (wParam)
		{
		case VK_LEFT:
			x -= step;
			InvalidateRect(hWnd, NULL, TRUE);
			break;
		case VK_RIGHT:
			x += step;
			InvalidateRect(hWnd, NULL, TRUE);
			break;
		case VK_UP:
			y -= step;
			InvalidateRect(hWnd, NULL, TRUE);
			break;

		case VK_DOWN:
			y += step;
			InvalidateRect(hWnd, NULL, TRUE);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
			break;
		}
	}
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}

	if (isOutsideWindow())
		MakeRebound(hWnd);
	return 0;
}
