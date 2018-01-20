#include "Launcher_child.h"
#include <string>
#include <CommCtrl.h>

#include "DBFunctions.h"
#include "Defines.h"

static void WMCommandProcessor(HWND hWnd, WPARAM wParam, LPARAM lParam);

void CreateProgramSelector(WINDOW_CONTROLS* wc, HWND hWnd, CREATESTRUCT* CreateStruct)
{
	wc->selectprogramLabelHWND = CreateWindowEx(0, WC_STATIC, L"selectprogramLabelHWND",
		WS_CHILD | WS_VISIBLE | WS_TABSTOP,
		PADDING,
		PADDING,
		(CreateStruct->cx / 2) * 3 / 4 - PADDING_HALF - PADDING,
		CreateStruct->cy / 4 - PADDING_HALF - PADDING,//5+3		
		hWnd, (HMENU)(ID_OPEN_FILE_LABEL),
		CreateStruct->hInstance,
		NULL);
	SetWindowText(wc->selectprogramLabelHWND, L"Select program:");

	wc->openfileButtonHWND = CreateWindowEx(0, WC_BUTTON, L"Select", WS_CHILDWINDOW | WS_VISIBLE,
		CreateStruct->cx * 3 / 8 + PADDING_HALF,
		PADDING,
		CreateStruct->cx / 8 - PADDING_HALF - PADDING_HALF,
		CreateStruct->cy / 4 - PADDING - PADDING_HALF,
		hWnd,
		(HMENU)ID_OPEN_FILE_DIALOG_BUTTON,
		CreateStruct->hInstance,
		NULL);

	wc->filepathEditWindowHWND = CreateWindowEx(
		WS_EX_CLIENTEDGE, WC_EDIT,   // predefined class 
		NULL,         // no window title 
		WS_CHILD | WS_VISIBLE | ES_LEFT,
		PADDING,
		CreateStruct->cy / 4 + PADDING_HALF,
		CreateStruct->cx / 2 - PADDING_HALF - PADDING,
		CreateStruct->cy / 4 - PADDING_HALF - PADDING_HALF,   // set size in WM_SIZE message 
		hWnd,         // parent window 
		(HMENU)ID_OPEN_FILE_EDIT,   // edit control ID 
		CreateStruct->hInstance,
		NULL);        // pointer not needed 
}

void CreateHotKeySelector(WINDOW_CONTROLS* wc, HWND hWnd, CREATESTRUCT* CreateStruct)
{
	wc->hotkeyLabelHWND = CreateWindowEx(0, WC_STATIC, L"hotkeyLabelHWND",
		WS_CHILD | WS_VISIBLE | WS_TABSTOP,
		CreateStruct->cx/2 + PADDING_HALF,
		PADDING,
		CreateStruct->cx / 2 - PADDING - PADDING_HALF,
		CreateStruct->cy / 4 - PADDING_HALF - PADDING,//5+3		
		hWnd, (HMENU)(ID_HOTKEY_LABEL),
		CreateStruct->hInstance,
		NULL);
	SetWindowText(wc->hotkeyLabelHWND, L"Choose hotkey:");

	wc->modifiersComboBoxHWND = CreateWindowEx(0, WC_COMBOBOX, L"ModifiersComboBox", CBS_DROPDOWNLIST | WS_CHILDWINDOW | WS_VISIBLE | WS_VSCROLL | ES_AUTOVSCROLL,
		CreateStruct->cx / 2 + PADDING_HALF,
		CreateStruct->cy / 4 + PADDING_HALF,
		CreateStruct->cx/4 - PADDING_HALF - PADDING_HALF,
		CreateStruct->cy - PADDING_HALF - PADDING_HALF,
		hWnd,
		(HMENU)ID_MODIFIERS_LISTBOX,
		CreateStruct->hInstance,
		NULL);

	SendMessage(wc->modifiersComboBoxHWND, CB_ADDSTRING, 0, (LPARAM)L"MOD_ALT");
	SendMessage(wc->modifiersComboBoxHWND, CB_ADDSTRING, 0, (LPARAM)L"MOD_CONTROL");
	SendMessage(wc->modifiersComboBoxHWND, CB_ADDSTRING, 0, (LPARAM)L"MOD_SHIFT");
	SendMessage(wc->modifiersComboBoxHWND, CB_ADDSTRING, 0, (LPARAM)L"MOD_WIN");
	SendMessage(wc->modifiersComboBoxHWND, CB_SETCURSEL, 0, 0);

	wc->virtualkeyEditWindowHWND = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT,   // predefined class 
		NULL,         // no window title 
		WS_CHILD | WS_VISIBLE | ES_LEFT,
		CreateStruct->cx *3/4 + PADDING_HALF,
		CreateStruct->cy / 4 + PADDING_HALF,
		CreateStruct->cx / 4 - PADDING - PADDING_HALF,
		CreateStruct->cy / 4 - PADDING_HALF - PADDING_HALF ,   // set size in WM_SIZE message 
		hWnd,         // parent window 
		(HMENU)ID_VIRTUAL_BUTTON_EDIT,   // edit control ID 
		CreateStruct->hInstance,
		NULL);        // pointer not needed 
}

