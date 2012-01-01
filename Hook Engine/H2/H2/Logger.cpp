#include "logger.h"
/* * UnicodeToAnsi converts the Unicode string pszW to an ANSI string

   * and returns the ANSI string through ppszA. Space for the

   * the converted string is allocated by UnicodeToAnsi.

   */
HRESULT __fastcall Log::AnsiToUnicode(LPCSTR pszW ){


 ULONG cbUnicode, cCharacters;

    DWORD dwError;
	WCHAR* tmp[10000];
	LPWSTR *ppszA=&(tmp[0]);
    cCharacters = strlen(pszW)+1;

    // Determine number of bytes to be allocated for ANSI string. An   

    // ANSI string can have at most 2 bytes per character (for Double   

    // Byte Character Strings.)   

    cbUnicode = cCharacters*4;

     

    // Use of the OLE allocator is not required because the resultant   

    // ANSI  string will never be passed to another COM component. You   

    // can use your own allocator.   

     

     *ppszA = (LPWSTR) CoTaskMemAlloc(cbUnicode);

    if (NULL == *ppszA)

		OutputDebugStringA("E_OUTOFMEMORY");

    // Convert to ANSI.

    if (0 == MultiByteToWideChar(CP_ACP, 0, pszW, cCharacters, *ppszA, cbUnicode))

    {

        dwError = GetLastError();

        CoTaskMemFree(*ppszA);

        *ppszA = NULL;
		
        return HRESULT_FROM_WIN32(dwError);


	}
	WCHAR c[10000];
	wsprintf(c,L"%s",tmp[0]);

	OutputDebugStringW(L"c");
	return NOERROR;
}
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
	char* tmp[10000];
	UnicodeToAnsi(logline,&(tmp[0]));
		char c[10000];
		sprintf(c,"%s",tmp[0]);
	/*m_stream<<lpcwst<<" : " <<c<<endl;*/
	m_stream<<c<<endl;
	//m_stream<<endl;

}

