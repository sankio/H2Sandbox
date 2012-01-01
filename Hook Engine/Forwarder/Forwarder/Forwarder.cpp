// Forwarder.cpp : Defines the exported functions for the DLL application.
//
#include "stdafx.h"
#include "forwarder.h"

FORWARDER_API HANDLE WINAPI MyLSPCreateFileA(
  __in      LPCSTR lpFileName,
  __in      DWORD dwDesiredAccess,
  __in      DWORD dwShareMode,
  __in_opt  LPSECURITY_ATTRIBUTES lpSecurityAttributes,
  __in      DWORD dwCreationDisposition,
  __in      DWORD dwFlagsAndAttributes,
  __in_opt  HANDLE hTemplateFile
) 
{
return CreateFileA(lpFileName,dwDesiredAccess,dwShareMode,lpSecurityAttributes,dwCreationDisposition,dwFlagsAndAttributes,hTemplateFile); 
}


