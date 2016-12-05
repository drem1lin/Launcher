#include <windows.h>
//#include <Commctrl.h>

#include "resource.h"
#include "HotKey.h"

#include "Launcher_child.h"

#define THIS_CLASSNAME L"Kaptur's Launcher"

UINT WM_TASKBARCREATED = 0;
HINSTANCE g_hInst = 0;
static BOOL g_bModalState = FALSE; //Is messagebox shown
HotKey* hk = nullptr;
								   //===================================================================================
								   //ShowPopupMenu
								   //===================================================================================
BOOL ShowPopupMenu(HWND hWnd, POINT *curpos) 
{
	//ADD MENU ITEMS.------------------------------------------------------------------
	HMENU hPop = CreatePopupMenu();
	if (g_bModalState) { return FALSE; }
	InsertMenu(hPop, 0, MF_BYPOSITION | MF_STRING, ID_PREFERENCES, L"Preferences");
	InsertMenu(hPop, 1, MF_BYPOSITION | MF_STRING, ID_ABOUT, L"About...");
	InsertMenu(hPop, 2, MF_BYPOSITION | MF_STRING, ID_EXIT, L"Exit");
	
	//CAN DO WITHOUT STUFF.------------------------------------------------------------
	SetMenuDefaultItem(hPop, ID_ABOUT, FALSE);
	SetFocus(hWnd);
	SendMessage(hWnd, WM_INITMENUPOPUP, (WPARAM)hPop, 0);

	//SHOW POPUP MENU.-----------------------------------------------------------------
	//GET CURSOR POSITION TO CREATE POPUP THERE.-------------------------------------
	POINT pt;
	if (!curpos) {
		GetCursorPos(&pt);
		curpos = &pt;
	}
	
	//DISPLAT MENU AND WAIT FOR SELECTION.-----------------------------------------
	WORD cmd = TrackPopupMenu(hPop, TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD | TPM_NONOTIFY, curpos->x, curpos->y, 0, hWnd, NULL);

	//SEND MESSAGE MAPPED TO SELECTED ITEM.----------------------------------------
	SendMessage(hWnd, WM_COMMAND, cmd, 0);
	DestroyMenu(hPop);
	return 0;
}

//===================================================================================
//RemoveTrayIcon
//===================================================================================
void RemoveTrayIcon(HWND hWnd, UINT uID) 
{
	NOTIFYICONDATA  nid;
	memset(&nid, 0, sizeof(NOTIFYICONDATA));
	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hWnd = hWnd;
	nid.uID = uID;

	Shell_NotifyIcon(NIM_DELETE, &nid);
}

//===================================================================================
//AddTrayIcon
//===================================================================================
void AddTrayIcon(HWND hWnd, UINT uID, UINT uCallbackMsg, UINT uIcon) 
{
	//CREATE SYSTEN TRAY ICON.---------------------------------------------------------------------
	NOTIFYICONDATA  nid;
	memset(&nid, 0, sizeof(NOTIFYICONDATA));
	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hWnd = hWnd;
	nid.uID = uID;
	nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	nid.uCallbackMessage = uCallbackMsg;
	//LoadIconMetric(g_hInst, MAKEINTRESOURCE(IDI_ICON1), LIM_SMALL, &(nid.hIcon));
	nid.hIcon = LoadIcon(g_hInst, MAKEINTRESOURCE(IDI_ICON1));
	wcscpy_s(nid.szTip, L"Fast App Launcher\0");

	//SEND MESSAGE TO SYSTEM TRAY TO ADD ICON.--------------------------------------------
	Shell_NotifyIcon(NIM_ADD, &nid);
}

static void WMCommandProcessor(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if (g_bModalState)
		return;
	switch (LOWORD(wParam))
	{
	case ID_ABOUT:
		g_bModalState = TRUE;
		MessageBox(hWnd, TEXT("Hi!"), TEXT("Title"), MB_ICONINFORMATION | MB_OK);
		g_bModalState = FALSE;
		break;

	case ID_EXIT:
		PostMessage(hWnd, WM_DESTROY, 0, 0);
		break;
	case ID_PREFERENCES:
		ShowWindow(hWnd, SW_RESTORE);
		break;
	default:
		break;
	}
	return;
}