void CreateParametersSelector(WINDOW_CONTROLS* wc, HWND hWnd, CREATESTRUCT* CreateStruct)
{
	wc->argumentsLabelHWND = CreateWindowEx(0, WC_STATIC, L"argumentsLabelHWND",
		WS_CHILD | WS_VISIBLE | WS_TABSTOP,
		PADDING,
		CreateStruct->cy / 2 + PADDING_HALF,
		CreateStruct->cx / 2 - PADDING_HALF - PADDING,
		CreateStruct->cy / 4 - PADDING_HALF - PADDING_HALF,//5+3		
		hWnd, (HMENU)(ID_ARGS_LABEL),
		CreateStruct->hInstance,
		NULL);
	SetWindowText(wc->argumentsLabelHWND, L"Enter program args:");

	wc->argumentsEditWindowHWND = CreateWindowEx(
		WS_EX_CLIENTEDGE, WC_EDIT,   // predefined class 
		NULL,         // no window title 
		WS_CHILD | WS_VISIBLE | ES_LEFT,
		PADDING,
		CreateStruct->cy * 3 / 4 + PADDING_HALF,
		CreateStruct->cx / 2 - PADDING_HALF - PADDING,
		CreateStruct->cy / 4 - PADDING_HALF - PADDING,   // set size in WM_SIZE message 
		hWnd,         // parent window 
		(HMENU)ID_ARGS_EDIT,   // edit control ID 
		CreateStruct->hInstance,
		NULL);        // pointer not needed 
}

void CreateAsAdminAndButtons(WINDOW_CONTROLS* wc, HWND hWnd, CREATESTRUCT* CreateStruct)
{
	wc->asadminLabelHWND = CreateWindowEx(0, WC_STATIC, L"asadminLabelHWND",
		WS_CHILD | WS_VISIBLE | WS_TABSTOP,
		CreateStruct->cx / 2 + PADDING_HALF,
		CreateStruct->cy / 2 + PADDING_HALF,
		CreateStruct->cx / 2 - PADDING_HALF - PADDING_HALF,
		CreateStruct->cy / 4 - PADDING_HALF - PADDING_HALF,//5+3		
		hWnd, (HMENU)(ID_AS_ADMIN_LABEL),
		CreateStruct->hInstance,
		NULL);
	SetWindowText(wc->asadminLabelHWND, L"As admin ");

	wc->asadminChechboxHWND = CreateWindowEx(0, WC_BUTTON, L"", BS_AUTOCHECKBOX | BS_CHECKBOX | WS_CHILDWINDOW | WS_VISIBLE,
		CreateStruct->cx *3/ 4 -15-PADDING_HALF,
		CreateStruct->cy / 2 + PADDING_HALF,
		CreateStruct->cx / 2 - PADDING_HALF - PADDING_HALF,
		CreateStruct->cy / 4 - PADDING_HALF - PADDING_HALF,//5+3
		hWnd,
		(HMENU)ID_AS_ADMIN_CHECKBOX,
		CreateStruct->hInstance,
		NULL);

	wc->saveButtonHWND = CreateWindowEx(0, WC_BUTTON, L"Save", WS_CHILDWINDOW | WS_VISIBLE,
		CreateStruct->cx /2 + PADDING_HALF,
		CreateStruct->cy*3/4+PADDING_HALF,
		CreateStruct->cx / 4 - PADDING_HALF - PADDING_HALF,
		CreateStruct->cy / 4 - PADDING - PADDING_HALF,
		hWnd,
		(HMENU)ID_SAVE_BUTTON,
		CreateStruct->hInstance,
		NULL);

	wc->deleteButtonHWND = CreateWindowEx(0, WC_BUTTON, L"Delete", WS_CHILDWINDOW | WS_VISIBLE,
		CreateStruct->cx * 3 / 4 + PADDING_HALF,
		CreateStruct->cy * 3 / 4 + PADDING_HALF,
		CreateStruct->cx / 4 - PADDING_HALF - PADDING,
		CreateStruct->cy / 4 - PADDING - PADDING_HALF,
		hWnd,
		(HMENU)ID_DELETE_BUTTON,
		CreateStruct->hInstance,
		NULL);
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
		CREATESTRUCT* CreateStruct = (CREATESTRUCT*)lParam;
		HotKey* key = (HotKey*)CreateStruct->lpCreateParams;
		WINDOW_CONTROLS* wc =	(WINDOW_CONTROLS*) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(WINDOW_CONTROLS));
		if (wc == NULL)
		{
			MessageBox(hWnd, L"Out of Memory.\nApplication closing.", L"Error", MB_OK);
			ExitProcess(1);
		}
		//CreateWindowEx(0,L"Button", L"", WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
		//	CreateStruct->x-1, CreateStruct->y-1, CreateStruct->cx-1, CreateStruct->cy-1, hWnd, 0, CreateStruct->hInstance, 0);
		CreateProgramSelector(wc, hWnd, CreateStruct);
		CreateHotKeySelector(wc, hWnd, CreateStruct);
		CreateParametersSelector(wc, hWnd, CreateStruct);
		CreateAsAdminAndButtons(wc, hWnd, CreateStruct);
		if (key!=nullptr)
		{
			SetWindowText(wc->filepathEditWindowHWND, key->GetPath());
			SetWindowText(wc->argumentsEditWindowHWND, key->GetArgs());
			UINT vk = key->GetVK();
			SetWindowText(wc->virtualkeyEditWindowHWND, (wchar_t*)(&vk));
			SendMessage(wc->asadminChechboxHWND, BM_SETCHECK, key->GetAsAdmin()!=0 ? BST_CHECKED: BST_UNCHECKED, 0);
			SendMessage(wc->modifiersComboBoxHWND, CB_SETCURSEL, key->GetFsModifiers(), 0);
		}
		
		if (NULL != SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)(wc)))
			MessageBox(hWnd, std::to_wstring(GetLastError()).c_str(), L"Error", MB_OK);


		break;
	}
	case WM_COMMAND:
		WMCommandProcessor(hWnd, wParam, lParam);
		break;

	case WM_QUIT:
	case WM_CLOSE:
		//ShowWindow(hWnd, SW_HIDE);
		DestroyWindow(hWnd);
		break;

	case WM_DESTROY:
		HeapFree(GetProcessHeap(), 0, (LPVOID)GetWindowLongPtr(hWnd, GWLP_USERDATA)); //don't call
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

