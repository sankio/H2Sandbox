#include "logger.h"
#include <Winternl.h>
#include <TlHelp32.h>
#include <Psapi.h>
#ifdef H2_EXPORTS
#define H2_API __declspec(dllexport)
#else
#define H2_API __declspec(dllimport)
#endif
#define MAX_NAME_PATH 10000

//Define Global Variable for H2.dll

time_t rawtime;
TCHAR szName[MAX_NAME_PATH]={0};

// Define Logging Files
//Log *pLogFile = new Log("C:\\file2.xml");
//Log *pLogReg = new Log("C:\\reg2.xml");
//Log *pLogProc = new Log("C:\\process2.xml");
Log *pLogReg=new Log("\\\\vboxsrv\\prethesis\\reg.xml");
Log *pLogProc=new Log("\\\\vboxsrv\\prethesis\\process.xml");
Log *pLogFile=new Log("\\\\vboxsrv\\prethesis\\file.xml");

//Define Typedef of Exported Functions by H2.dll
int SetDebugPrivileges();

void GetProcessId(IN PCHAR szExeName);

void ScanExecutableModule();

H2_API NTSTATUS WINAPI MyNtCreateFile (
    PHANDLE FileHandle,
    ACCESS_MASK DesiredAccess,
	POBJECT_ATTRIBUTES ObjectAttributes,
    PIO_STATUS_BLOCK IoStatusBlock,
    PLARGE_INTEGER AllocationSize OPTIONAL,
    ULONG FileAttributes,
    ULONG ShareAccess,
    ULONG CreateDisposition,
    ULONG CreateOptions,
    PVOID EaBuffer OPTIONAL,
    ULONG EaLength
    );
H2_API LONG WINAPI MyRegSetValueExA(
  __in      HKEY hKey,
  __in_opt  LPCSTR lpValueName,
  __reserved      DWORD Reserved, 
  __in      DWORD dwType,
  __in_opt      const BYTE *lpData,
  __in      DWORD cbData
);
H2_API LONG WINAPI MyRegSetValueW(
  __in      HKEY hKey,
  __in_opt  LPCWSTR lpSubKey,
  __in      DWORD dwType,
  __in      LPCWSTR lpData,
  __in      DWORD cbData
);
H2_API LONG WINAPI MyRegQueryValueExW(
  __in         HKEY hKey,  __in_opt     LPCWSTR lpValueName,
  __reserved   LPDWORD lpReserved,
  __out_opt    LPDWORD lpType,
  __out_opt    LPBYTE lpData,
  __inout_opt  LPDWORD lpcbData
) ;

char *convertToXMLFormatTime();

H2_API LONG WINAPI MyRegOpenKeyExA(
  __in        HKEY hKey,
  __in_opt    LPCSTR lpSubKey,  
  __reserved  DWORD ulOptions,
  __in        REGSAM samDesired,
  __out       PHKEY phkResult
) ;

H2_API HANDLE WINAPI MyCreateFileA(
  __in      LPCSTR lpFileName,
  __in      DWORD dwDesiredAccess,
  __in      DWORD dwShareMode,
  __in_opt  LPSECURITY_ATTRIBUTES lpSecurityAttributes,
  __in      DWORD dwCreationDisposition,
  __in      DWORD dwFlagsAndAttributes,
  __in_opt  HANDLE hTemplateFile
) ;

H2_API HANDLE WINAPI MyCreateFileW(
  __in      LPCWSTR lpFileName,
  __in      DWORD dwDesiredAccess,
  __in      DWORD dwShareMode,
  __in_opt  LPSECURITY_ATTRIBUTES lpSecurityAttributes,
  __in      DWORD dwCreationDisposition,
  __in      DWORD dwFlagsAndAttributes,
  __in_opt  HANDLE hTemplateFile
);
H2_API BOOL WINAPI MyDeleteFileW(
  __in      LPCWSTR lpFileName
);
H2_API BOOL WINAPI MyDeleteFileA(
  __in LPCSTR lpFileName
);
H2_API BOOL WINAPI MyCopyFileA(
  __in  LPCSTR lpExistingFileName,
  __in  LPCSTR lpNewFileName,
  __in  BOOL bFailIfExists
);
H2_API LONG WINAPI MyRegCreateKeyW(
  __in      HKEY hKey,
  __in_opt  LPCWSTR lpSubKey,
  __out     PHKEY phkResult
) ;