//===================================================================================
//WndProc
//===================================================================================
static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
	switch (uMsg)
	{
	case WM_CREATE:
	{		
		AddTrayIcon(hWnd, 1, WM_APP, 0);
		CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, CHILD_CLASSNAME, L"Child 1", WS_CHILDWINDOW | WS_VISIBLE,
			5,
			5,
			450,
			50,
			hWnd,
			NULL,
			g_hInst,
			NULL);
		CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, CHILD_CLASSNAME, L"Child 1", WS_CHILDWINDOW | WS_VISIBLE,
			5,
			65,
			450,
			50,
			hWnd,
			NULL,
			g_hInst,
			NULL);
		break;
	}

	case WM_CLOSE:
		ShowWindow(hWnd, SW_HIDE);
		//DestroyWindow(hWnd);
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_COMMAND:
		WMCommandProcessor(hWnd, wParam, lParam);
		break;

	case WM_APP:
		if (g_bModalState)
			break;
		switch (lParam) 
		{
		case WM_LBUTTONDBLCLK:
			ShowWindow(hWnd,SW_RESTORE);
			break;

		case WM_RBUTTONUP:
			SetForegroundWindow(hWnd);
			ShowPopupMenu(hWnd, NULL);
			//PostMessage(hWnd, WM_APP + 1, 0, 0);
			break;
		}
		break;
	case WM_HOTKEY:
		if (hk->isPressedKeyMatch(LOWORD((UINT)lParam), (UINT)wParam))
		{
			hk->ExecuteCommand();
		}
		break;
	default:
		{
			if (uMsg == WM_TASKBARCREATED)
				AddTrayIcon(hWnd, 1, WM_APP, 0);
		}
		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void RegisterMainClass(HINSTANCE hInst)
{
	WNDCLASSEX wclx;
	memset(&wclx, 0, sizeof(wclx));

	wclx.cbSize = sizeof(wclx);
	wclx.style = 0;
	wclx.lpfnWndProc = &WndProc;
	wclx.cbClsExtra = 0;
	wclx.cbWndExtra = 0;
	wclx.hInstance = hInst;
	wclx.hIconSm = wclx.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON1));
	wclx.hCursor = LoadCursor(NULL, IDC_ARROW);
	wclx.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);

	wclx.lpszMenuName = NULL;
	wclx.lpszClassName = THIS_CLASSNAME;

	RegisterClassEx(&wclx);
}

//===================================================================================
//WinMain
//===================================================================================
int WINAPI wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nShowCmd
)
{

	{
		//CHECK IF PREVIOUS ISTANCE IS RUNNING.-----------------------------------------------------
		HWND hPrev = NULL;
		if (hPrev = FindWindow(THIS_CLASSNAME, TEXT("Title"))) {
			MessageBox(NULL, TEXT("Previous instance alredy running!"), TEXT("Warning"), MB_OK);
			return 0;
		}
	}

	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

	WM_TASKBARCREATED = RegisterWindowMessageA("TaskbarCreated");

	RECT WorkAreaSize;
	WorkAreaSize.bottom = 720;
	WorkAreaSize.top = 0;
	WorkAreaSize.right = 1280;
	WorkAreaSize.left = 0;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &WorkAreaSize, 0);

	UINT xWindowsSize = 480;
	UINT yWindowsSize = 320;

	UINT yPosition = WorkAreaSize.bottom - WorkAreaSize.top - yWindowsSize;
	UINT xPosition = WorkAreaSize.right - WorkAreaSize.left - xWindowsSize;
	//REGISTER WINDOW.--------------------------------------------------------------------------
	RegisterMainClass(hInstance);
	RegisterChildClass(hInstance);

	g_hInst = hInstance;
	//CREATE WINDOW.----------------------------------------------------------------------------
	HWND hWnd = CreateWindow(THIS_CLASSNAME, TEXT("Title"), WS_SYSMENU | WS_CAPTION, xPosition, yPosition, xWindowsSize, yWindowsSize, NULL, NULL, hInstance, NULL);
	if (!hWnd) 
	{
		MessageBox(NULL, L"Can't create window!", TEXT("Warning!"), MB_ICONERROR | MB_OK | MB_TOPMOST);
		return 1;
	}

	ShowWindow(hWnd, SW_HIDE);
	
	hk = new HotKey(1,hWnd, MOD_CONTROL,0x59, L"test.exe", 7, true);
	//MONITOR MESSAGE QUEUE.--------------------------------------------------------------------
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	//DESTROY WINDOW.---------------------------------------------------------------------------
	UnregisterClass(THIS_CLASSNAME, hInstance);
	hk->~HotKey();
	RemoveTrayIcon(hWnd, 1);
	return (int)msg.wParam;
}