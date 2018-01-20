#include "Launcher_Main.h"
#include <Commctrl.h>
#include <vector>
#include <string>

#include "resource.h"
#include "HotKey.h"
#include "Defines.h"

#include "../sqlite/sqlite3.h"
#include "DBFunctions.h"
#include "Launcher_ScrollWindow.h"
#include "Launcher_child.h"

std::vector<HotKey> keys;
std::vector<HWND> ChildWindows;

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
	//LoadIconMetric(g_hInst, MAKEINTRESOURCE(IDI_ICON1), LIM_SMALL, &(nid.hIcon));
	HINSTANCE hInst = (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE);
	nid.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON1));
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

bool GetWindowPos(HWND hWnd, int *x, int* y)
{
	HWND hParent = GetParent(hWnd);
	if (hParent == NULL)
		return false;

	POINT p = { 0 };
	
	if (!x || !y)
		return false;

	if (0 == MapWindowPoints(hWnd, hParent, &p, 1))
		return false;
	*x = p.x;
	*y = p.y;
	return true;
}

//===================================================================================
//WndProc
//===================================================================================
static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
	int j = 0;
	int ScrollBarWidth = 15;
	SCROLLINFO si = { 0 };
	RECT WindowRectSize = { 0 };
	SCROLLWINDOWPARAMETERS sbp = { 0 };
	MAINWINDOWPARAMETERS* mwp = nullptr;


	switch (uMsg)
		{
		case WM_CREATE:
		{	
			CREATESTRUCT* CreateStruct = (CREATESTRUCT*)lParam;
			CREATEMAINWINDOWPARAMETERS* cmwp = (CREATEMAINWINDOWPARAMETERS*)CreateStruct->lpCreateParams;
			if (!GetClientRect(hWnd, &WindowRectSize))
				break;

			AddTrayIcon(hWnd, 1, WM_APP, 0);

			HWND hScrollBar = CreateWindowEx(0, L"SCROLLBAR", NULL, WS_VISIBLE | WS_CHILD | SBS_VERT, WindowRectSize.right - ScrollBarWidth,
				0, ScrollBarWidth, WindowRectSize.bottom, hWnd, (HMENU)IDC_VSCROLLBAR, CreateStruct->hInstance, NULL);
				
			sbp.hScrollBarHandle = hScrollBar;

			int WindowCx = WindowRectSize.right - ScrollBarWidth - PADDING * 2;
			int WindowCy = WindowRectSize.bottom - PADDING * 2;

			HWND Child=CreateWindowEx(0, SCROLLWINDOW_CLASSNAME, NULL, WS_CHILD | WS_VISIBLE,
				PADDING, PADDING, WindowCx, WindowCy,
				hWnd, NULL, CreateStruct->hInstance, &sbp);
			DWORD err = GetLastError();

			mwp = (MAINWINDOWPARAMETERS*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(MAINWINDOWPARAMETERS));
			if (mwp == NULL)
			{
				MessageBox(hWnd, L"Out of Memory.\nApplication closing.", L"Error", MB_OK);
				ExitProcess(1);
			}

			mwp->ScrollWindowX = mwp->ScrollWindowY = PADDING;
			mwp->ScrollWindowStartCX = WindowCx;
			mwp->ScrollWindowStartCY = WindowCy;
			mwp->hScrollBar = hScrollBar;
			mwp->hScrollableWindow = Child;
			mwp->TaskBarCreatedMessage = mwp->TaskBarCreatedMessage;
			if (NULL != SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)(mwp)))
				MessageBox(hWnd, std::to_wstring(GetLastError()).c_str(), L"Error", MB_OK);

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
			for (size_t i = 0; i < keys.size(); i++)
			{
				if (keys[i].isPressedKeyMatch(LOWORD((UINT)lParam), (UINT)wParam))
					keys[i].ExecuteCommand();
			}
			break;
		case WM_VSCROLL:
			j = (int)HIWORD(wParam);
			RtlZeroMemory(&si, sizeof(si));
			si.cbSize = sizeof(SCROLLINFO);
			si.fMask = SIF_POS | SIF_RANGE;
			GetScrollInfo((HWND)lParam, SB_CTL, &si);
			mwp = (MAINWINDOWPARAMETERS*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
			if (!mwp)
				break;
			switch ((int)LOWORD(wParam))
			{
			case SB_PAGEUP:
			case SB_LINEUP:
				if (si.nPos <= si.nMin)
					break;
				si.nPos -= 20;
				if (si.nPos <= si.nMin)
					si.nPos = si.nMin;
				break;
			case SB_PAGEDOWN:
			case SB_LINEDOWN:
				if (si.nPos >= si.nMax)
					break;
				si.nPos += 20;
				if (si.nPos >= si.nMax)
					si.nPos = si.nMax;
				break;
			case SB_THUMBPOSITION:
			case SB_THUMBTRACK:
				si.nPos = j;
				break;
			}
			si.fMask = SIF_POS;
			SetScrollInfo((HWND)lParam, SB_CTL, &si, TRUE);
			if (!GetClientRect(mwp->hScrollableWindow, &WindowRectSize))
				break;
			MoveWindow(mwp->hScrollableWindow, mwp->ScrollWindowX, mwp->ScrollWindowY - si.nPos, 
				WindowRectSize.right, WindowRectSize.bottom, TRUE);
		//	SetWindowPos(mwp->hScrollableWindow, hWnd, mwp->ScrollWindowX, mwp->ScrollWindowY - si.nPos, 0, 0, SWP_NOSIZE);
			UpdateWindow(hWnd);
			break;
			
		default:
			{
				mwp = (MAINWINDOWPARAMETERS*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
				if (!mwp)
					break;
				if (uMsg == mwp->TaskBarCreatedMessage)
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
	wclx.lpszClassName = MAINWINDOW_CLASSNAME;

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
		if (hPrev = FindWindow(MAINWINDOW_CLASSNAME, TEXT("Title"))) {
			MessageBox(NULL, TEXT("Previous instance alredy running!"), TEXT("Warning"), MB_OK);
			return 0;
		}
	}

	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

	CREATEMAINWINDOWPARAMETERS cmwp = { 0 };
	cmwp.TaskBarCreatedMessage = RegisterWindowMessageA("TaskbarCreated");

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
	RegisterScrollWindowClass(hInstance);
	RegisterChildClass(hInstance);

	//CREATE WINDOW.----------------------------------------------------------------------------
	HWND hWnd = CreateWindow(MAINWINDOW_CLASSNAME, TEXT("Title"), WS_SYSMENU | WS_CAPTION | WS_CLIPCHILDREN, 
		xPosition, yPosition, xWindowsSize, yWindowsSize, NULL, NULL, hInstance, &cmwp);
	if (!hWnd) 
	{
		MessageBox(NULL, L"Can't create window!", TEXT("Warning!"), MB_ICONERROR | MB_OK | MB_TOPMOST);
		return 1;
	}

	ShowWindow(hWnd, SW_HIDE);
	
	//hk = new HotKey(1,hWnd, MOD_CONTROL,0x59, L"C:\\Windows\\System32\\notepad.exe", 2*31, true);
	//MONITOR MESSAGE QUEUE.--------------------------------------------------------------------
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	//DESTROY WINDOW.---------------------------------------------------------------------------
	UnregisterClass(MAINWINDOW_CLASSNAME, hInstance);
	//hk->~HotKey();
	RemoveTrayIcon(hWnd, 1);
	return (int)msg.wParam;
}