#include "hookmanager.h"


	 HookManager::HookManager()
{
	 MyHookedAPIList =new MyHookedAPI();
}
BOOL HookManager::AddHookList(LPCSTR lhModule,LPCSTR baseLibraryName, LPCSTR lFuncName, PROC newFunctionAddress)
{
	PROC OrigFunctionAdress;
	if(MyHookedAPIList->g_Counter<24)
	{
		// Store Address of Original Function and its related information into API List
		HMODULE baseDLLName=LoadLibraryA(baseLibraryName);
		OrigFunctionAdress=(PROC)GetProcAddress(baseDLLName,lFuncName);
		MyHookedAPIList->SetHookedAPI(lhModule,lFuncName,baseLibraryName,OrigFunctionAdress,newFunctionAddress,FALSE);
		return TRUE;
	}
	else return FALSE;
}
void HookManager::GetHookList()
{
	
}
BOOL HookManager::DoHook(int hookingMethod)
{
	HMODULE hModule;
	PROC OrigAddrFunc;
	if(hookingMethod==Hook_Specified_Module){
		for(int i=0;i< MyHookedAPIList->g_Counter;i++)
		{
			if(strcmp(MyHookedAPIList->GetModuleName(i),"")==0)
			{
				OutputDebugStringA("Module Not Found!");
				return FALSE;
			}
			hModule=LoadLibraryA(MyHookedAPIList->GetModuleName(i));
			//Also Hook EAT function for yet-to-load DLL if functions , that are going
			//to be hooked , are exported.
			OrigAddrFunc=HookExportTable(MyHookedAPIList->GetDLLName(i),
										 MyHookedAPIList->GetOrigAddressOfFunc(i),
										 MyHookedAPIList->GetNewAddressOfFunc(i));
			OrigAddrFunc=HookInOneModule(hModule,
										 MyHookedAPIList->GetDLLName(i),
										 MyHookedAPIList->GetOrigAddressOfFunc(i),
										 MyHookedAPIList->GetNewAddressOfFunc(i));
		
			MyHookedAPIList->SetHookedStatus(i);
		}
	}
	//If all modules are to be hooked , HookInAllModule should be defined 
	else if(hookingMethod==Hook_All_Module) {
		HMODULE hMyDll1=LoadLibraryA("H2.dll");
		HMODULE hMyDll2=LoadLibraryA("Forwarder.dll");
		HANDLE snap= CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,GetCurrentProcessId());
		MODULEENTRY32 mod;
		mod.dwSize=sizeof(MODULEENTRY32);
		
		for(int i=0;i<MyHookedAPIList->g_Counter;i++)
		{
			//Also Hook EAT function for yet-to-load DLL if functions , that are going
			//to be hooked , are exported.
			OrigAddrFunc=HookExportTable(MyHookedAPIList->GetDLLName(i),
										 MyHookedAPIList->GetOrigAddressOfFunc(i),
										 MyHookedAPIList->GetNewAddressOfFunc(i));
			Module32First(snap,&mod);
			HMODULE first=mod.hModule;
			OrigAddrFunc=HookInOneModule(first,
										 MyHookedAPIList->GetDLLName(i),
										 MyHookedAPIList->GetOrigAddressOfFunc(i),
										 MyHookedAPIList->GetNewAddressOfFunc(i));
			while(Module32Next(snap,&mod))
			{
				HMODULE next=mod.hModule;
				if(next==hMyDll1||next==hMyDll2) continue; // Do not hook injected DLL
				OrigAddrFunc=HookInOneModule(next,
											 MyHookedAPIList->GetDLLName(i),
											 MyHookedAPIList->GetOrigAddressOfFunc(i),
											 MyHookedAPIList->GetNewAddressOfFunc(i));
			}
			MyHookedAPIList->SetHookedStatus(i);
		}
	}
	return TRUE;
}

