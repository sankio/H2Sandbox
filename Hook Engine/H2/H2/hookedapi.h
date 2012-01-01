#include <windows.h>
#include <stdio.h>

#define MAX_NUMBER_OF_FUNC 24

struct  HookedAPIInfo
{
  LPCSTR hModuleName;
  LPCSTR FuncName;
  LPCSTR DLLName;
  LPVOID OrigAddressOfFunc;
  LPVOID NewAddressOfFunc;
  BOOL HookedStatus;
} ;

class MyHookedAPI {
	HookedAPIInfo MyHookedAPIInfo[MAX_NUMBER_OF_FUNC];
public: int g_Counter;
public:	
		MyHookedAPI::MyHookedAPI(); // constructor
BOOL	MyHookedAPI::SetHookedAPI(LPCSTR hModuleName,LPCSTR funcName,LPCSTR DLLName, PROC old_addressOfFunc, PROC new_addressOfFunc,BOOL status);
LPCSTR	MyHookedAPI::GetDLLName(int);
LPCSTR	MyHookedAPI::GetFuncName(int index);
LPCSTR	MyHookedAPI::GetModuleName(int);
BOOL	MyHookedAPI::SetModuleName(int, LPCSTR);
LPCSTR	MyHookedAPI::GetAddress(int index);
LPCSTR	MyHookedAPI::GetHookedStatus(int index);
PROC	MyHookedAPI::GetOrigAddressOfFunc(int index);
PROC	MyHookedAPI::GetNewAddressOfFunc(int index);
void	MyHookedAPI::SetHookedStatus(int index);

};