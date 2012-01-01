#include <windows.h>
#include "hookedapi.h"
#include <DbgHelp.h>
#include <TlHelp32.h>

#define Hook_Specified_Module 0
#define Hook_All_Module 1
#define GetAddressOfInjectedFunction(cast, x, y) (cast) ( (DWORD_PTR)(x) - (DWORD_PTR)(y)) 

class HookManager {
	MyHookedAPI *MyHookedAPIList;
public:
		 HookManager::HookManager();
	PROC HookManager::HookInOneModule(HMODULE, LPCSTR, PROC, PROC);
	void HookManager::HookInManyModule();
	BOOL HookManager::AddHookList(LPCSTR ,LPCSTR , LPCSTR, PROC );
	void HookManager::GetHookList();
	BOOL HookManager::DoHook(int);
	PROC HookManager::HookExportTable(LPCSTR, PROC, PROC);
};