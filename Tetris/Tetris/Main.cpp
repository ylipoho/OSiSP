#include <windows.h>
#include <stdlib.h> 
#include <stdio.h>
#include <tchar.h>
#include <string>
#include <ctime>
#include "MMSystem.h"
#include "ConstAndTypes.h"
#include "Decoration.h"
#include "FileWork.h"


HDC hdc, hdcMem; 
HBITMAP hbm;
BITMAP bm;
PAINTSTRUCT ps;
LOGFONT logsFont, logbFont;
HFONT hsFont, hbFont;
HBRUSH backgroundBrush, colorBrush, greyBrush;
RECT gameRect, scoreRect, nextRect;

bool isFirstLaunch = true; 
bool isGame = false; // true - идет игра, false - проигрыш
bool isPause = false; // true - пауза
bool isMusic = true; // true - если музыка играет
bool isEasy = true; // true - ускорения нет, false - есть
bool isNewStep = false; // true - начало нового хода
int score = 0; // счет
int speed = 1500;	// скорость падения
int easyRecord, hardRecord;

int table[14][10];

BLOCK currentBlock, nextBlock; 

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
	wcex.hbrBackground = CreateSolidBrush(BACKGROUND_COLOR);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = _T("WindowClass");
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));

	RegisterClassEx(&wcex);

	HWND hWnd = CreateWindow(
		_T("WindowClass"),
		_T("TETRIS 2000"),
		WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZE,
		(GetSystemMetrics(SM_CXSCREEN) - WINDOW_WIDTH) / 2,
		(GetSystemMetrics(SM_CYSCREEN) - WINDOW_HEIGHT) / 2,
		WINDOW_WIDTH, WINDOW_HEIGHT,
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

	KillTimer(hWnd, 1);

	return 0;
}




void CleanTable()
{
	for (int i = 0; i < TABLE_HEIGHT; i++)
		for (int j = 0; j < TABLE_WIDTH; j++)
			table[i][j] = -1;
}

VOID FirstLaunch(HWND hWnd) // создать все необходимое: поле, оформление итд
{
	if (isMusic)
		PlaySound(TEXT("C:\\Users\\admin\\source\\repos\\Tetris\\music.wav"), NULL, SND_LOOP | SND_ASYNC);

	SetRect(&gameRect, INDENT, INDENT, INDENT+GAME_WIDTH, INDENT+GAME_HEIGHT); // задать игровое поле
	SetRect(&scoreRect, SCORE_X, SCORE_Y, WINDOW_WIDTH-INDENT, SCORE_Y+100); // задать поле для очков
	SetRect(&nextRect, NEXT_X, NEXT_Y, NEXT_X+200, NEXT_Y+200); // задать поле для вывода след блока

	backgroundBrush = CreateSolidBrush(BACKGROUND_COLOR);
	colorBrush = CreateSolidBrush(GREY_COLOR);
	greyBrush = CreateSolidBrush(GREY_COLOR);
	CleanTable();
}


int FindTheMostLeftX(BLOCK currentBlock)
{
	int min = 15;
	for (int i = 0; i < 4; i++)
	{
		int x = currentBlock.coord[i][1];
		min = min(x, min);
	}
	return min;
}

int FindTheMostRightX(BLOCK currentBlock)
{
	int max = -15;
	for (int i = 0; i < 4; i++)
	{
		int x = currentBlock.coord[i][1];
		max = max(x, max);
	}
	return max;
}

int findTheMostDownY(BLOCK currentBlock)
{
	int max = -5;
	for (int i = 0; i < 4; i++)
	{
		int y = currentBlock.coord[i][0];
		max = max(y, max);
	}
	return max;
}

bool CheckRightPosition(BLOCK b) // true - блоки для перемещения вправо свободны
{
	if (FindTheMostRightX(b) >= 9)
		return false;

	for (int i = 0; i < 4; i++)
	{
		int currentY = b.coord[i][0];
		int currentX = b.coord[i][1];

		if (table[currentY][currentX + 1] != -1)
			return false;
	}
	return true;
}