H2_API BOOL WINAPI MyCreateProcessA(
  __in_opt     LPCSTR lpApplicationName,
  __inout_opt  LPSTR lpCommandLine,
  __in_opt     LPSECURITY_ATTRIBUTES lpProcessAttributes,
  __in_opt     LPSECURITY_ATTRIBUTES lpThreadAttributes,
  __in         BOOL bInheritHandles,
  __in         DWORD dwCreationFlags,
  __in_opt     LPVOID lpEnvironment,
  __in_opt     LPCSTR lpCurrentDirectory,
  __in         LPSTARTUPINFOA lpStartupInfo,
  __out        LPPROCESS_INFORMATION lpProcessInformation
) ;


typedef NTSTATUS (WINAPI *NTCREATEFILE)( PHANDLE FileHandle,
    ACCESS_MASK DesiredAccess,
    POBJECT_ATTRIBUTES ObjectAttributes,
    OUT PIO_STATUS_BLOCK IoStatusBlock,
    IN PLARGE_INTEGER AllocationSize OPTIONAL,
    IN ULONG FileAttributes,
    IN ULONG ShareAccess,
    IN ULONG CreateDisposition,
    IN ULONG CreateOptions,
    IN PVOID EaBuffer OPTIONAL,
    IN ULONG EaLength);
NTCREATEFILE NtCreateFile;

//Define Export Functions
H2_API NTSTATUS WINAPI MyNtCreateFile (
    PHANDLE FileHandle,
    ACCESS_MASK DesiredAccess,
    POBJECT_ATTRIBUTES ObjectAttributes,
    OUT PIO_STATUS_BLOCK IoStatusBlock,
    IN PLARGE_INTEGER AllocationSize OPTIONAL,
    IN ULONG FileAttributes,
    IN ULONG ShareAccess,
    IN ULONG CreateDisposition,
    IN ULONG CreateOptions,
    IN PVOID EaBuffer OPTIONAL,
    IN ULONG EaLength
    )
{
	NTSTATUS status;
	HMODULE baseModule=LoadLibraryA("ntdll.dll");
	NtCreateFile=(NTCREATEFILE)GetProcAddress(baseModule,"NtCreateFile");
	status=NtCreateFile( FileHandle, DesiredAccess, ObjectAttributes, IoStatusBlock, AllocationSize OPTIONAL, FileAttributes, ShareAccess, CreateDisposition, CreateOptions, EaBuffer, EaLength);
	return status;
}

H2_API LONG WINAPI MyRegQueryValueExW(
  __in         HKEY hKey,  
  __in_opt     LPCWSTR lpValueName,
  __reserved   LPDWORD lpReserved,
  __out_opt    LPDWORD lpType,
  __out_opt    LPBYTE lpData,
  __inout_opt  LPDWORD lpcbData
) {
char buffer[100];
char *tmp=convertToXMLFormatTime();
sprintf(buffer,"<function call= \"RegQueryValueExW\" category=\"Registry\" time=\"%s\">",tmp);
pLogReg->WriteAnsi(buffer);
pLogReg->WriteUnicodeArgument2((LPVOID*)lpValueName,"LPCWSTR","lpValueName");
sprintf(buffer,"</function>","");
pLogReg->WriteAnsi(buffer);
return RegQueryValueExW(hKey,lpValueName,lpReserved,lpType,lpData,lpcbData);
} 

