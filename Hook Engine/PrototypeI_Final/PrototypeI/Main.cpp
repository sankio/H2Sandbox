#include <windows.h>
#include <DbgHelp.h>
#include <TlHelp32.h>
#include <tchar.h>
#include "NTProcessInfo.h"
#include "ntundoc.h"
//#define App "C:\\target.exe"
#define App "\\\\vboxsrv\\sharedmal\\target.exe"
#include "logger.h"

CONTEXT orig_context={0};
DWORD GetProcessEntryPointAddress( HANDLE hProcess, HANDLE hThread );
DWORD GetMainThreadByPID(DWORD dwProcessID);
DWORD GetProcessId(IN PCHAR szExeName);
BOOL CreateRemoteThreadInject(HANDLE hProcess,DWORD ID, const char * dll) ;
int SetDebugPrivileges(HANDLE);
#pragma pack(push,1)
typedef struct _optional_header
{
	unsigned short magic;
	char linker_version_major;
	char linker_version_minor;
	unsigned int code_size;
	unsigned int idata_size;
	unsigned int udate_size;
	unsigned int entry_point;
	unsigned int code_base;
}optional_header;
typedef struct _coff_header
{
	unsigned short machine;
	unsigned short sections;
	unsigned int timestamp;
	unsigned int symboltable;
	unsigned int symbols;
	unsigned short size_of_opt_table;
	unsigned short characteristics;
}coff_header;
#pragma pack(pop)
//
// Gets the address of the entry point routine given a
// handle to a process and its primary thread.
//
DWORD GetProcessEntryPointAddress( HANDLE hProcess, HANDLE hThread )
{
	
	CONTEXT				context={0};
	DWORD				Seg;
	LDT_ENTRY			entry;
	stTEB					teb;
	stPEB					peb;
	DWORD				read;
	DWORD				dwFSBase;
	DWORD				dwImageBase, dwOffset;
	DWORD				dwOptHeaderOffset;
	optional_header		opt;
DWORD err=0;
//LPVOID dwEip;
	//
	// get the current thread context
	//
	context.ContextFlags = CONTEXT_SEGMENTS|CONTEXT_CONTROL|CONTEXT_FULL;
	GetThreadContext( hThread, &context );
err=GetLastError();
orig_context=context;
		Seg=context.SegFs;
	//
	// use the segment register value to get a pointer to
	// the TEB
	//
	GetThreadSelectorEntry( hThread, Seg, &entry );
	dwFSBase = ( entry.HighWord.Bits.BaseHi << 24 ) |
					 ( entry.HighWord.Bits.BaseMid << 16 ) |
					 ( entry.BaseLow );

	//
	// read the teb
	//
	ReadProcessMemory( hProcess, (LPCVOID)dwFSBase, &teb, sizeof( stTEB ), &read );
	
	//
	// read the peb from the location pointed at by the teb
	//
	ReadProcessMemory( hProcess, (LPCVOID)teb.Peb, &peb, sizeof( stPEB ), &read );

	//
	// figure out where the entry point is located;
	//
	
	dwImageBase = (DWORD)peb.ImageBaseAddress;
	PVOID ab=(PVOID)(dwImageBase+0x3c);
	ReadProcessMemory( hProcess, (LPCVOID)( dwImageBase + 0x3c ), &dwOffset, sizeof( DWORD ), &read );
	DWORD dwCoff=(dwImageBase+dwOffset);
	coff_header coff;
	ReadProcessMemory( hProcess, (LPCVOID)dwCoff, &coff, sizeof(coff_header), &read );
	
	dwOptHeaderOffset = ( dwImageBase + dwOffset + 4 + sizeof( coff_header ) );
	ReadProcessMemory( hProcess, (LPCVOID)dwOptHeaderOffset, &opt, sizeof( optional_header ), &read );
	
	return ( dwImageBase + opt.entry_point );
}


