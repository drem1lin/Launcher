#pragma once

#include <windows.h>

typedef struct _MAINWINDOWPARAMETERS
{
	HWND hScrollBar;
	int ScrollWindowX;
	int ScrollWindowY;
	int ScrollWindowStartCX;
	int ScrollWindowStartCY;
	HWND hScrollableWindow;
	UINT TaskBarCreatedMessage;
}MAINWINDOWPARAMETERS, *PMAINWINDOWPARAMETERS;

typedef struct _CREATEMAINWINDOWPARAMETERS
{
	UINT TaskBarCreatedMessage;
}CREATEMAINWINDOWPARAMETERS, *PCREATEMAINWINDOWPARAMETERS;