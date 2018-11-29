#include <windows.h>
#include <stdlib.h> 
#include <fstream>
#include <strsafe.h>

#include "FileWork.h"

Records ReadRecordsFromFile(HWND hWnd)
{
	Records r;
	try
	{
		std::ifstream file("C:\\Users\\admin\\source\\repos\\Tetris\\Records.txt");
		if (file.is_open())
		{
			file >> r.easyRecord;
			file >> r.hardRecord;
		}
		else
		{
			r.easyRecord = 0;
			r.hardRecord = 0;
		}
		file.close();
		return r;
	}
	catch (int i)
	{
		MessageBox(hWnd, L"Error", L"Oops...", MB_OK);
		r.easyRecord = 0;
		r.hardRecord = 0;
		return r;
	}
}

void WriteRecordsToFile(int first, int second)
{
	using std::ios_base;
	std::wfstream file(L"C:\\Users\\admin\\source\\repos\\Tetris\\Records.txt");
	file << first;
	file << "\n";
	file << second;
}