H2_API LONG WINAPI MyRegOpenKeyExA(
  __in        HKEY hKey,
  __in_opt    LPCSTR lpSubKey,  
  __reserved  DWORD ulOptions,
  __in        REGSAM samDesired,
  __out       PHKEY phkResult
) {
char buffer[100];
char *time=convertToXMLFormatTime();
sprintf(buffer,"<function call= \"RegOpenKeyExA\" category=\"Registry\" time=\"%s\">",time);
pLogReg->WriteAnsi(buffer);
pLogReg->WriteUnicodeArgument2((LPVOID*)hKey,"HKEY","hKey");
if(!lpSubKey==NULL) 
	pLogReg->WriteUnicodeArgument2((LPVOID*)lpSubKey,"LPCSTR","Sub Key");
pLogReg->WriteAnsi("</function>");
return RegOpenKeyExA(hKey,lpSubKey,ulOptions,samDesired,phkResult); 
}

H2_API BOOL WINAPI MyCopyFileA(
  __in  LPCSTR lpExistingFileName,
  __in  LPCSTR lpNewFileName,
  __in  BOOL bFailIfExists
) {
char buffer[100];
char *time=convertToXMLFormatTime();
sprintf(buffer,"<function call= \"CopyFileA\" category=\"File\" time=\"%s\">",time);
pLogFile->WriteAnsi(buffer);
pLogFile->WriteUnicodeArgument2((LPVOID*)lpExistingFileName,"LPCSTR","Original Name");
pLogFile->WriteUnicodeArgument2((LPVOID*)lpNewFileName,"LPCSTR","New Name");
pLogFile->WriteUnicodeArgument2((LPVOID*)bFailIfExists,"BOOL","Status");
pLogFile->WriteAnsi("</function>");
return CopyFileA(lpExistingFileName,lpNewFileName,bFailIfExists); 
 }

H2_API BOOL WINAPI MyCreateProcessA(
  __in_opt     LPCSTR lpApplicationName,
  __inout_opt  LPSTR lpCommandLine,
  __in_opt     LPSECURITY_ATTRIBUTES lpProcessAttributes,
  __in_opt     LPSECURITY_ATTRIBUTES lpThreadAttributes,
  __in         BOOL bInheritHandles,
  __in         DWORD dwCreationFlags,
  __in_opt     LPVOID lpEnvironment,
  __in_opt     LPCSTR lpCurrentDirectory,
  __in         LPSTARTUPINFOA lpStartupInfo,
  __out        LPPROCESS_INFORMATION lpProcessInformation
) {
char buffer[100];
char *time=convertToXMLFormatTime();
sprintf(buffer,"<function call= \"CreateProcessA\" category=\"Process\" time=\"%s\">",time);
pLogFile->WriteAnsi(buffer);
if(CreateProcessA(lpApplicationName,lpCommandLine,lpProcessAttributes,lpThreadAttributes,bInheritHandles,dwCreationFlags,lpEnvironment,lpCurrentDirectory,lpStartupInfo,lpProcessInformation)) 
{	if(!lpApplicationName==NULL) 
	pLogFile->WriteUnicodeArgument2((LPVOID*)lpApplicationName,"LPCSTR","Application Name");
	if(!lpCommandLine==NULL) 
	pLogFile->WriteUnicodeArgument2((LPVOID*)lpCommandLine,"LPCSTR","Command Line");
//pLog->WriteUnicodeArgument((LPVOID*)lpProcessAttributes,"LPSECURITY_ATTRIBUTES");
//pLog->WriteUnicodeArgument((LPVOID*)lpThreadAttributes,"LPSECURITY_ATTRIBUTES");
	pLogFile->WriteUnicodeArgument2((LPVOID*)bInheritHandles,"BOOL","Inherit Handles");
	pLogFile->WriteUnicodeArgument2((LPVOID*)dwCreationFlags,"DWORD","Creation Flags");
//pLog->WriteUnicodeArgument((LPVOID*)lpEnvironment,"LPVOID");
	if(!lpCurrentDirectory==NULL) 
	pLogFile->WriteUnicodeArgument2((LPVOID*)lpCurrentDirectory,"LPCSTR","Current Directory");
//pLog->WriteUnicodeArgument((LPVOID*)lpStartupInfo,"LPSTARTUPINFO");
	pLogFile->WriteUnicodeArgument2((LPVOID*)lpProcessInformation->dwProcessId,"DWORD","Process ID");
	pLogFile->WriteAnsi("</function>");
	return TRUE;
}
else {
		pLogFile->WriteAnsi("</function>");
		return FALSE;
	}
}

