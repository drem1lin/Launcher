#pragma once
#include <Windows.h>

class HotKey
{
public:
	HotKey(int id, HWND hWnd, UINT fsModifiers, UINT vk, wchar_t* Command, size_t CommandLength, wchar_t* Arg, size_t ArgLen, bool asAdmin);
	HotKey(int id, HWND hWnd, UINT fsModifiers, UINT vk, const wchar_t* Command, size_t CommandLength, const wchar_t* Arg, size_t ArgLen, bool asAdmin);
	HotKey(int id, HWND hWnd, UINT fsModifiers, UINT vk, wchar_t* Command, size_t CommandLength, bool asAdmin);
	HotKey(int id, HWND hWnd, UINT fsModifiers, UINT vk, wchar_t* Command, size_t CommandLength);
	HotKey();
	~HotKey();
	void ExecuteCommand();
	BOOL isPressedKeyMatch(UINT fsModifiers, UINT vk);
	BOOL Register();
	BOOL Unregister();

	HWND GetHWND()
	{
		return hWnd;
	};

	UINT GetFsModifiers()
	{
		return fsModifiers;
	};

	UINT GetVK()
	{
		return vk;
	};

	UINT GetAsAdmin()
	{
		return bRequireAdmin!=FALSE?1:0;
	};

	wchar_t* GetPath()
	{
		return Path;
	}

	wchar_t* GetArgs()
	{
		return Agrument;
	}

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

