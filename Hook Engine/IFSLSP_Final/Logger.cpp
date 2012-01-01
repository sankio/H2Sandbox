#include "logger.h"
/* * UnicodeToAnsi converts the Unicode string pszW to an ANSI string

   * and returns the ANSI string through ppszA. Space for the

   * the converted string is allocated by UnicodeToAnsi.

   */

 
HRESULT __fastcall Log::UnicodeToAnsi(LPCOLESTR pszW, LPSTR* ppszA){

    ULONG cbAnsi, cCharacters;

    DWORD dwError;

    // If input is null then just return the same.   

    if (pszW == NULL)   

    {

        *ppszA = NULL;

        return NOERROR;

         

    }

    cCharacters = wcslen(pszW)+1;

    // Determine number of bytes to be allocated for ANSI string. An   

    // ANSI string can have at most 2 bytes per character (for Double   

    // Byte Character Strings.)   

    cbAnsi = cCharacters*2;

     

    // Use of the OLE allocator is not required because the resultant   

    // ANSI  string will never be passed to another COM component. You   

    // can use your own allocator.   

     

    *ppszA = (LPSTR) CoTaskMemAlloc(cbAnsi);

    if (NULL == *ppszA)

        return E_OUTOFMEMORY;

    // Convert to ANSI.

    if (0 == WideCharToMultiByte(CP_ACP, 0, pszW, cCharacters, *ppszA, cbAnsi, NULL, NULL))

    {

        dwError = GetLastError();

        CoTaskMemFree(*ppszA);

        *ppszA = NULL;

        return HRESULT_FROM_WIN32(dwError);

         

    }

    return NOERROR;

    
}

Log::Log(char* filename)
{
	
	m_stream.open(filename);


}

void Log::WriteUnicode(WCHAR* logline)
{
	char* tmp[100];
	UnicodeToAnsi(logline,&(tmp[0]));
	m_stream<<tmp[0]<<endl;

}
void Log::WriteLPCWSTR(const WCHAR* logline,char* lpcwst)
{
	char* tmp[100];
	UnicodeToAnsi(logline,&(tmp[0]));
		char c[100];
		sprintf(c,"%s",tmp[0]);
	m_stream<<lpcwst<<" : " <<c<<endl;
	m_stream<<endl;

}
void Log::WriteHWND(unsigned int* logline,char* HWN)
{
	char tmp[100];
	sprintf(tmp,"%u",logline);
	m_stream<<HWN<<" : " <<tmp[0]<<endl;
	m_stream<<endl;

}
void Log::WriteDWORD(unsigned long* logline,char* DWO)
{
	char tmp[100];
	sprintf(tmp,"%ul",logline);
	m_stream<<DWO<<" : " <<tmp[0]<<endl;
	m_stream<<endl;

}
void Log::WriteUnicodeArgument(LPVOID * logline,char* datatype)
{
	if( strcmp(datatype,"LPCWSTR")==0)
			{
		           WriteLPCWSTR((WCHAR*)logline,datatype);
			}
	else if( strcmp(datatype,"HWND")==0)
			{
		           WriteHWND((unsigned int*)logline,datatype);
			}
	else if( strcmp(datatype,"LPCTSTR")==0)
			{
		           WriteAnsi((CHAR*)logline);
			}
	else if( strcmp(datatype,"DWORD")==0)
			{
		           WriteDWORD((DWORD*)logline,datatype);
			}
	

}
void Log::WriteAnsi(CHAR* logline)
{

	m_stream<<logline<<endl;

}

Log::~Log()
{
	m_stream.close();
}