H2_API LONG WINAPI MyRegSetValueA(
  __in      HKEY hKey,
  __in_opt  LPCSTR lpSubKey,
  __in      DWORD dwType,
  __in      LPCSTR lpData,
  __in      DWORD cbData
)
{
	char buffer[100];
char *time=convertToXMLFormatTime();
sprintf(buffer,"<function call= \"RegSetValueA\" category=\"Registry\" time=\"%s\">",time);
pLogReg->WriteAnsi(buffer);
pLogReg->WriteUnicodeArgument2((LPVOID*)hKey,"HKEY","hKey");
//pLogReg->WriteUnicodeArgument2((LPVOID*)lpSubKey,"LPCSTR","Sub Key");
if(!lpSubKey==NULL)
	pLogReg->WriteUnicodeArgument2((LPVOID*)lpSubKey,"LPCSTR","Sub Key");
if(!lpData==NULL) 
	pLogReg->WriteUnicodeArgument2((LPVOID*)lpData,"LPCSTR","Data");
pLogReg->WriteUnicodeArgument2((LPVOID*)dwType,"DWORD","Data Type");
pLogReg->WriteAnsi("</function>");
return MyRegSetValueA(hKey,lpSubKey,dwType,lpData,cbData);
}

H2_API LONG WINAPI MyRegSetValueExA(
  __in      HKEY hKey,
  __in_opt  LPCSTR lpValueName,
  __reserved      DWORD Reserved, 
  __in      DWORD dwType,
  __in_opt      const BYTE *lpData,
  __in      DWORD cbData
)
{
	char buffer[100];
char *time=convertToXMLFormatTime();
sprintf(buffer,"<function call= \"RegSetValueExA\" category=\"Registry\" time=\"%s\">",time);
pLogReg->WriteAnsi(buffer);
pLogReg->WriteUnicodeArgument2((LPVOID*)hKey,"HKEY","hKey");
if(!lpValueName==NULL)
	pLogReg->WriteUnicodeArgument2((LPVOID*)lpValueName,"LPCSTR","Value Name");
pLogReg->WriteUnicodeArgument2((LPVOID*)dwType,"DWORD","Data Type");
pLogReg->WriteAnsi("</function>");
return RegSetValueExA(hKey,lpValueName,Reserved,dwType,lpData,cbData);
}

H2_API BOOL WINAPI MyCreateProcessW(
  __in_opt     LPCWSTR lpApplicationName,
  __inout_opt  LPWSTR lpCommandLine,
  __in_opt     LPSECURITY_ATTRIBUTES lpProcessAttributes,
  __in_opt     LPSECURITY_ATTRIBUTES lpThreadAttributes,
  __in         BOOL bInheritHandles,
  __in         DWORD dwCreationFlags,
  __in_opt     LPVOID lpEnvironment,
  __in_opt     LPCWSTR lpCurrentDirectory,
  __in         LPSTARTUPINFOW lpStartupInfo,
  __out        LPPROCESS_INFORMATION lpProcessInformation
) {
char buffer[100];
time(&rawtime);
sprintf(buffer,"------CreateProcessW-------%s",ctime(&rawtime));
pLogProc->WriteAnsi(buffer);
pLogProc->WriteUnicodeArgument2((LPVOID*)lpApplicationName,"LPCWSTR","Application Name");
pLogProc->WriteUnicodeArgument2((LPVOID*)lpCommandLine,"LPCWSTR","Command Line");
//pLogProc->WriteUnicodeArgument((LPVOID*)lpProcessAttributes,"LPSECURITY_ATTRIBUTES");
//pLogProc->WriteUnicodeArgument((LPVOID*)lpThreadAttributes,"LPSECURITY_ATTRIBUTES");
pLogProc->WriteUnicodeArgument2((LPVOID*)bInheritHandles,"BOOL","Inherit Handle");
pLogProc->WriteUnicodeArgument2((LPVOID*)dwCreationFlags,"DWORD","Creation Flag");
//pLogProc->WriteUnicodeArgument((LPVOID*)lpEnvironment,"LPVOID");
pLogProc->WriteUnicodeArgument2((LPVOID*)lpCurrentDirectory,"LPCWSTR","Current Directory");
//pLogProc->WriteUnicodeArgument((LPVOID*)lpStartupInfo,"LPSTARTUPINFO");
//pLogProc->WriteUnicodeArgument((LPVOID*)lpProcessInformation,"LPPROCESS_INFORMATION");
pLogFile->WriteAnsi("</function>");
return CreateProcessW(lpApplicationName,lpCommandLine,lpProcessAttributes,lpThreadAttributes,bInheritHandles,dwCreationFlags,lpEnvironment,lpCurrentDirectory,lpStartupInfo,lpProcessInformation); }

