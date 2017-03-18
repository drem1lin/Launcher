#pragma once
#include <Windows.h>

#define CHILD_CLASSNAME L"Kaptur's Launcher child"

#define ID_OPEN_FILE_DIALOG_BUTTON 0x5001
#define ID_OPEN_FILE_EDIT 0x5002
#define ID_OPEN_FILE_LABEL 0x5003

#define ID_HOTKEY_LABEL 0x5004
#define ID_MODIFIERS_LISTBOX 0x5005
#define ID_VIRTUAL_BUTTON_EDIT 0x5006

#define ID_ARGS_LABEL 0x5007
#define ID_ARGS_EDIT 0x5008

#define ID_AS_ADMIN_LABEL 0x5009
#define ID_AS_ADMIN_CHECKBOX 0x5010
#define ID_SAVE_BUTTON 0x5011
#define ID_DELETE_BUTTON 0x5012




typedef struct _WINDOW_CONTROLS
{
	HWND filepathEditWindowHWND;
	HWND openfileButtonHWND;
	HWND selectprogramLabelHWND;

	HWND hotkeyLabelHWND;
	HWND modifiersComboBoxHWND;
	HWND virtualkeyEditWindowHWND;

	HWND argumentsLabelHWND;
	HWND argumentsEditWindowHWND;

	HWND asadminChechboxHWND;
	HWND asadminLabelHWND;
	HWND saveButtonHWND;
	HWND deleteButtonHWND;

}WINDOW_CONTROLS, *PWINDOW_CONTROLS;

void RegisterChildClass(HINSTANCE hInst);