bool CheckLeftPosition(BLOCK b) // true - блоки для перемещения влево свободны
{
	if (FindTheMostLeftX(b) <= 0)
		return false;

	for (int i = 0; i < 4; i++)
	{
		int currentY = b.coord[i][0];
		int currentX = b.coord[i][1];

		if (table[currentY][currentX - 1] != -1)
			return false;
	}
	return true;
}

bool CheckDownPosition(BLOCK b) // true - блоки для перемещения вниз свободны
{
	if (findTheMostDownY(b) >= 13)
		return false;
	for (int i = 0; i < 4; i++)
	{
		int currentY = b.coord[i][0];
		int currentX = b.coord[i][1];

		if (table[currentY + 1][currentX] != -1)
			return false;
	}
	return true;
}

void PrintNextBlock(HWND hWnd) // рисует следующий элемент в области nextRect
{
	HBRUSH brush = CreateSolidBrush(ChooseBrushColor(nextBlock.type)); //ChooseBrushColor(nextBlock.type);
	for (int i = 0; i < 4; i++)  // нарисовать каждый квадратик 
	{
		int deltaX = NEXT_X + (nextBlock.coord[i][1] - nextBlock.x) * BLOCK_SIDE; // смещение по иксу в пикселях
		int deltaY = NEXT_Y + (nextBlock.coord[i][0] - nextBlock.y) * BLOCK_SIDE; // смещение по игрику в пикселях

		RECT r;
		SetRect(&r, deltaX, deltaY, deltaX + BLOCK_SIDE, deltaY + BLOCK_SIDE);
		FillRect(hdcMem, &r, brush);
	}
	DeleteObject(brush);
}


void PrintCurrentBlock(HWND hWnd) // рисует следующий элемент в области nextRect
{
	for (int i = 0; i < 4; i++)  // нарисовать каждый квадратик 
	{
		int deltaX = INDENT + currentBlock.coord[i][1] * BLOCK_SIDE; // смещение по иксу в пикселях
		int deltaY = INDENT + currentBlock.coord[i][0] * BLOCK_SIDE; // смещение по игрику в пикселях

		RECT r;
		SetRect(&r, deltaX, deltaY, deltaX + BLOCK_SIDE, deltaY + BLOCK_SIDE);
		HBRUSH brush = CreateSolidBrush(ChooseBrushColor(currentBlock.type));
		FillRect(hdcMem, &r, brush);
		DeleteObject(brush);
	}
}


