#include "Launcher_child.h"

extern HINSTANCE g_hInst;
//===================================================================================
//WndProc
//===================================================================================
static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:
	{
		CreateWindowEx(
			WS_EX_CLIENTEDGE, L"EDIT",   // predefined class 
			NULL,         // no window title 
			WS_CHILD | WS_VISIBLE | ES_LEFT ,
			5, 15, 290, 20,   // set size in WM_SIZE message 
			hWnd,         // parent window 
			(HMENU)ID_OPEN_FILE_EDIT,   // edit control ID 
			((LPCREATESTRUCT)lParam)->hInstance,
			NULL);        // pointer not needed 

		CreateWindowEx(WS_EX_CLIENTEDGE, L"BUTTON", L"Open", WS_CHILDWINDOW | WS_VISIBLE,
			305,
			15,
			50,
			20,
			hWnd,
			(HMENU)ID_OPEN_FILE_DIALOG_BUTTON,
			((LPCREATESTRUCT)lParam)->hInstance,
			NULL);

		HWND hCheckBox = CreateWindowEx(0, L"BUTTON", L"Checkbox", BS_AUTOCHECKBOX| BS_CHECKBOX| WS_CHILDWINDOW | WS_VISIBLE, 360, 15, 20, 20, hWnd, (HMENU)ID_AS_ADMIN_CHECKBOX, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		break;
	}
	case WM_CLOSE:
		//ShowWindow(hWnd, SW_HIDE);
		DestroyWindow(hWnd);
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void RegisterChildClass(HINSTANCE hInst)
{
	WNDCLASSEX wclx;
	memset(&wclx, 0, sizeof(wclx));

	wclx.cbSize = sizeof(wclx);
	wclx.style = 0;
	wclx.lpfnWndProc = &WndProc;
	wclx.cbClsExtra = 0;
	wclx.cbWndExtra = 0;
	wclx.hInstance = hInst;
	wclx.hIconSm = wclx.hIcon = 0;
	wclx.hCursor = LoadCursor(NULL, IDC_ARROW);
	wclx.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);

	wclx.lpszMenuName = NULL;
	wclx.lpszClassName = CHILD_CLASSNAME;

	RegisterClassEx(&wclx);
}