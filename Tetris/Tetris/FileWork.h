#pragma once


struct Records {
	int easyRecord;
	int hardRecord;
};

Records ReadRecordsFromFile(HWND hWnd);
void WriteRecordsToFile(int first, int second);

