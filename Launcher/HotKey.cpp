#include "HotKey.h"



HotKey::HotKey(int id, UINT fsModifiers,UINT vk, wchar_t* Command, size_t CommandLength, wchar_t* Arg, size_t ArgLen, bool asAdmin)
{
	this->DbId = id;
	this->fsModifiers = fsModifiers;
	this->vk = vk;

	ZeroMemory(this->Path, MAX_PATH * sizeof(wchar_t));
	ZeroMemory(this->Agrument, MAX_PATH * sizeof(wchar_t));

	if (CommandLength != 0 && CommandLength<MAX_PATH && Command != nullptr)
		CopyMemory(this->Path, Command, CommandLength*sizeof(wchar_t));

	if (ArgLen != 0 && ArgLen < MAX_PATH && Arg != nullptr)
		CopyMemory(this->Agrument, Arg, ArgLen * sizeof(wchar_t));
	
	this->bRequireAdmin = asAdmin;
	this->hWnd = 0;
	this->HotKeyRegistrationId = 0;

	this->isHotKeyRegistered = false;
}

HotKey::HotKey(int id, UINT fsModifiers, UINT vk, wchar_t* Command, size_t CommandLength, bool asAdmin) 
	: HotKey(id, fsModifiers, vk, Command, CommandLength, nullptr, 0, asAdmin)
{

}

HotKey::HotKey(int id, UINT fsModifiers, UINT vk, wchar_t* Command, size_t CommandLength)
	: HotKey(id, fsModifiers, vk, Command, CommandLength, nullptr, 0, false)
{

}

HotKey::HotKey(UINT fsModifiers, UINT vk, wchar_t* Command, size_t CommandLength, bool asAdmin)
	: HotKey(0, fsModifiers, vk, Command, CommandLength, nullptr, 0, asAdmin)
{

}

HotKey::HotKey(UINT fsModifiers, UINT vk, wchar_t* Command, size_t CommandLength)
	: HotKey(0, fsModifiers, vk, Command, CommandLength, nullptr, 0, false)
{

}

HotKey::HotKey()
	: HotKey(0, 0, 0, (wchar_t*)NULL, 0, (wchar_t*)NULL, 0, false)
{

}

HotKey::~HotKey()
{
	if(this!=nullptr && isHotKeyRegistered)
		UnregisterHotKey(hWnd, HotKeyRegistrationId);
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
	if (fsModifiers==this->fsModifiers && id==this->HotKeyRegistrationId)
	{
		return TRUE;
	}
	return FALSE;
}

BOOL HotKey::Register(HWND Window, int id)
{
	this->hWnd = Window;
	this->HotKeyRegistrationId = id;
	if (hWnd!= INVALID_HANDLE_VALUE)
		this->isHotKeyRegistered = RegisterHotKey(hWnd, id, fsModifiers, vk);
	return this->isHotKeyRegistered;
}

BOOL HotKey::Unregister()
{
	this->isHotKeyRegistered = ~UnregisterHotKey(hWnd, HotKeyRegistrationId);
	return this->isHotKeyRegistered;
}