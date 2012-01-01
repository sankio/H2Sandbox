#include <fstream>
#include <tchar.h>
#include <WTypes.h>

#include <time.h>
#include <stdlib.h>
#include <string.h>


using namespace std;

class Log {
public:
	Log(char* filename);
	~Log();
	void WriteUnicode(WCHAR*  logline);
	HRESULT __fastcall Log::AnsiToUnicode(LPCSTR pszW);
	void Log::WriteLPCWSTR(const WCHAR* logline,char* lpcwst);
	void Log::WriteLPCWSTR2(const WCHAR* logline,char* lpcwst,char* cname);
	void Log::WriteHWND(unsigned int* logline,char* HWN);
	void Log::WriteDWORD(unsigned long* logline,char* DWO);
	void WriteUnicodeArgument(LPVOID* logline,char* datatype);
	void WriteUnicodeArgument2(LPVOID * logline,char* datatype,char* cname);
	void Log::WriteLPCTSTR2(const CHAR* logline,char* lpcwst,char* cname);
	void Log::WriteHKEY2(HKEY logline,char* HKE,char* cname);
	void WriteDWORD2(DWORD logline,char* DWO,char* cname);
	void WriteAnsi(CHAR*  logline);
	HRESULT __fastcall UnicodeToAnsi(LPCOLESTR pszW, LPSTR* ppszA);
private:
	ofstream m_stream;
};