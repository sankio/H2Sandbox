// dllmain.cpp : Defines the entry point for the DLL application.
#include "hookmanager.h"
#include "h2.h"
//Define Global Variables
CRITICAL_SECTION gDebugCritSec;
int icounter=0;
//Entry Point of DLL
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{   HookManager *MyHookEngine=new HookManager();
	BOOL status=FALSE;
	//Append First xml header to Logging Files
		if(icounter==0) 
	{
		pLogFile->WriteAnsi("<functions>");
		pLogReg->WriteAnsi("<registry>");
		icounter++;}
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		{
					InitializeCriticalSection(&gDebugCritSec);
					DisableThreadLibraryCalls(hModule);
					ScanExecutableModule();
					MyHookEngine->AddHookList("","ADVAPI32.dll","RegSetValueExA",(PROC)MyRegSetValueExA);
				//	MyHookEngine->AddHookList("","ADVAPI32.dll","RegSetValueW",(PROC)MyRegSetValueW);
					MyHookEngine->AddHookList("","ADVAPI32.dll","RegQueryValueExW",(PROC)MyRegQueryValueExW);
					MyHookEngine->AddHookList("","ADVAPI32.dll","RegCreateKeyW",(PROC)MyRegCreateKeyW);
					MyHookEngine->AddHookList("","ADVAPI32.dll","RegOpenKeyExA",(PROC)MyRegOpenKeyExA);
					MyHookEngine->AddHookList("","kernel32.dll","CreateFileW",(PROC)MyCreateFileW);
					MyHookEngine->AddHookList("","kernel32.dll","DeleteFileW",(PROC)MyDeleteFileW);
					MyHookEngine->AddHookList("","kernel32.dll","DeleteFileA",(PROC)MyDeleteFileA);
					MyHookEngine->AddHookList("","kernel32.dll","CreateFileA",(PROC)MyCreateFileA);
					MyHookEngine->AddHookList("","kernel32.dll","CreateProcessA",(PROC)MyCreateProcessA);
					MyHookEngine->AddHookList("","kernel32.dll","CopyFileA",(PROC)MyCopyFileA);	
					status=MyHookEngine->DoHook(Hook_All_Module);
		}
                        break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

