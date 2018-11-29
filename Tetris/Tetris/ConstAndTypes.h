#pragma once

#define INDENT 25
#define TABLE_HEIGHT 14
#define TABLE_WIDTH 10
#define  WINDOW_HEIGHT 800
#define  WINDOW_WIDTH 800
#define  GAME_HEIGHT 700
#define  BLOCK_SIDE GAME_HEIGHT/TABLE_HEIGHT // 700/14 = 50 
#define  GAME_WIDTH BLOCK_SIDE*TABLE_WIDTH // 500

#define SCORE_X 550 // значения для прямоугольников с выводом, а не заголовками
#define SCORE_Y 200

#define NEXT_X 550
#define NEXT_Y 450

#define GREY_COLOR RGB(140, 145, 150) // цвет рамки игрового поля
#define BACKGROUND_COLOR RGB(25, 25, 25) // цвет фона
#define COLOR_TEXT_COLOR RGB(180, 180, 70) // цвет для вывода текста и счета



enum BlockType // ВИД БЛОКА
{
	I, J, L, O, S, T, Z
};

typedef struct BLOCK_STATUS {
	int x;	//крайний левый угол  0..9
	int y;   // 0..13
	BlockType type; // квадрат, полоса и т.д.
	int rotate; // в каком положении блок, куда повернут
	int coord[4][2];  // четыре пары (у, x) координат занимаемого в таблице блока
} BLOCK;