void CalculateCoordinates(BLOCK *bl) // вычисляет занимаемые в данный момент координаты 
{ 
	BLOCK b;
	BlockType bt = bl->type;
	b.x = bl->x;
	b.y = bl->y;

	switch (bl->type)
	{
	case I:
		if (bl->rotate % 2 == 0) // ====
		{
			b.coord[0][1] = b.x;
			b.coord[1][1] = b.x + 1;
			b.coord[2][1] = b.x + 2;
			b.coord[3][1] = b.x + 3;
			b.coord[0][0] = b.coord[1][0] = b.coord[2][0] = b.coord[3][0] = b.y;
		}
		else // вертикально расположен
		{
			b.coord[0][1] = b.coord[1][1] = b.coord[2][1] = b.coord[3][1] = b.x;
			b.coord[0][0] = b.y;
			b.coord[1][0] = b.y + 1;
			b.coord[2][0] = b.y + 2;
			b.coord[3][0] = b.y + 3;
		}
		break;
	case J:
		switch (bl->rotate)
		{
		case 0: // J
			b.coord[0][1] = b.coord[1][1] = b.coord[2][1] = b.x + 1;
			b.coord[0][0] = b.y;
			b.coord[1][0] = b.y + 1;
			b.coord[2][0] = b.coord[3][0] = b.y + 2;
			b.coord[3][1] = b.x;
			break;
		case 1: //  L__
			b.coord[0][0] = b.y;
			b.coord[1][0] = b.coord[2][0] = b.coord[3][0] = b.y + 1;
			b.coord[0][1] = b.coord[1][1] = b.x;
			b.coord[2][1] = b.x + 1;
			b.coord[3][1] = b.x + 2;  
			break;
		case 2: //Г
			b.coord[0][1] = b.coord[2][1] = b.coord[3][1] = b.x;
			b.coord[0][0] = b.coord[1][0] = b.y;
			b.coord[1][1] = b.x + 1;
			b.coord[2][0] = b.y + 1;
			b.coord[3][0] = b.y + 2;
			break;
		case 3: // ---|  как приставка
			b.coord[0][1] = b.x;
			b.coord[0][0] = b.coord[1][0] = b.coord[2][0] = b.y;
			b.coord[1][1] = b.x + 1;
			b.coord[2][1] = b.coord[3][1] = b.x + 2;
			b.coord[3][0] = b.y + 1;
			break;
		}
		break;
	case L:
		switch (bl->rotate)
		{
		case 0: // L
			b.coord[0][1] = b.coord[1][1] = b.coord[2][1] = b.x;
			b.coord[0][0] = b.y;
			b.coord[1][0] = b.y + 1;
			b.coord[2][0] = b.coord[3][0] = b.y + 2;
			b.coord[3][1] = b.x + 1; 
			break;
		case 1: //  /-- как постфикс
			b.coord[3][0] = b.y + 1;
			b.coord[0][0] = b.coord[1][0] = b.coord[2][0] = b.y;
			b.coord[1][1] = b.x + 1;
			b.coord[2][1] = b.x + 2;
			b.coord[3][1] = b.coord[0][1] = b.x;
			break;
		case 2: //Г в другую сторону
			b.coord[1][1] = b.coord[2][1] = b.coord[3][1] = b.x + 1;
			b.coord[0][1] = b.x;
			b.coord[0][0] = b.coord[1][0] = b.y;
			b.coord[2][0] = b.y + 1;
			b.coord[3][0] = b.y + 2;
			break;
		case 3: //
			b.coord[3][0] = b.y;
			b.coord[0][0] = b.coord[1][0] = b.coord[2][0] = b.y + 1;
			b.coord[3][1] = b.coord[0][1] = b.x;
			b.coord[1][1] = b.x + 1;
			b.coord[2][1] = b.coord[3][1] = b.x + 2;
			break;
		}
		break;
	case O:  // только одна позиция, не важно какая
		b.coord[0][1] = b.coord[2][1] = b.x;
		b.coord[1][1] = b.coord[3][1] = b.x + 1;
		b.coord[0][0] = b.coord[1][0] = b.y;
		b.coord[2][0] = b.coord[3][0] = b.y + 1;
		break;
	case S:
		if (bl->rotate % 2 == 0) // типа s
		{
			b.coord[0][1] = b.x;
			b.coord[1][1] = b.coord[2][1] = b.x + 1;
			b.coord[3][1] = b.x + 2;
			b.coord[0][0] = b.coord[1][0] = b.y + 1;
			b.coord[2][0] = b.coord[3][0] = b.y;
		}
		else //как молния s
		{
			b.coord[0][0] = b.y;
			b.coord[1][0] = b.coord[2][0] = b.y + 1;
			b.coord[3][0] = b.y + 2;
			b.coord[0][1] = b.coord[1][1] = b.x;
			b.coord[2][1] = b.coord[3][1] = b.x + 1;
		}
		break;
	case T:
		switch (bl->rotate)
		{
		case 0: // T
			b.coord[0][0] = b.coord[1][0] = b.coord[2][0] = b.y;
			b.coord[3][0] = b.y + 1;
			b.coord[0][1] = b.x;
			b.coord[1][1] = b.coord[3][1] = b.x + 1;
			b.coord[2][1] = b.x + 2;
			break;
		case 1: //  хвост влево 
			b.coord[3][1] = b.x;
			b.coord[0][1] = b.coord[1][1] = b.coord[2][1] = b.x + 1;
			b.coord[0][0] = b.y;
			b.coord[1][0] = b.coord[3][0] = b.y +1;
			b.coord[2][0] = b.y + 2;    
			break;
		case 2: //Г
			b.coord[0][0] = b.coord[1][0] = b.coord[2][0] = b.y + 1;
			b.coord[3][0] = b.y;
			b.coord[0][1] = b.x;
			b.coord[1][1] = b.coord[3][1] = b.x + 1;
			b.coord[2][1] = b.x + 2;			
			break;
		case 3: // ---|  как приставка
			b.coord[3][1] = b.x + 1;
			b.coord[0][1] = b.coord[1][1] = b.coord[2][1] = b.x;
			b.coord[0][0] = b.y;
			b.coord[1][0] = b.coord[3][0] = b.y + 1;
			b.coord[2][0] = b.y + 2;
			break;
		}
		break;
	case Z:
		if (bl->rotate % 2 == 0) // типа z
		{
			b.coord[0][1] = b.x;
			b.coord[1][1] = b.coord[2][1] = b.x + 1;
			b.coord[3][1] = b.x + 2;
			b.coord[0][0] = b.coord[1][0] = b.y;
			b.coord[2][0] = b.coord[3][0] = b.y + 1;
		}
		else //как молния z
		{
			b.coord[0][0] = b.y;
			b.coord[1][0] = b.coord[2][0] = b.y + 1;
			b.coord[3][0] = b.y + 2;
			b.coord[0][1] = b.coord[1][1] = b.x + 1;
			b.coord[2][1] = b.coord[3][1] = b.x;
		}
		break;
	}

	for (int i = 0; i < 4; i++)
	{
		bl->coord[i][0] = b.coord[i][0];
		bl->coord[i][1] = b.coord[i][1];
	}

}


