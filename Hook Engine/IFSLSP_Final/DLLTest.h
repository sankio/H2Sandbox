// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the DLLTEST_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// DLLTEST_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef DLLTEST_EXPORTS
#define DLLTEST_API __declspec(dllexport)
#else
#define DLLTEST_API __declspec(dllimport)
#endif

// This class is exported from the DLLTest.dll
class DLLTEST_API CDLLTest {
public:
	CDLLTest(void);
	// TODO: add your methods here.
};

extern DLLTEST_API int nDLLTest;

extern "C" DLLTEST_API int AddValue(void);
