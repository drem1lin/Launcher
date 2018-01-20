#pragma once
#include <Windows.h>
#include "HotKey.h"
#include "Defines.h"

#define IDC_VSCROLLBAR 0x6000

void RegisterScrollWindowClass(HINSTANCE hInst);
HWND CreateChildWindow(HWND ParentWindow, int Window, HotKey* key, RECT* WindowRectSize);