int SetDebugPrivileges(HANDLE CurrentProcess)
{
	TOKEN_PRIVILEGES Debug_Privileges;
	
	//STEP 1
	if (!LookupPrivilegeValue (NULL, // Privieleges for the local system
								SE_DEBUG_NAME, // define the name of the privilege 
                               &Debug_Privileges.Privileges[0].Luid)) // will get the LUID value into this variable
   {	//if function failed, cannot proceed to the next step
		return GetLastError(); //terminate the outer function
   }

	//STEP 2
	DWORD err = 0; // define error holder, used to store the error code in case of failure
	HANDLE hToken = 0; //  instantiate a token handle 
	if (!OpenProcessToken (GetCurrentProcess() , // current process ID handle
                           TOKEN_ADJUST_PRIVILEGES, //set the desired access
                           &hToken)) // handle to the token will be held here 
   {	// if function failed, cannot proceed to the next step
		err = GetLastError();  
		if (hToken) // if handle is still valid
        	CloseHandle (hToken); // destroy it
		return err; //terminate the outer function
   }

	//STEP3
	Debug_Privileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; // set to enable privilege
	Debug_Privileges.PrivilegeCount = 1; // working with only one privilege

	if (!AdjustTokenPrivileges (hToken, // access token handle
								FALSE, // do not disable privileges
								&Debug_Privileges, // pointer to the token structure
								0,  // no need for a buffer 
								NULL, // previous state not set
								NULL)) //  no need for a buffer
   {
        err = GetLastError();
		if (hToken) // if handle is still valid
        CloseHandle (hToken); // destroy it
		return err; //terminate the outer function
   }

	return err; 
}
DWORD GetMainThreadByPID(DWORD dwProcessID)
{
	THREADENTRY32 ThreadEntry;
	ThreadEntry.dwSize = sizeof(THREADENTRY32);
	HANDLE hThreadSnapshot=CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD,dwProcessID);
	if(Thread32First(hThreadSnapshot, &ThreadEntry))
	{
		do
		{
			if(ThreadEntry.th32OwnerProcessID==dwProcessID)
			{
				CloseHandle(hThreadSnapshot);
				return ThreadEntry.th32ThreadID;
			}
		}
		while(Thread32Next(hThreadSnapshot,&ThreadEntry));
		CloseHandle(hThreadSnapshot);
	}
	return NULL;
}


DWORD GetProcessId(IN PCHAR szExeName)

{
    DWORD dwRet = 0;
    DWORD dwCount = 0;

    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (hSnapshot != INVALID_HANDLE_VALUE)
    {
        PROCESSENTRY32 pe = {0};
        pe.dwSize = sizeof(PROCESSENTRY32);

        BOOL bRet = Process32First(hSnapshot, &pe);

        while (bRet)
        {
            if (!strcmp(pe.szExeFile,_T("target.exe")))
            {
                dwCount++;
                dwRet = pe.th32ProcessID;
            }
            bRet = Process32Next(hSnapshot, &pe);
        }

        if (dwCount > 1)
            dwRet = 0xFFFFFFFF;

        CloseHandle(hSnapshot);
    }

    return dwRet;
}
typedef NTSTATUS(NTAPI *pfnNtQueryInformationProcess)
	(
	HANDLE ProcessHandle,
	PROCESSINFOCLASS ProcessInformationClass,
	PVOID ProcessInformation,
	ULONG ProcessInformationLength,
	PULONG ReturnLength OPTIONAL
	);