char *convertToXMLFormatTime()
{
	time(&rawtime);
	char *tmp=ctime(&rawtime);
	int iLength=strlen(tmp);
	tmp[iLength-1]=' '; // Replace terminate string '\0'
	return tmp;
}

H2_API HANDLE WINAPI MyCreateFileW(
  __in      LPCWSTR lpFileName,
  __in      DWORD dwDesiredAccess,
  __in      DWORD dwShareMode,
  __in_opt  LPSECURITY_ATTRIBUTES lpSecurityAttributes,
  __in      DWORD dwCreationDisposition,
  __in      DWORD dwFlagsAndAttributes,
  __in_opt  HANDLE hTemplateFile
) {
char buffer[100];
char *time=convertToXMLFormatTime();
sprintf(buffer,"<function call= \"CreateFileW\" category=\"File\" time=\"%s\">",time);
pLogFile->WriteAnsi(buffer);
pLogFile->WriteUnicodeArgument2((LPVOID*)lpFileName,"LPCWSTR","File Name");
pLogFile->WriteUnicodeArgument2((LPVOID*)dwDesiredAccess,"DWORD","Desired Access");
pLogFile->WriteUnicodeArgument2((LPVOID*)dwShareMode,"DWORD","Share Mode");
//pLogFile->WriteUnicodeArgument((LPVOID*)lpSecurityAttributes,"LPSECURITY_ATTRIBUTES");
pLogFile->WriteUnicodeArgument2((LPVOID*)dwCreationDisposition,"DWORD","Creation/Disposition");
pLogFile->WriteUnicodeArgument2((LPVOID*)dwFlagsAndAttributes,"DWORD","Flags/Attributes");
pLogFile->WriteAnsi("</function>");
return CreateFileW(lpFileName,dwDesiredAccess,dwShareMode,lpSecurityAttributes,dwCreationDisposition,dwFlagsAndAttributes,hTemplateFile); }


H2_API HANDLE WINAPI MyCreateFileA(
  __in      LPCSTR lpFileName,
  __in      DWORD dwDesiredAccess,
  __in      DWORD dwShareMode,
  __in_opt  LPSECURITY_ATTRIBUTES lpSecurityAttributes,
  __in      DWORD dwCreationDisposition,
  __in      DWORD dwFlagsAndAttributes,
  __in_opt  HANDLE hTemplateFile
) {
char buffer[100];
char *time=convertToXMLFormatTime();
sprintf(buffer,"<function call= \"CreateFileA\" category=\"File\" time=\"%s\">",time);
pLogFile->WriteAnsi(buffer);
pLogFile->WriteUnicodeArgument2((LPVOID*)lpFileName,"LPCSTR","File Name");
pLogFile->WriteUnicodeArgument2((LPVOID*)dwDesiredAccess,"DWORD","Desired Access");
pLogFile->WriteUnicodeArgument2((LPVOID*)dwShareMode,"DWORD","Share Mode");
//pLogFile->WriteUnicodeArgument((LPVOID*)lpSecurityAttributes,"LPSECURITY_ATTRIBUTES");
pLogFile->WriteUnicodeArgument2((LPVOID*)dwCreationDisposition,"DWORD","Creation/Disposition");
pLogFile->WriteUnicodeArgument2((LPVOID*)dwFlagsAndAttributes,"DWORD","Flags/Attributes");
pLogFile->WriteAnsi("</function>");
return CreateFileA(lpFileName,dwDesiredAccess,dwShareMode,lpSecurityAttributes,dwCreationDisposition,dwFlagsAndAttributes,hTemplateFile); }