BOOL GetExecutibleFileName(HWND hwnd, std::wstring& FileName)
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
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFilter = L"Executible files\0*.exe\0\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_ENABLESIZING;

	// Display the Open dialog box. 
	BOOL b = GetOpenFileName(&ofn);
	if (b == TRUE)
		FileName.append(ofn.lpstrFile);
	return b;
}

bool InitializeHotKeyFromWindowData(WINDOW_CONTROLS* wc, HotKey& key, HWND hWnd)
{
	UINT fsModifiers;
	UINT vk;
	//CommandData
	std::wstring Path;
	std::wstring Agrument;
	bool bRequireAdmin;
	
	int cTxtLen;
	PWSTR pszMem;

	int argslen = GetWindowTextLength(wc->argumentsEditWindowHWND);
	int pathlen = GetWindowTextLength(wc->filepathEditWindowHWND);
	int vklen = GetWindowTextLength(wc->virtualkeyEditWindowHWND);
	if (pathlen == 0 && argslen == 0)
		return false;
	cTxtLen = max(max(pathlen, argslen), vklen);
	pszMem = (PWSTR)VirtualAlloc((LPVOID)NULL, cTxtLen + 1, MEM_COMMIT,PAGE_READWRITE);
	if (!pszMem)
		return false;
	GetWindowText(wc->filepathEditWindowHWND, pszMem, cTxtLen + 1);
	Path.append(pszMem);
	RtlZeroMemory(pszMem, cTxtLen + 1);
	GetWindowText(wc->argumentsEditWindowHWND, pszMem, cTxtLen + 1);
	Agrument.append(pszMem);
	RtlZeroMemory(pszMem, cTxtLen + 1);
	GetWindowText(wc->virtualkeyEditWindowHWND, pszMem, cTxtLen + 1);
	vk = pszMem[0];
	VirtualFree(pszMem, 0, MEM_RELEASE);

	bRequireAdmin = SendMessage(wc->asadminChechboxHWND, BM_GETCHECK, 0, 0) == BST_CHECKED ? TRUE: FALSE;
	
	fsModifiers = (UINT) SendMessage(wc->modifiersComboBoxHWND, CB_GETCURSEL, 0, 0);

	key = *new HotKey(0, fsModifiers, vk, const_cast<wchar_t*>(Path.c_str()), Path.size(), const_cast<wchar_t*>(Agrument.c_str()), argslen, bRequireAdmin);
	return true;
}


static void WMCommandProcessor(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	WINDOW_CONTROLS* wc = (PWINDOW_CONTROLS)GetWindowLongPtr(hWnd, GWLP_USERDATA);
	if (!wc)
		return;
	switch (LOWORD(wParam))
	{
	case ID_OPEN_FILE_DIALOG_BUTTON:
	{
		std::wstring FileName;
		if (!GetExecutibleFileName(hWnd, FileName))
			break;
		if (wc->filepathEditWindowHWND != NULL)
			SetWindowText(wc->filepathEditWindowHWND, FileName.c_str());
		break;
	}
	case ID_SAVE_BUTTON:
	{
		HotKey key;
		if (InitializeHotKeyFromWindowData(wc, key, hWnd))
		{
			std::string dbName;
			GetDataBaseName(dbName);
			SaveSettingsToDataBase(const_cast<char*>(dbName.c_str()), "COMMANDS", key);
		}
		break;
	}
	default:
		break;
	}
	return;
}