#pragma once

#define MAINWINDOW_CLASSNAME L"Kaptur's Launcher"
#define SCROLLWINDOW_CLASSNAME L"Kaptur's Launcher scroll window"
#define CHILD_CLASSNAME L"Kaptur's Launcher child"
#define DATABASE_NAME "Settings.db"
#define TABLE_NAME "COMMANDS"

typedef struct _SCROLLWINDOWPARAMETERS
{
	HWND hScrollBarHandle;
}SCROLLWINDOWPARAMETERS, *PSCROLLWINDOWPARAMETERS;