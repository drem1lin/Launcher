#include "Launcher_child.h"
#include <string>

static void WMCommandProcessor(HWND hWnd, WPARAM wParam, LPARAM lParam);

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
		WINDOW_CONTROLS* wc =	(WINDOW_CONTROLS*) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(WINDOW_CONTROLS));
		if (wc == NULL)
		{
			MessageBox(hWnd, L"Out of Memory.\nApplication closing.", L"Error", MB_OK);
			ExitProcess(1);
		}
		wc->filepathEditWindowHWND = CreateWindowEx(
			WS_EX_CLIENTEDGE, L"EDIT",   // predefined class 
			NULL,         // no window title 
			WS_CHILD | WS_VISIBLE | ES_LEFT ,
			5, 15, 290, 20,   // set size in WM_SIZE message 
			hWnd,         // parent window 
			(HMENU)ID_OPEN_FILE_EDIT,   // edit control ID 
			((LPCREATESTRUCT)lParam)->hInstance,
			NULL);        // pointer not needed 

		wc->openfileButtonHWND = CreateWindowEx(WS_EX_CLIENTEDGE, L"BUTTON", L"Open", WS_CHILDWINDOW | WS_VISIBLE,
			305,
			15,
			50,
			20,
			hWnd,
			(HMENU)ID_OPEN_FILE_DIALOG_BUTTON,
			((LPCREATESTRUCT)lParam)->hInstance,
			NULL);

		wc->asadminChechboxHWND = CreateWindowEx(0, L"BUTTON", L"Checkbox", BS_AUTOCHECKBOX| BS_CHECKBOX| WS_CHILDWINDOW | WS_VISIBLE, 
			360,
			15,
			20,
			20,
			hWnd,
			(HMENU)ID_AS_ADMIN_CHECKBOX,
			((LPCREATESTRUCT)lParam)->hInstance,
			NULL);
		
		if (NULL != SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)(wc)))
			MessageBox(hWnd, std::to_wstring(GetLastError()).c_str(), L"Error", MB_OK);

		break;
	}
	case WM_COMMAND:
		WMCommandProcessor(hWnd, wParam, lParam);
		break;

	case WM_CLOSE:
		//ShowWindow(hWnd, SW_HIDE);
		DestroyWindow(hWnd);
		return 0;

	case WM_DESTROY:
		HeapFree(GetProcessHeap(), 0, (LPVOID)GetWindowLongPtr(hWnd, GWLP_USERDATA));
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
	wclx.cbWndExtra = 1000;
	wclx.hInstance = hInst;
	wclx.hIconSm = wclx.hIcon = 0;
	wclx.hCursor = LoadCursor(NULL, IDC_ARROW);
	wclx.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);

	wclx.lpszMenuName = NULL;
	wclx.lpszClassName = CHILD_CLASSNAME;

	RegisterClassEx(&wclx);
}

bool GetExecutibleFileName(HWND hwnd, std::wstring& FileName)
{
	OPENFILENAME ofn;       // common dialog box structure
	wchar_t szFile[MAX_PATH];       // buffer for file name

									// Initialize OPENFILENAME
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFile = szFile;
	// Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
	// use the contents of szFile to initialize itself.
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = L"*.exe\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	// Display the Open dialog box. 
	bool b = GetOpenFileName(&ofn);
	if (b == TRUE)
		FileName.append(ofn.lpstrFile);
	return b;
}

static void WMCommandProcessor(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD(wParam))
	{
	case ID_OPEN_FILE_DIALOG_BUTTON:
	{
		std::wstring FileName;
		if (!GetExecutibleFileName(hWnd, FileName))
			break;
		WINDOW_CONTROLS* wc = (PWINDOW_CONTROLS)GetWindowLongPtr(hWnd, GWLP_USERDATA);
		if (wc->filepathEditWindowHWND!=NULL)
			SetWindowText(wc->filepathEditWindowHWND, FileName.c_str());
		break;
	}
	default:
		break;
	}
	return;
}