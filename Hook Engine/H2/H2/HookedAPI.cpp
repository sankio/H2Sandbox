#include <windows.h>
#include "hookedapi.h"

MyHookedAPI::MyHookedAPI()
{
	//Initialize the struct
	ZeroMemory(MyHookedAPIInfo, MAX_NUMBER_OF_FUNC*sizeof(HookedAPIInfo));
	g_Counter=0;
}
BOOL MyHookedAPI::SetHookedAPI(LPCSTR hModuleName,LPCSTR funcName, LPCSTR dllName, PROC old_addressOfFunc, PROC new_addressOfFunc,BOOL status)
{
	MyHookedAPIInfo[g_Counter].DLLName=dllName;
	MyHookedAPIInfo[g_Counter].FuncName;
	MyHookedAPIInfo[g_Counter].hModuleName=hModuleName;
	MyHookedAPIInfo[g_Counter].OrigAddressOfFunc=(LPVOID)old_addressOfFunc;
	MyHookedAPIInfo[g_Counter].NewAddressOfFunc=(LPVOID)new_addressOfFunc;
	MyHookedAPIInfo[g_Counter].HookedStatus=status;
	g_Counter++;
	return TRUE;
}
void	MyHookedAPI::SetHookedStatus(int index)
{
	MyHookedAPIInfo[index].HookedStatus=TRUE;
}
BOOL	MyHookedAPI::SetModuleName(int index,LPCSTR str)
{
	if(index<24){
			MyHookedAPIInfo[index].hModuleName=str;
			return true;
	}
	else return false;
}
LPCSTR MyHookedAPI::GetDLLName(int index)
{
	return MyHookedAPIInfo[index].DLLName;
}
LPCSTR MyHookedAPI::GetFuncName(int index)
{
	return MyHookedAPIInfo[index].FuncName;
}
LPCSTR	MyHookedAPI::GetModuleName(int index)
{
	return MyHookedAPIInfo[index].hModuleName;
}
PROC	MyHookedAPI::GetOrigAddressOfFunc(int index)
{
	return (PROC)MyHookedAPIInfo[index].OrigAddressOfFunc;
}
PROC	MyHookedAPI::GetNewAddressOfFunc(int index)
{
	return (PROC)MyHookedAPIInfo[index].NewAddressOfFunc;
}