#include "Launcher_ScrollWindow.h"
#include <string>
#include "DBFunctions.h"
#include "Launcher_child.h"

extern std::vector<HotKey> keys;
extern std::vector<HWND> ChildWindows;

HWND CreateChildWindow(HWND ParentWindow, HWND SubscribeWindow, int Window, HotKey* key, RECT* WindowRectSize)
{
	std::wstring WindowNameTemplate(L"Child ");
	std::wstring WindowName;
	WindowName = WindowNameTemplate + std::to_wstring(Window);
	if (key)
		key->Register(SubscribeWindow, Window);

	HINSTANCE hInst = (HINSTANCE)GetWindowLongPtr(ParentWindow, GWLP_HINSTANCE);
	return CreateWindowEx(0, CHILD_CLASSNAME, WindowName.c_str(), WS_CHILDWINDOW | WS_VISIBLE | WS_BORDER,
		WindowRectSize->left,
		WindowRectSize->top + 102 * Window,
		WindowRectSize->right,
		100,
		ParentWindow,
		NULL,
		hInst,
		key);
}

//===================================================================================
//WndProc
//===================================================================================
static LRESULT CALLBACK ScrollWindowWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	std::string dbName;
	size_t i = 0;
	HWND Child = 0;

	RECT WindowRectSize = { 0 };
	SCROLLINFO si = { 0 };

	switch (uMsg)
	{
		case WM_CREATE:
		{
			CREATESTRUCT* CreateStruct = (CREATESTRUCT*)lParam;
			if (!CreateStruct || !CreateStruct->lpCreateParams)
				break;
			if (!GetClientRect(hWnd, &WindowRectSize))
				break;

			GetDataBaseName(dbName);
			if (dbName.empty())
				break;
			ReadSettingFromDatabase(const_cast<char*>(dbName.c_str()), keys);

			//WindowRectSize.right -= ScrollBarWidth;
			for (i = 0; i < keys.size(); i++)
			{
				Child = CreateChildWindow(hWnd, CreateStruct->hwndParent, (int)i, &keys[i], &WindowRectSize);
				if (Child == NULL)
					break;
				ChildWindows.push_back(Child);

			}
			Child = CreateChildWindow(hWnd, CreateStruct->hwndParent, (int)i, nullptr, &WindowRectSize);
			if (Child == NULL)
				break;
			ChildWindows.push_back(Child);
			HWND hScrollBar = ((PSCROLLWINDOWPARAMETERS)CreateStruct->lpCreateParams)->hScrollBarHandle;

			RtlZeroMemory(&si, sizeof(si));
			si.fMask = SIF_RANGE | SIF_POS;
			si.cbSize = sizeof(si);
			si.nPos = si.nMin = 0;
			si.nMax = abs((int)(i + 1) * 102 - CreateStruct->cy);
			SetScrollInfo(hScrollBar, SB_CTL, &si, TRUE); 

			BOOL b = SetWindowPos(hWnd, 
				0, 
				CreateStruct->x, 
				CreateStruct->y,
				CreateStruct->cx,
				max((int)(i + 1) * 102, CreateStruct->cy), 
				SWP_DRAWFRAME | SWP_SHOWWINDOW);
			break;
		}
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void RegisterScrollWindowClass(HINSTANCE hInst)
{
	WNDCLASSEX wclx;
	memset(&wclx, 0, sizeof(wclx));

	wclx.cbSize = sizeof(wclx);
	wclx.style = 0;
	wclx.lpfnWndProc = &ScrollWindowWndProc;
	wclx.cbClsExtra = 0;
	wclx.cbWndExtra = 1000;
	wclx.hInstance = hInst;
	wclx.hIconSm = wclx.hIcon = 0;
	wclx.hCursor = LoadCursor(NULL, IDC_ARROW);
	wclx.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);

	wclx.lpszMenuName = NULL;
	wclx.lpszClassName = SCROLLWINDOW_CLASSNAME;

	RegisterClassEx(&wclx);
}