int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevinstance, LPSTR lpComdLine,int ShowCmd)
{
	char buff1[2];
	DWORD err=0;
	PROCESS_INFORMATION processInformation;
	STARTUPINFO startupInfo;
	char buff[4096];
	smPROCESSINFO spi ={0};
	smPEB peb={0};
	smPEB_LDR_DATA peb_ldr={0};
	//HANDLE hHeap;
	
	ZeroMemory(&spi,sizeof(spi));
	ZeroMemory(&peb,sizeof(peb));
	ZeroMemory(&peb_ldr,sizeof(peb_ldr));

	DWORD bytes;
	HANDLE file=CreateFile(App,GENERIC_READ|GENERIC_WRITE,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
	err=GetLastError();
	ReadFile(file,buff,1024,&bytes,0);
	err=GetLastError();
	CloseHandle(file);

	IMAGE_DOS_HEADER* dosheader=(IMAGE_DOS_HEADER*) buff;
	IMAGE_OPTIONAL_HEADER *optionalheader=(IMAGE_OPTIONAL_HEADER*)((BYTE*)buff+dosheader->e_lfanew+24);
	DWORD entryptr=optionalheader->AddressOfEntryPoint+optionalheader->ImageBase;

	memset(&processInformation,0,sizeof(processInformation));
	memset(&startupInfo,0,sizeof(startupInfo));BOOL result=false;
	result = CreateProcess(App,NULL,NULL,NULL,TRUE,CREATE_DEFAULT_ERROR_MODE|CREATE_SUSPENDED, NULL,NULL, &startupInfo,&processInformation);
	err=GetLastError();
	
	

//----Get Entry Address
	DWORD PID =GetProcessId(App);
	int status=0;
	//spi.dwPID =PID;
	//Attemp to access process
	HANDLE hProcess=OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ,FALSE,PID);
	if(hProcess==INVALID_HANDLE_VALUE)
	{
		return false;
	}
	status=SetDebugPrivileges(hProcess);
	hProcess=OpenProcess(PROCESS_ALL_ACCESS,FALSE,PID);
	
	HANDLE hThread=OpenThread(THREAD_ALL_ACCESS,FALSE,GetMainThreadByPID(PID));
	err=GetLastError();
	
	DWORD ImageBase=GetProcessEntryPointAddress(hProcess,hThread);
//Add while(1)
	PVOID k =(PVOID)ImageBase;
	char buff2[4];
	DWORD read;
	ReadProcessMemory( hProcess, k, buff2, 4, &read );
	buff1[0]=(char)0xEB; // PUSH opcodes is 0x68
	buff1[1]=(char)0xFE;
	WriteProcessMemory(hProcess,k,buff1,4,0);
	err=GetLastError();
	ResumeThread(hThread);
	Sleep(2000);

//----CreateRemoteThread----------//
	char dll[MAX_PATH];

	//Get the full path of our .dll
   GetFullPathNameA("H2.dll",MAX_PATH,dll,NULL); 
if (!CreateRemoteThreadInject(hProcess,PID,dll))
	{
		//If CreateRemoteThreadInject Returned true
		//printf("Injection failed!");
		Sleep(3000);
		exit(1);
		
	}
	else
	{
		//If CreateRemoteThreadInject Returned true
		//printf("Injection Successful!");
		Sleep(3000);
		
	}
//----Suspend Again And Replace 2 bytes

	DWORD ThreadStatus= SuspendThread(hThread);
	SetThreadContext(hThread, &orig_context);
	WriteProcessMemory(hProcess,k,buff2,4,&read);
	err=GetLastError();
	ResumeThread(hThread);

	return 0;
}
BOOL CreateRemoteThreadInject(HANDLE hProcess,DWORD ID, const char * dll) 

{ 
DWORD err=0;
FARPROC Load;
LPVOID Memory;

   HINSTANCE LoadME = LoadLibrary(_T("kernel32.dll"));
   Load = (FARPROC)GetProcAddress(LoadME, "LoadLibraryA"); 

   err=GetLastError();
   // Allocate space in the process for our DLL 
   Memory = (LPVOID)VirtualAllocEx(hProcess, NULL, strlen(dll)+1, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE); 
  
   err=GetLastError();
   // Write the string name of our DLL in the memory allocated 
  int result =  WriteProcessMemory(hProcess, (LPVOID)Memory, dll, strlen(dll)+1, NULL); 
   
        if (result == 0) 
        {
                printf("Error writing the library name to the remote process.\n");
                return -1;
        }

  err=GetLastError();
   // Load our DLL 
  HANDLE hThread=CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)Load, (LPVOID)Memory, NULL,NULL); 
  
  err=GetLastError();
  
  WaitForSingleObject(hThread, INFINITE);


   //Let the program regain control of itself


   //Free the allocated memory
   VirtualFreeEx(hProcess , (LPVOID)Memory , 0, MEM_RELEASE);

   return true;
 
} 
