#pragma once
#include <Windows.h>

#define CHILD_CLASSNAME L"Kaptur's Launcher child"

#define ID_OPEN_FILE_DIALOG_BUTTON 0x5001
#define ID_OPEN_FILE_EDIT 0x5002
#define ID_AS_ADMIN_CHECKBOX 0x5002

void RegisterChildClass(HINSTANCE hInst);