PROC HookManager::HookInOneModule(HMODULE hModule, LPCSTR baseLibraryName, PROC oldFunctionPointer, PROC newFunctionPointer)
{
        char text[1000];
        ULONG size;
        PROC result = NULL; // If the function is found, returns the old address
		
		//Modules that are global hooked will not need to be hooked again
		if (hModule==NULL) return NULL;
        
		// Retrieving the position of the first Import Descriptor Table
        // This table holds the information about the imports from one
        // single dll. It holds the address to dll's name, to dll's IAT
        // and some other data.
PIMAGE_IMPORT_DESCRIPTOR pImportDesc;

        pImportDesc = (PIMAGE_IMPORT_DESCRIPTOR) ImageDirectoryEntryToData(
                hModule,
                TRUE,
                IMAGE_DIRECTORY_ENTRY_IMPORT,
                &size
        );

        // No Import Descriptor Tables found or some error occured
        if (pImportDesc == NULL) 
        {
                return NULL;
        }

        // For each Import Descriptor Table (for each DLL) check for the
        // existance of wanted DLL
        while (pImportDesc -> Name)
        {
                // We add the base address of the module whose IAT we are going to read and the 
                // RVA offset to the DLL (module's) name. We cast this to pointer to string.
                PSTR IDTModName = (PSTR)((PBYTE)hModule + pImportDesc -> Name);

                // We compare the library names. If the comparrison returns zero, we have found
                // out that this module imports something from the library which holds a function
                // that we want to control. That means we have to check all the functions and see
                // if our function is one of them.
                if (_stricmp(baseLibraryName, IDTModName) == 0)
                {
                        break;
                }

                 //If the compared names were not the same, go to the next Import Descriptor Table.
                pImportDesc++;
        }
        
        // If we went through all the descriptor tables and haven't found wanted dll, we are
        // done with this module.
        if (pImportDesc -> Name == NULL)        
        {
                return NULL;
        }

        // If we found a module we want to hook, we need to search through all of the functions
        // for the one we want.

        // First we add the RVA to the first IAT entry (first address of the imported function) to
        // the base of the module.
        PIMAGE_THUNK_DATA pThunk = (PIMAGE_THUNK_DATA)((PBYTE)hModule + pImportDesc -> FirstThunk);


        // For each function in the IAT we check if it's the wanted one
        while (pThunk -> u1.Function)
        {
                // Retrieve the pointer to pointer to function. pfn will point inside the IAT where
                // the pointer to function is stored.
                PROC *ppfn = (PROC *) &pThunk -> u1.Function;
                
                // If the pointer to current function equals our wanted function, we found a hit
                sprintf_s(text, "%p <--> %p", *ppfn, oldFunctionPointer);
                if (*ppfn == oldFunctionPointer)
                {
                        // First we store the page information of the IAT. Note the pfn holds
                        // the pointer to IAT entry, and not directy to function
                        MEMORY_BASIC_INFORMATION info;

                        if (VirtualQuery(ppfn, &info, sizeof(MEMORY_BASIC_INFORMATION)) == 0)
                        {
                                return NULL;
                        }

                        // This function attempts to change the protection scheme of a region of pages.
                        // It also copies the old protection scheme to the last parameter (in this case
                        // it is info.Protect).
                        if (VirtualProtect(info.BaseAddress, info.RegionSize, PAGE_EXECUTE_READWRITE, &info.Protect) == FALSE)
                        {
                                return NULL;
                        }

                        // We store the old pointer in the IAT memory and change it to point to our function.
                        result = *ppfn;	
                        *ppfn = *newFunctionPointer;
						sprintf_s(text, "%p <--> %p", result);
               
                        // Lastly we attempt to restore the previous protection. "dummyProtect is used
                        // to store the PAGE_READWRITE protection, although we don't need it. NULL can't
                        // be used because in that case the function fails.
                        DWORD dummyProtect;

                        if (VirtualProtect(info.BaseAddress, info.RegionSize, info.Protect, &dummyProtect) == FALSE)
                        {
                             /*    Actually this should not be vital, so could be turned off in case of trouble.
                                 But if the first VirtualProtect succeeded, this one should, too.
                       */         return NULL;
                        }


                        return result;
                }

                pThunk++;
        }

        return NULL;
}
PROC HookManager::HookExportTable(LPCSTR lpDllName, PROC oldFunctionPointer, PROC newFunctionPointer)
{
        ULONG size;
        PROC result = NULL; // If the function is found, returns the old address
		HMODULE hModule=LoadLibraryA(lpDllName);
        // Retrieving the position of the first Export Table
        // This table holds the information about the imports from one
        // single dll. It holds the address to dll's name, to dll's EAT
        // and some other data.
PIMAGE_EXPORT_DIRECTORY pExportDesc;

        pExportDesc = (PIMAGE_EXPORT_DIRECTORY) ImageDirectoryEntryToData(
                hModule,
                TRUE,
                IMAGE_DIRECTORY_ENTRY_EXPORT ,
                &size        );

        // No Import Descriptor Tables found or some error occured
        if (pExportDesc == NULL) 
        {
                return NULL;
        }
//		EAT Redirection - The EAT, or Export Address Table is similar to the 
//		IAT.  Except in the opposite direction.  When a module exports a function 
//		so that it can be used by other modules, it stores the address of  
//		that function in it's EAT.  EAT redirection overwrites that address 
//		with the offset of your hook.  EAT redirection will not effect any 
//		currently loaded modules.  It will only effect modules loaded after the 
//		redirection	has been made.  It will also effect subsequent calls to 
//		GetProcAddress(), as they will return the address of your hook instead of 
//		the real function. 
        
		ULONG *addressoffunctions=(ULONG*)((BYTE*) hModule +pExportDesc->AddressOfFunctions);
		ULONG * addressofnames=(ULONG*)((BYTE*) hModule +pExportDesc->AddressOfNames);
        // For each function in the EAT we check if it's the wanted one
		for(DWORD x=0; x < pExportDesc->NumberOfFunctions;x++)
			{
				char*functionname=(char*)((BYTE*) hModule +addressofnames[x]);
				DWORD functionaddress=(DWORD)((BYTE*)hModule +addressoffunctions[x]);
				PROC *pfunc=(PROC*)&functionaddress;
				  if (*pfunc==oldFunctionPointer)
				  {
						 PROC* ppfn = (PROC *) &addressoffunctions[x];
						 MEMORY_BASIC_INFORMATION info;

                        if (VirtualQuery(ppfn, &info, sizeof(MEMORY_BASIC_INFORMATION)) == 0)
                        {
                                return NULL;
                        }

                        // This function attempts to change the protection scheme of a region of pages.
                        // It also copies the old protection scheme to the last parameter (in this case
                        // it is info.Protect).
                        if (VirtualProtect(info.BaseAddress, info.RegionSize, PAGE_EXECUTE_READWRITE, &info.Protect) == FALSE)
                        {
                                return NULL;
                        }

                        // We store the old pointer in the IAT memory and change it to point to our function.
                        result = *ppfn;	
                        *ppfn = GetAddressOfInjectedFunction(PROC,newFunctionPointer,hModule);
					    // Lastly we attempt to restore the previous protection. "dummyProtect is used
                        // to store the PAGE_READWRITE protection, although we don't need it. NULL can't
                        // be used because in that case the function fails.
                        DWORD dummyProtect;

                        if (VirtualProtect(info.BaseAddress, info.RegionSize, info.Protect, &dummyProtect) == FALSE)
                        {
                             /*    Actually this should not be vital, so could be turned off in case of trouble.
                                 But if the first VirtualProtect succeeded, this one should, too.
                       */         return NULL;
                        }


                        return result;
                
				  }
			
		}
}