BLOCK GenerateNewBlock(HWND hWnd) // генерирует новый блок случайного вида
{
	BLOCK block;
	int n = time(NULL) % 7; 
	block.x = 3;
	block.y = 0;
	block.type = BlockType(n);
	block.rotate = 0;
	CalculateCoordinates(&block);
	return block;
}

void PutDownForOne()  
{
	for (int i = 0; i < 4; i++)
		currentBlock.coord[i][0] += 1;
	currentBlock.y += 1;
}

void ShiftLeftForOne()
{
	for (int i = 0; i < 4; i++)
		currentBlock.coord[i][1] -= 1;
	currentBlock.x -= 1;
}

void ShiftRightForOne()
{
	for (int i = 0; i < 4; i++)
		currentBlock.coord[i][1] += 1;
	currentBlock.x += 1;
}


void AddNewToTable() // внести упавший блок в table[][]
{
	for (int i = 0; i < 4; i++)
	{
		int y = currentBlock.coord[i][0]; // те координаты котороые сейчас занимает блок
		int x = currentBlock.coord[i][1];
		table[y][x] = currentBlock.type; //занести в table информацию о типе (и цвете) блока
	}
}

void DeleteLine(int number) 
{
	for (int i = number; i>0; i--)
		for (int j=0; j<TABLE_WIDTH; j++)
			table[i][j] = table[i-1][j];
	for (int j = 0; j < TABLE_WIDTH; j++)
		table[0][j] = -1;
	score += 10;
	if (!isEasy && (speed >= 300))
		speed -= 50;
}

void CheckFullLines(HWND hWnd)
{
	for (int i = 0; i < TABLE_HEIGHT; i++)
	{
		int count = 0;
		for (int j = 0; j < TABLE_WIDTH; j++)
		{
			if (table[i][j] != -1)
				count++;
			else break;
		}
		if (count == TABLE_WIDTH)
		{
			DeleteLine(i);
			SetTimer(hWnd, 1, speed, NULL);
		}
	}
}


bool CheckOverlap() // true - данные позиции в table[][] заняты
{
	// сравнить каждую пару из coord с такой же ячейкой table
	for (int i = 0; i < 4; i++)
	{
		int blockX = currentBlock.coord[i][1];
		int blockY = currentBlock.coord[i][0];
		if (table[blockY][blockX] != -1)
			return true;
	}
	return false;
}

bool TryToRotate() 
{
	currentBlock.rotate = (currentBlock.rotate + 1)%4;
	CalculateCoordinates(&currentBlock);
	// проверить выход за края экрана
	for (int i = 0; i < 4; i++)
	{
		if (currentBlock.coord[i][0]<0 || currentBlock.coord[i][1]<0 || currentBlock.coord[i][0] >= TABLE_HEIGHT || currentBlock.coord[i][1] >= TABLE_WIDTH)
		{
			currentBlock.rotate = (currentBlock.rotate - 1 + 4) % 4;
			CalculateCoordinates(&currentBlock);
			return false;
		}
	}
	
	if (CheckOverlap())
	{
		currentBlock.rotate = (currentBlock.rotate - 1 + 4) % 4;
		CalculateCoordinates(&currentBlock);
		return false;
	}
	return true;
}