H2_API LONG WINAPI MyRegCreateKeyW(
  __in      HKEY hKey,
  __in_opt  LPCWSTR lpSubKey,
  __out     PHKEY phkResult
) {
char buffer[100];
char *time=convertToXMLFormatTime();
sprintf(buffer,"<function call= \"RegCreateKeyW\" category=\"Registry\" time=\"%s\">",time);
pLogReg->WriteAnsi(buffer);
pLogReg->WriteUnicodeArgument2((LPVOID*)hKey,"HKEY","Hkey");
pLogReg->WriteUnicodeArgument2((LPVOID*)lpSubKey,"LPCWSTR","Sub Key");
sprintf(buffer,"</function>","");
pLogReg->WriteAnsi(buffer);
return RegCreateKeyW(hKey,lpSubKey,phkResult); 
}

H2_API BOOL WINAPI MyDeleteFileW(
  __in LPCWSTR lpFileName
) {
char buffer[100];
char *time=convertToXMLFormatTime();
sprintf(buffer,"<function call= \"DeleteFileW\" category=\"File\" time=\"%s\">",time);
pLogFile->WriteAnsi(buffer);
pLogFile->WriteUnicodeArgument2((LPVOID*)lpFileName,"LPCWSTR","File Name");
sprintf(buffer,"</function>","");
pLogFile->WriteAnsi(buffer);
return DeleteFileW(lpFileName); 
}
H2_API BOOL WINAPI MyDeleteFileA(
  __in LPCSTR lpFileName
) {
char buffer[100];
char *time=convertToXMLFormatTime();
sprintf(buffer,"<function call= \"DeleteFileA\" category=\"File\" time=\"%s\">",time);
pLogFile->WriteAnsi(buffer);
pLogFile->WriteUnicodeArgument2((LPVOID*)lpFileName,"LPCSTR","File Name");
sprintf(buffer,"</function>","");
pLogFile->WriteAnsi(buffer);
return DeleteFileA(lpFileName); 
}
int SetDebugPrivileges()
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
void GetProcessId(IN PCHAR szExeName)

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
          //  sprintf(cbuff,"
			bRet = Process32Next(hSnapshot, &pe);
        }

        if (dwCount > 1)
            dwRet = 0xFFFFFFFF;

        CloseHandle(hSnapshot);
    }

}


void ScanExecutableModule()
{
	char cbuff[MAX_NAME_PATH];
	DWORD dCountBytes;
	DWORD PID;
	HMODULE hModuleArray[2048];
	MODULEINFO mInfo;
	HANDLE hProcess;
	char *cModuleName[MAX_NAME_PATH];

	PID=GetCurrentProcessId();
					if(SetDebugPrivileges()!=0)
					{
					//	pLogProc->WriteAnsi("SetDebug failed");
					}
					hProcess=OpenProcess(PROCESS_ALL_ACCESS,FALSE,PID);
					if(hProcess==0)
					{
					//	pLogProc->WriteAnsi("hProcess failed");
					}
					//Give a handle to process, return all the modules running within the process
					if(EnumProcessModules( hProcess, hModuleArray, sizeof(hModuleArray),&dCountBytes))
					{
						//Return the actual number of modules
						dCountBytes /=sizeof(hModuleArray[0]);
						for(int j=0;j<dCountBytes;j++)
						{
							GetModuleFileNameEx(hProcess,hModuleArray[j], szName, MAX_NAME_PATH);
							GetModuleInformation(hProcess,hModuleArray[j],&mInfo,sizeof(MODULEINFO));
							pLogProc->UnicodeToAnsi(szName,&(cModuleName[0]));
							sprintf(cbuff,"<dll link=\"%s\" size=\"%d\" start=\"%p\"></dll>",cModuleName[0],mInfo.SizeOfImage,mInfo.lpBaseOfDll);
							pLogProc->WriteAnsi(cbuff);
						}
					}

}
