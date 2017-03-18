#include "HotKey.h"



HotKey::HotKey(int id, HWND hWnd,UINT fsModifiers,UINT vk, wchar_t* Command, size_t CommandLength, wchar_t* Arg, size_t ArgLen, bool asAdmin)
{
	this->id = id;
	this->hWnd = hWnd;
	this->fsModifiers = fsModifiers;
	this->vk = vk;

	ZeroMemory(this->Path, MAX_PATH * sizeof(wchar_t));
	ZeroMemory(this->Agrument, MAX_PATH * sizeof(wchar_t));

	if (CommandLength != 0 && CommandLength<MAX_PATH && Command != nullptr)
		CopyMemory(this->Path, Command, CommandLength);

	if (ArgLen != 0 && ArgLen < MAX_PATH && Arg != nullptr)
		CopyMemory(this->Agrument, Arg, ArgLen);
	
	this->bRequireAdmin = asAdmin;

	this->isHotKeyRegistered = false;
}

HotKey::HotKey(int id, HWND hWnd, UINT fsModifiers, UINT vk, wchar_t* Command, size_t CommandLength, bool asAdmin) 
	: HotKey(id, hWnd, fsModifiers, vk, Command, CommandLength, nullptr, 0, asAdmin)
{

}

HotKey::HotKey(int id, HWND hWnd, UINT fsModifiers, UINT vk, wchar_t* Command, size_t CommandLength)
	: HotKey(id, hWnd, fsModifiers, vk, Command, CommandLength, nullptr, 0, false)
{

}

HotKey::HotKey()
	: HotKey(0, 0, 0, 0, (wchar_t*)NULL, 0, (wchar_t*)NULL, 0, false)
{

}

HotKey::~HotKey()
{
	if(isHotKeyRegistered)
		UnregisterHotKey(hWnd, id);
}

void HotKey::ExecuteCommand()
{
	SHELLEXECUTEINFO shExInfo = { 0 };
	shExInfo.cbSize = sizeof(shExInfo);
	shExInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	shExInfo.hwnd = 0;
	if (this->bRequireAdmin)
		shExInfo.lpVerb = TEXT("runas");
	else
		shExInfo.lpVerb = TEXT("open");										// Operation to perform
	shExInfo.lpFile = this->Path;       // Application to start    
	shExInfo.lpParameters = this->Agrument;                  // Additional parameters
	shExInfo.lpDirectory = 0;
	shExInfo.nShow = SW_SHOW;
	shExInfo.hInstApp = 0;

	ShellExecuteEx(&shExInfo);
}

BOOL HotKey::isPressedKeyMatch(UINT fsModifiers, UINT id)
{
	if (fsModifiers==this->fsModifiers && id==this->id)
	{
		return TRUE;
	}
	return FALSE;
}

BOOL HotKey::Register()
{
	this->isHotKeyRegistered = RegisterHotKey(hWnd, id, fsModifiers, vk);
	return this->isHotKeyRegistered;
}

BOOL HotKey::Unregister()
{
	this->isHotKeyRegistered = ~UnregisterHotKey(hWnd, id);
	return this->isHotKeyRegistered;
}