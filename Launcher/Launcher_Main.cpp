#include <windows.h>

#include "resource.h"

#define THIS_CLASSNAME L"Kaptur's Launcher"

UINT WM_TASKBARCREATED = 0;
HINSTANCE g_hInst = 0;
static BOOL g_bModalState = FALSE; //Is messagebox shown

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
	nid.hIcon = LoadIcon(g_hInst, MAKEINTRESOURCE(IDI_ICON1));
	wcscpy(nid.szTip, L"Fast App Launcher\0");

	//SEND MESSAGE TO SYSTEM TRAY TO ADD ICON.--------------------------------------------
	Shell_NotifyIcon(NIM_ADD, &nid);
}

//===================================================================================
//WndProc
//===================================================================================
static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
	switch (uMsg)
	{
	case WM_CREATE:
		AddTrayIcon(hWnd, 1, WM_APP, 0);
		return 0;

	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_COMMAND:
		if (g_bModalState)
			break;
		switch (LOWORD(wParam)) 
		{
		case ID_ABOUT:
			g_bModalState = TRUE;
			MessageBox(hWnd, TEXT("Hi!"), TEXT("Title"), MB_ICONINFORMATION | MB_OK);
			g_bModalState = FALSE;
			break;

		case ID_EXIT:
			PostMessage(hWnd, WM_CLOSE, 0, 0);
			break;
		default:
			break;
		}
		break;

	case WM_APP:
		if (g_bModalState)
			break;
		switch (lParam) 
		{
		case WM_LBUTTONDBLCLK:
			SendMessage(hWnd, WM_COMMAND, ID_ABOUT, 0);
			break;

		case WM_RBUTTONUP:
			SetForegroundWindow(hWnd);
			ShowPopupMenu(hWnd, NULL);
			//PostMessage(hWnd, WM_APP + 1, 0, 0);
			break;
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

void MyRegisterClass(HINSTANCE hInst)
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
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE prev, LPSTR cmdline, int show) {

	{
		//CHECK IF PREVIOUS ISTANCE IS RUNNING.-----------------------------------------------------
		HWND hPrev = NULL;
		if (hPrev = FindWindow(THIS_CLASSNAME, TEXT("Title"))) {
			MessageBox(NULL, TEXT("Previous instance alredy running!"), TEXT("Warning"), MB_OK);
			return 0;
		}
	}

	WM_TASKBARCREATED = RegisterWindowMessageA("TaskbarCreated");

	//REGISTER WINDOW.--------------------------------------------------------------------------
	MyRegisterClass(hInst);

	g_hInst = hInst;
	//CREATE WINDOW.----------------------------------------------------------------------------
	HWND hWnd = CreateWindow(THIS_CLASSNAME, TEXT("Title"), WS_OVERLAPPEDWINDOW | ~WS_VISIBLE, 100, 100, 250, 150, NULL, NULL, hInst, NULL);
	if (!hWnd) {
		MessageBox(NULL, L"Can't create window!", TEXT("Warning!"), MB_ICONERROR | MB_OK | MB_TOPMOST);
		return 1;
	}

	//MONITOR MESSAGE QUEUE.--------------------------------------------------------------------
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	//DESTROY WINDOW.---------------------------------------------------------------------------
	UnregisterClass(THIS_CLASSNAME, hInst);
	RemoveTrayIcon(hWnd, 1);
	return msg.wParam;
}