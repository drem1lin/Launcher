#pragma once
#include <Windows.h>

class HotKey
{
public:
	HotKey(int id, HWND hWnd, UINT fsModifiers, UINT vk, wchar_t* Command, size_t CommandLength, wchar_t* Arg, size_t ArgLen, bool asAdmin);
	HotKey(int id, HWND hWnd, UINT fsModifiers, UINT vk, wchar_t* Command, size_t CommandLength, bool asAdmin);
	HotKey(int id, HWND hWnd, UINT fsModifiers, UINT vk, wchar_t* Command, size_t CommandLength);
	~HotKey();
	void ExecuteCommand();
	bool isPressedKeyMatch(UINT fsModifiers, UINT vk);

private:
	//Hotkey Data
	int id;
	HWND hWnd;
	UINT fsModifiers;
	UINT vk;
	BOOL isHotKeyRegistered;


	//CommandData
	wchar_t Path[MAX_PATH];
	wchar_t Agrument[MAX_PATH];
	BOOL bRequireAdmin;
};