void StartNewGame(HWND hWnd)
{
	isGame = true;
	CleanTable();

	currentBlock.x = 3;
	currentBlock.y = 0;

	CalculateCoordinates(&currentBlock);
	speed = 1500;
	score = 0;
	nextBlock = GenerateNewBlock(hWnd);
	SetTimer(hWnd, 1, speed, NULL);
	isNewStep = true;
	isFirstLaunch = true;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		CreateMenu(hWnd);
		FirstLaunch(hWnd);
		hsFont = InitializeSmallFont(logsFont);
		hbFont = InitializeBigFont(logbFont);
		currentBlock = GenerateNewBlock(hWnd);
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);

		hdcMem = CreateCompatibleDC(hdc); // new
		RECT r;
		SetRect(&r, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
		
		hbm = CreateCompatibleBitmap(hdc, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)); // new
		SelectObject(hdcMem, hbm); // new

		FillRect(hdcMem, &r, backgroundBrush);

		if (isFirstLaunch) // запуск программы, начало новой игры 
		{
			isFirstLaunch = false; 
			SetBkColor(hdcMem, BACKGROUND_COLOR);
			SelectObject(hdcMem, hsFont);
			SetTextColor(hdcMem, GREY_COLOR);
			RECT r;
			SetRect(&r, 2 * INDENT + GAME_WIDTH, SCORE_Y - 65, WINDOW_WIDTH - INDENT, SCORE_Y - 20 );
			DrawTextW(hdcMem, _T("SCORE"), 5, &r, DT_CENTER);
			SetRect(&r, 2 * INDENT + GAME_WIDTH, NEXT_Y - 65, WINDOW_WIDTH - INDENT, NEXT_Y - 20);
			DrawTextW(hdcMem, _T("NEXT"), 4, &r, DT_CENTER);
		}

		if (isNewStep)  // вывести nextBlock
		{
			FillRect(hdcMem, &scoreRect, backgroundBrush);
			FillRect(hdcMem, &nextRect, backgroundBrush); 

			SetBkColor(hdcMem, BACKGROUND_COLOR);
			SelectObject(hdcMem, hbFont);
			SetTextColor(hdcMem, GREY_COLOR);
			DrawTextW(hdcMem, (LPCWSTR)std::to_wstring(score).c_str(), std::to_wstring(score).length(), &scoreRect, DT_CENTER);

			PrintNextBlock(hWnd);
			PrintCurrentBlock(hWnd);
			isNewStep = false;			
		}

		SelectObject(hdcMem, backgroundBrush);
		FillRect(hdcMem, &gameRect, backgroundBrush);
		FrameRect(hdcMem, &gameRect, greyBrush);

		if (isGame && isPause) // перекрыть черным поле для игры и написать "пауза"
		{
			SetBkColor(hdcMem, BACKGROUND_COLOR);
			SetTextColor(hdcMem, GREY_COLOR);
			SelectObject(hdcMem, hbFont);
			RECT r;
			SetRect(&r, INDENT, WINDOW_HEIGHT/2-90,INDENT+GAME_WIDTH, WINDOW_HEIGHT/2);
			DrawTextW(hdcMem, _T("PAUSE"), 5, &r, DT_CENTER | DT_EDITCONTROL);
		} 
		else if (isGame && !isPause)
		{
			// прорисовка игрового поля
			for (int i = 0; i < TABLE_HEIGHT; i++)
				for (int j = 0; j < TABLE_WIDTH; j++)
					if (table[i][j] != -1) // не пустой
					{
						RECT r;
						int x11 = INDENT + j * BLOCK_SIDE;
						int y11 = INDENT + i * BLOCK_SIDE;
						SetRect(&r, x11, y11, x11 + BLOCK_SIDE, y11 + BLOCK_SIDE);
						HBRUSH blockBrush = CreateSolidBrush(ChooseBrushColor(table[i][j]));
						FillRect(hdcMem, &r, blockBrush);
						DeleteObject(blockBrush);
					}
			PrintCurrentBlock(hWnd);
		}
		BitBlt(hdc, 0, 0, (GetSystemMetrics(SM_CXSCREEN)), (GetSystemMetrics(SM_CYSCREEN)), hdcMem, 0, 0, SRCCOPY); // new
		EndPaint(hWnd, &ps);
		break;
	case WM_COMMAND:
		switch (wParam)
		{
		case 11: // new game, easy
			isEasy = true;
			StartNewGame(hWnd);
			InvalidateRect(hWnd, NULL, FALSE);
			break;
		case 12:	// new game, hard
			isEasy = false;
			StartNewGame(hWnd);
			InvalidateRect(hWnd, NULL, FALSE);
			break;
		case 2:
			try
			{
				if (isMusic)
					PlaySound(NULL, NULL, NULL);
				else PlaySound(TEXT("C:\\Users\\admin\\source\\repos\\Tetris\\music.wav"), NULL, SND_LOOP | SND_ASYNC);
				isMusic = !isMusic;
			}
			catch (int i) {}
			break;
		case 3:
				Records rec = ReadRecordsFromFile(hWnd);
	
				std::wstring result = _T("Easy mode:  ");
				result = result + std::to_wstring(rec.easyRecord).c_str() + _T("\n");
				result = result + _T("Hard mode:  ") + std::to_wstring(rec.hardRecord).c_str();
				MessageBox(hWnd, (LPCWSTR)result.c_str(), L"Records", MB_OK);
			break;
		}
		break;
	case WM_KEYDOWN:	// стрелки
		switch (wParam)
		{ 
			case VK_LEFT: 
				if (!isPause &&  CheckLeftPosition(currentBlock))
				{
					ShiftLeftForOne();
					InvalidateRect(hWnd, &gameRect, FALSE);
				}
				break;
			case VK_RIGHT:
				if (!isPause && CheckRightPosition(currentBlock))
				{
					ShiftRightForOne();
					InvalidateRect(hWnd, &gameRect, FALSE);
				}
				break;
			case VK_UP:
				TryToRotate();
				InvalidateRect(hWnd, &gameRect, FALSE);
				break;
			case VK_DOWN:
				if (!isPause && CheckDownPosition(currentBlock)) //  блок спускается на одну позицию вниз
				{									
					PutDownForOne();
					InvalidateRect(hWnd, &gameRect, FALSE);
				}
				break;
			case VK_SPACE: 
				if (isGame)
				{
					if (!isPause)
					{
						KillTimer(hWnd, 1);
					}
					else SetTimer(hWnd, 1, speed, NULL);

					isPause = !isPause; // смена пауза <-> игровой процесс
					InvalidateRect(hWnd, &gameRect, FALSE);
				}
		}
		break;
	case WM_TIMER:
		if (wParam == 1)
		{
			if (CheckDownPosition(currentBlock)) // опустить блок
			{
				PutDownForOne();
				InvalidateRect(hWnd, &gameRect, FALSE);
			}
			else								// приземлить блок
			{
				if (CheckOverlap())
				{					
					isGame = false; //false - проигрыш
					isPause = false; // true - пауза
					KillTimer(hWnd, 1);
					//Compare records
					Records rec = ReadRecordsFromFile(hWnd);
						if (isEasy && easyRecord < score)
						{
							WriteRecordsToFile(score, rec.hardRecord);
						} 
						else if (!isEasy && hardRecord < score)
						{
							WriteRecordsToFile(rec.easyRecord, score);
						}
					std::wstring result = _T("Your score is ");
					result = result + std::to_wstring(score).c_str();
					MessageBox(hWnd, (LPCWSTR)result.c_str(), L"GAME OVER", MB_OK);
				}
				else
				{
					AddNewToTable();
					currentBlock = nextBlock;
					nextBlock = GenerateNewBlock(hWnd);

					isNewStep = true;
					score += 4;
					CheckFullLines(hWnd);

					InvalidateRect(hWnd, &gameRect, FALSE);
					InvalidateRect(hWnd, &nextRect, FALSE);
					InvalidateRect(hWnd, &scoreRect, FALSE);
				}
			}
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}
	return 0;
}