void Log::WriteLPCWSTR2(const WCHAR* logline,char* lpcwst,char* cname)
{
	char* tmp[10000];
	UnicodeToAnsi(logline,&(tmp[0]));
		char c[10000];
		sprintf(c,"%s",tmp[0]);
	m_stream<<"<parameter name=\""<<cname<<"\" text='"<<c<<"'></parameter>"<<endl;

}
void Log::WriteLPCTSTR2(const char* logline,char* lpcwst,char* cname)
{

	m_stream<<"<parameter name=\""<<cname<<"\" text='"<<logline<<"'></parameter>"<<endl;

}
void Log::WriteHWND(unsigned int* logline,char* HWN)
{
	char tmp[100];
	sprintf(tmp,"%u",logline);
	//m_stream<<HWN<<" : " <<tmp[0]<<endl;
	m_stream<<tmp[0]<<endl;
	m_stream<<endl;

}
void Log::WriteDWORD(unsigned long* logline,char* DWO)
{
	char tmp[100];
	sprintf(tmp,"%ul",logline);
	/*m_stream<<DWO<<" : " <<tmp[0]<<endl;*/
	m_stream<<tmp[0]<<endl;
	//m_stream<<endl;

}
void Log::WriteHKEY2(HKEY logline,char* HKE,char* cname)
{
	char tmp[100];
	sprintf(tmp,"%p",logline);
	if(logline==HKEY_LOCAL_MACHINE)
		m_stream<<"<parameter name=\""<<HKE<<"\" text='HKEY_LOCAL_MACHINE'></parameter>"<<endl;
	else if(logline==HKEY_CURRENT_USER )
		m_stream<<"<parameter name=\""<<HKE<<"\" text='HKEY_CURRENT_USER '></parameter>"<<endl;
	else if(logline==HKEY_CLASSES_ROOT)
		m_stream<<"<parameter name=\""<<HKE<<"\" text='HKEY_CLASSES_ROOT '></parameter>"<<endl;	
	else m_stream<<"<parameter name=\""<<HKE<<"\" text='"<<tmp<<"'></parameter>"<<endl;	
	
}
void Log::WriteDWORD2(DWORD logline,char* DWO,char* cname)
{
	char tmp[100];
	sprintf(tmp,"%u",logline);
	/*m_stream<<DWO<<" : " <<tmp[0]<<endl;*/
	if(strcmp(cname,"Process ID")==0)
	{
		m_stream<<"<parameter name=\""<<cname<<"\" text=\""<<tmp<<"\"></parameter>"<<endl;	
	}
	else if(strcmp(cname,"Desired Access")==0)
	{

		m_stream<<"<parameter name=\""<<cname<<"\" text=\"";//<<(int)(logline&GENERIC_WRITE);
			 
		if (logline&GENERIC_ALL)
		{
			m_stream<<"GENERIC_ALL";
		}
		else
		{
			int flag=0;
			if (logline&GENERIC_WRITE)
			{
				flag=1;
				m_stream<<"GENERIC_WRITE";
			}
			if (logline&GENERIC_READ)
			{
				if(flag)
					m_stream<<"|";
				m_stream<<"GENERIC_READ";
				flag=1;
			}
			if (logline&GENERIC_EXECUTE)
			{
				if(flag)
					m_stream<<"|";
				m_stream<<"GENERIC_EXCUTE";
			}
		}

		m_stream<<"\"></parameter>"<<endl;
	}
	else if(strcmp(cname,"Share Mode")==0)
	{
			m_stream<<"<parameter name=\""<<cname<<"\" text=\"";//<<(int)(logline&GENERIC_WRITE);
		if (logline==0x00000000)
			m_stream<<"PREVENT_ACCESS";
		else { 	
			int flag=0;
			if (logline&FILE_SHARE_DELETE)
			{
				flag=1;
				m_stream<<"FILE_SHARE_DELETE";
			}
			if (logline&FILE_SHARE_READ)
			{
				if(flag)
					m_stream<<"|";
				m_stream<<"FILE_SHARE_READ";
				flag=1;
			}
			if (logline&FILE_SHARE_WRITE)
			{
				if(flag)
					m_stream<<"|";
				m_stream<<"FILE_SHARE_WRITE";
			}
			
		}
			m_stream<<"\"></parameter>"<<endl;
	}
	else if(strcmp(cname,"Creation/Disposition")==0)
	{
		if((logline&OPEN_EXISTING)==OPEN_EXISTING) 
		 m_stream<<"<parameter name=\""<<cname<<"\" text=\"OPEN_EXISTING\"></parameter>"<<endl;
	   else if((logline&CREATE_NEW)==CREATE_NEW)
		  m_stream<<"<parameter name=\""<<cname<<"\" text=\"CREATE_NEW\"></parameter>"<<endl;
	    else if((logline&CREATE_ALWAYS)==CREATE_ALWAYS)
		  m_stream<<"<parameter name=\""<<cname<<"\" text=\"CREATE_ALWAYS\"></parameter>"<<endl;
	   else if((logline&OPEN_ALWAYS)==OPEN_ALWAYS)
		  m_stream<<"<parameter name=\""<<cname<<"\" text=\"OPEN_ALWAYS\"></parameter>"<<endl;
	   else if((logline&TRUNCATE_EXISTING)==TRUNCATE_EXISTING)
		  m_stream<<"<parameter name=\""<<cname<<"\" text=\"TRUNCATE_EXISTING\"></parameter>"<<endl;	  
	   else m_stream<<"<parameter name=\""<<cname<<"\" text=\""<<tmp[0]<<"\"></parameter>"<<endl;
		 
	}
	else if(strcmp(cname,"Flags/Attributes")==0)
	{
			m_stream<<"<parameter name=\""<<cname<<"\" text=\"";//<<(int)(logline&GENERIC_WRITE);
			int flag=0;
			if (logline&FILE_ATTRIBUTE_NORMAL)
			{
				flag=1;
				m_stream<<"FILE_ATTRIBUTE_NORMAL";
			}
			if (logline&FILE_ATTRIBUTE_ARCHIVE)
			{
				if(flag)
					m_stream<<"|";
				m_stream<<"FILE_ATTRIBUTE_ARCHIVE";
				flag=1;
			}
			if (logline&FILE_ATTRIBUTE_DIRECTORY)
			{
				if(flag)
					m_stream<<"|";
				m_stream<<"FILE_ATTRIBUTE_DIRECTORY";
			}
			if (logline&FILE_ATTRIBUTE_HIDDEN)
			{
				if(flag)
					m_stream<<"|";
				m_stream<<"FILE_ATTRIBUTE_HIDDEN";
			}
				if (logline&FILE_ATTRIBUTE_SYSTEM)
			{
				if(flag)
					m_stream<<"|";
				m_stream<<"FILE_ATTRIBUTE_SYSTEM";
			}
				if (logline&FILE_FLAG_OVERLAPPED)
			{
				if(flag)
					m_stream<<"|";
				m_stream<<"FILE_FLAG_OVERLAPPED";
			}	
				m_stream<<"\"></parameter>"<<endl;	
	}
	
	else m_stream<<"<parameter name=\""<<cname<<"\" text=\""<<tmp[0]<<"\"></parameter>"<<endl;
		
	//m_stream<<endl;

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
		          // WriteDWORD((PDWORD*)logline,datatype);
			}
	

}
// Update Log
void Log::WriteUnicodeArgument2(LPVOID * logline,char* datatype,char* cname)
{
	if( strcmp(datatype,"LPCWSTR")==0)
			{
		           WriteLPCWSTR2((WCHAR*)logline,datatype,cname);
			}
	else if (strcmp(datatype,"HKEY")==0)
			{
				   WriteHKEY2((HKEY)logline,datatype,cname);
			}
	else if( strcmp(datatype,"LPCSTR")==0)
			{
		           WriteLPCTSTR2((const char *)logline,datatype,cname);
			}
	else if( strcmp(datatype,"HWND")==0)
			{
		           WriteHWND((unsigned int*)logline,datatype);
			}
	else if( strcmp(datatype,"LPSTR")==0)
			{
		           WriteAnsi((CHAR*)logline);
			}
	else if( strcmp(datatype,"DWORD")==0)
			{
		           WriteDWORD2((DWORD)logline,datatype,cname);
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
