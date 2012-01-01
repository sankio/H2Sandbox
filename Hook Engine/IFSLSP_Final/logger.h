#include<fstream>
#include <tchar.h>
#include <windows.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

using namespace std;


class Log {
public:
	Log(char* filename);
	~Log();
	void WriteUnicode(WCHAR*  logline);
	void Log::WriteLPCWSTR(const WCHAR* logline,char* lpcwst);
	void Log::WriteHWND(unsigned int* logline,char* HWN);
	void Log::WriteDWORD(unsigned long* logline,char* DWO);
void WriteUnicodeArgument(LPVOID* logline,char* datatype);

	void WriteAnsi(CHAR*  logline);
	HRESULT __fastcall UnicodeToAnsi(LPCOLESTR pszW, LPSTR* ppszA);
private:
	ofstream m_stream;
};