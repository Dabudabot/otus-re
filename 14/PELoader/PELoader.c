#include "PELoader.h"

VOID CloseHandles(
  _In_ HANDLE hFile,
  _In_ HANDLE hFileMapping,
  _In_ PVOID pImageBase
)
/*
  Close up all handles that were opened
  hFile         - handle to file to close
  hFileMapping  - handle to file mapping to close
  pImageBase    - mapped memory to unmap
*/
{
  BOOL bFlag;

  try
  {
    // unmap
    IF_FALSE_LEAVE(UnmapViewOfFile(pImageBase));

    // if mapping created then close
    if (NULL != hFileMapping &&
      INVALID_HANDLE_VALUE != hFileMapping)
    {
      IF_FALSE_LEAVE(CloseHandle(hFileMapping));
    }

    // if file created then close
    if (NULL != hFile &&
      INVALID_HANDLE_VALUE != hFile)
    {
      IF_FALSE_LEAVE(CloseHandle(hFile));
    }
  }
  finally
  {
    if (!bFlag)
    {
      Log(
        L_CRITICAL,
        _T("Failed to close handles\n")
      );
    }
  }
}

VOID FreeMemory(
  _In_ LPVOID peImage,
  _In_opt_ PIMPORT_FUNC_LIST pFuncList
)
/*
  Free all memory that was allocated
  peImage         - image in memory
  pFuncList       - head to list of imported functions
*/
{
  BOOL bFlag;

  try
  {
    // free image
    IF_FALSE_LEAVE(VirtualFree(peImage, 0, MEM_RELEASE));
  }
  finally
  {
    if (!bFlag)
    {
      Log(
        L_CRITICAL,
        _T("Failed to virtual free memory\n")
      );
    }
  }

  // get list head
  PIMPORT_FUNC_LIST pFuncListCurrent = pFuncList;

  // iterate list
  while (NULL != pFuncListCurrent)
  {
    // this will be deleted
    PIMPORT_FUNC_LIST toDelete = pFuncListCurrent;

    // move to next
    pFuncListCurrent = pFuncListCurrent->Next;
    free(toDelete);
  }
}

HRESULT GetImage(
  _In_ PTCHAR filename,
  _Out_ HANDLE* hFile,
  _Out_ HANDLE* hFileMapping,
  _Out_ DWORD* dwFileSize,
  _Out_ PVOID* pImageBase
)
/*
  Read pe from disk
  filename      - name of the file
  hFile         - handle to file created by callie
  hFileMapping  - handle to mapping created by callie
  dwFileSize    - size of image on disk
  pImageBase    - start address of pe
*/
{
  HRESULT result = S_OK;
  *hFile = INVALID_HANDLE_VALUE;
  *hFileMapping = INVALID_HANDLE_VALUE;
  *dwFileSize = 0;
  *pImageBase = NULL;

  try
  {
    //----------------------------------
    //            OPEN FILE
    //----------------------------------

    IF_NULL_LEAVE(
      *hFile,
      CreateFile(
        filename,
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        0,
        NULL
      )
    );

    *dwFileSize = GetFileSize(*hFile, NULL);

    //----------------------------------
    //      CREATE FILE MAPPING
    //----------------------------------

    IF_NULL_LEAVE(
      *hFileMapping,
      CreateFileMapping(
        *hFile,
        NULL,
        SEC_IMAGE | PAGE_READONLY,
        0,
        0,
        NULL
      )
    );

    IF_NULL_LEAVE(
      *pImageBase,
      MapViewOfFile(
        *hFileMapping,
        FILE_MAP_READ,
        0, 0, 0
      )
    );
  }
  finally
  {
    if (HERROR(result))
    {
      Log(
        L_CRITICAL,
        _T("GetImage fails with %d \n"),
        GetLastError()
      );

      if (NULL != *hFile && 
        INVALID_HANDLE_VALUE != *hFile)
      {
        CloseHandle(*hFile);
        *hFile = NULL;
      }

      if (NULL != *hFileMapping &&
        INVALID_HANDLE_VALUE != *hFileMapping)
      {
        CloseHandle(*hFileMapping);
        *hFileMapping = NULL;
      }
    }
    else
    {
      Log(L_INFO, _T("File was opened & mapped\n"));
    }
  }

  return result;
}

HRESULT CheckHeaders(
  _In_ PVOID pImageBase,
  _In_ DWORD dwFileSize
)
/*
  Check dos and nt header
  pImageBase    - start address of the image
  dwFileSize    - size of immage on disk
*/
{
  // image starts with dos header
  PIMAGE_DOS_HEADER pImageDosHeader = (PIMAGE_DOS_HEADER)pImageBase;

  //----------------------------------
  //      CHECK SIGNATURE MZ
  //----------------------------------

  // Dos header must start with MZ
  if (IMAGE_DOS_SIGNATURE != pImageDosHeader->e_magic)
  {
    Log(
      L_CRITICAL,
      _T("Wrong image dos signature %c%c \n"),
      ((PTCHAR)pImageBase)[0],
      ((PTCHAR)pImageBase)[1]
    );
    return E_UNEXPECTED;
  }

  // size of image without headers
  CONST ULONGLONG REMAIN = dwFileSize - sizeof(IMAGE_DOS_HEADER) - sizeof(IMAGE_OPTIONAL_HEADER);

  // if pointer to nt header miss avaliable size then something wrong
  if (pImageDosHeader->e_lfanew >= REMAIN)
  {
    Log(
      L_CRITICAL,
      _T("Wrong e_lfanew 0x%x\n"),
      pImageDosHeader->e_lfanew
    );
    return E_UNEXPECTED;
  }

  Log(L_INFO, _T("IMAGE_DOS_HEADER - OK\n"));

  //----------------------------------
  //      GET IMAGE NT HEADER
  //----------------------------------

  PIMAGE_NT_HEADERS pImageNtHeader = RVA_TO_VA(
    PIMAGE_NT_HEADERS,
    pImageDosHeader,
    pImageDosHeader->e_lfanew
  );

  // if signature of nt header not match then fail
  if (IMAGE_NT_SIGNATURE != pImageNtHeader->Signature)
  {
    Log(
      L_CRITICAL,
      _T("Wrong image nt signature 0x%x \n"),
      pImageNtHeader->Signature
    );
    return E_UNEXPECTED;
  }

  Log(L_INFO, _T("IMAGE_NT_HEADER - OK\n"));

  return S_OK;
}

HRESULT AllocPe(
  _In_ PVOID pImageBase,
  _Out_ LPVOID* peImage
)
/*
  Allocate executable memory for image and move memory
  pImageBase    - image on disk
  peImage       - image in memory
*/
{
  // get headers
  PIMAGE_DOS_HEADER pImageDosHeader = (PIMAGE_DOS_HEADER)pImageBase;
  PIMAGE_NT_HEADERS pImageNtHeader = RVA_TO_VA(PIMAGE_NT_HEADERS, pImageBase, pImageDosHeader->e_lfanew);

  //----------------------------------
  //          ALLOCATE
  //----------------------------------

  *peImage = VirtualAlloc(
    NULL,
    pImageNtHeader->OptionalHeader.SizeOfImage,
    MEM_COMMIT,
    PAGE_EXECUTE_READWRITE
  );

  if (NULL == *peImage)
  {
    Log(
      L_CRITICAL,
      _T("VirtualAlloc fails with %d \n"),
      GetLastError()
    );
    return E_OUTOFMEMORY;
  }

  Log(L_INFO, _T("Image allocated at 0x%x\n"), *peImage);

  MoveMemory(
    *peImage,
    pImageBase,
    pImageNtHeader->OptionalHeader.SizeOfHeaders
  );

  return S_OK;
}

HRESULT LoadSections(
  _In_ PVOID pImageBase,
  _In_ LPVOID peImage
)
/*
  Move sections to memory
  pImageBase    - image on disk
  peImage       - image in memory
*/
{
  // get headers
  PIMAGE_DOS_HEADER pImageDosHeader = (PIMAGE_DOS_HEADER)peImage;
  PIMAGE_NT_HEADERS pImageNtHeader = RVA_TO_VA(PIMAGE_NT_HEADERS, peImage, pImageDosHeader->e_lfanew);

  //----------------------------------
  //          LOAD SECTIONS
  //----------------------------------

  // get first section
  PIMAGE_SECTION_HEADER pSection = IMAGE_FIRST_SECTION(pImageNtHeader);
 
  if (!pSection)
  {
    Log(
      L_CRITICAL,
      _T("First section empty\n")
    );
    return E_UNEXPECTED;
  }

  Log(L_INFO, _T("Moving sections...\n"));

  DWORD dwNumSection = pImageNtHeader->FileHeader.NumberOfSections;
  DWORD i = 0;

  // iterate over sections
  while (i < dwNumSection)
  {
    LPVOID pDest = RVA_TO_VA(LPVOID, peImage, pSection->VirtualAddress);
    LPCVOID pSrc = RVA_TO_VA(LPCVOID, pImageBase, pSection->VirtualAddress);
    DWORD SectionSize = pSection->SizeOfRawData;

    // TODO data may be initilized\uninitilized

    MoveMemory(pDest, pSrc, SectionSize);

    Log(L_INFO, _T("Section #%02d is %s at 0x%x\n"), i, pSection->Name, pDest);

    pSection++;
    i++;
  }

  Log(L_INFO, _T("Sections ready\n"));

  return S_OK;
}

HRESULT AllocFunc(
  _Inout_ PIMPORT_FUNC_LIST* pImportFuncListHead,
  _In_ PTCHAR DllName,
  _In_ PTCHAR FuncName,
  _In_ DWORD FuncAddr
)
/*
  Allocate memory for imported functions
  pImportFuncListHead - head of function list
  DllName             - name of the dll
  FuncName            - name of the function
  FuncAddr            - address of the function
*/
{
  PIMPORT_FUNC_LIST pImportFuncList = malloc(sizeof(IMPORT_FUNC_LIST));

  if (NULL == pImportFuncList)
  {
    Log(
      L_CRITICAL,
      _T("Import list failed to alloc\n")
    );
    return E_OUTOFMEMORY;
  }

  pImportFuncList->DllName = DllName;
  pImportFuncList->FunctionName = FuncName;
  pImportFuncList->FunctionAddr = FuncAddr;
  pImportFuncList->Next = NULL;

  // if head is empty assign allocate to head
  if (NULL == *pImportFuncListHead)
  {
    *pImportFuncListHead = pImportFuncList;
  }
  // if not iterate to last
  else
  {
    PIMPORT_FUNC_LIST pImportFuncListCurrent = *pImportFuncListHead;

    while (NULL != pImportFuncListCurrent->Next)
    {
      pImportFuncListCurrent = pImportFuncListCurrent->Next;
    }

    pImportFuncListCurrent->Next = pImportFuncList;
  }

  return S_OK;
}

HRESULT FixImportTable(
  _In_ LPVOID peImage,
  _Inout_ PIMPORT_FUNC_LIST* pImportFuncListHead
)
/*
  Fix imports
  peImage             - image in memory
  pImportFuncListHead - functions list head
*/
{
  HRESULT result = E_UNEXPECTED;
  // get headers of image in memory
  PIMAGE_DOS_HEADER pImageDosHeader = (PIMAGE_DOS_HEADER) peImage;
  PIMAGE_NT_HEADERS pImageNtHeader = RVA_TO_VA(PIMAGE_NT_HEADERS, peImage, pImageDosHeader->e_lfanew);
  // VA of import
  DWORD imageImportDescrVA = pImageNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
  PTCHAR curDllName;
  HMODULE curDllModule;
  DWORD dllInfoThunkVA;
  
  // first discriptor
  PIMAGE_IMPORT_DESCRIPTOR imageImportDescr = RVA_TO_VA(PIMAGE_IMPORT_DESCRIPTOR,
    peImage, imageImportDescrVA);

  PIMAGE_IMPORT_DESCRIPTOR currentDescr = imageImportDescr;

  // iterate over import discriptors
  while (TRUE)
  {
    // if null then descriptors over
    if (!*(DWORD*)currentDescr || !currentDescr->FirstThunk ||
      (!currentDescr->Characteristics && !currentDescr->Name))
    {
      break;
    }

    // get dll name
    curDllName = RVA_TO_VA(
      PTCHAR, 
      peImage,
      currentDescr->Name
    );

    Log(L_INFO, _T("Found dll %s\n"), curDllName);

    try 
    {
      IF_NULL_LEAVE(
        curDllModule,
        LoadLibrary(curDllName)
      );

      dllInfoThunkVA = currentDescr->FirstThunk;

      PIMAGE_THUNK_DATA pThunk = RVA_TO_VA(
        PIMAGE_THUNK_DATA,
        peImage,
        dllInfoThunkVA
      );

      // iterate over thunks
      while (*(DWORD*)pThunk)
      {
        if ((pThunk->u1.AddressOfData & 0xFFFF) == 0x8000)
        {
          continue;
        }
        
        DWORD functionsVA = pThunk->u1.AddressOfData;
        
        PTCHAR functionName = RVA_TO_VA(PTCHAR, RVA_TO_VA(DWORD, peImage, functionsVA), 2);
        DWORD functionAddr = (DWORD)GetProcAddress(curDllModule, functionName);

        Log(L_INFO, _T("  Found function %s at 0x%x\n"), functionName, functionAddr);

        IF_FAIL_LEAVE(
          AllocFunc(
            pImportFuncListHead,
            curDllName,
            functionName,
            functionAddr
          )
        );

        // fix
        pThunk->u1.AddressOfData = functionAddr;

        pThunk++;
      }
    }
    finally
    {

      if (HERROR(result))
      {
        Log(
          L_CRITICAL,
          _T("Some error with imports\n")
        );
      }

      currentDescr++;
    }
  }

  Log(L_INFO, _T("Imports done\n"));

  return result;
}

HRESULT FixRelocationTable(
  _In_ LPVOID peImage
)
/*
  Fix realocations
  peImage             - image in memory
*/
{
  // get headers of image in memory
  PIMAGE_DOS_HEADER pImageDosHeader = (PIMAGE_DOS_HEADER) peImage;
  PIMAGE_NT_HEADERS pImageNtHeader = RVA_TO_VA(PIMAGE_NT_HEADERS, peImage, pImageDosHeader->e_lfanew);

  if (pImageNtHeader->FileHeader.Characteristics & IMAGE_FILE_RELOCS_STRIPPED)
  {
    Log(L_INFO, _T("IMAGE_FILE_RELOCS_STRIPPED\n"));
    return S_OK;
  }

  DWORD imageRelocationVA = pImageNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress;
  PIMAGE_BASE_RELOCATION pImageBaseRelocation = RVA_TO_VA(PIMAGE_BASE_RELOCATION, peImage, imageRelocationVA);

  // iterate over realocation table
  while (pImageBaseRelocation->VirtualAddress && pImageBaseRelocation->SizeOfBlock)
  {
    DWORD relocServionAddress = pImageBaseRelocation->VirtualAddress;
    DWORD relocCount = (pImageBaseRelocation->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / sizeof(WORD);

    PRELOCATION fixUps = RVA_TO_VA(PRELOCATION, pImageBaseRelocation, sizeof(IMAGE_BASE_RELOCATION));

    // iterate over fixups
    for (DWORD i = 0; i < relocCount; i++)
    {
      DWORD* address = NULL;
      if (fixUps[i].Type == IMAGE_REL_BASED_HIGHLOW)
      {
        address = RVA_TO_VA(PDWORD, peImage, relocServionAddress + fixUps[i].Offset);
        DWORD oldAddress = *address;
        DWORD newAddress = oldAddress - pImageNtHeader->OptionalHeader.ImageBase + (DWORD)peImage;
        *address = newAddress;  // fix

        Log(L_INFO, _T("    Relocating 0x%x -> 0x%x\n"), oldAddress, newAddress);
      }
    }
    // move to next
    pImageBaseRelocation = RVA_TO_VA(PIMAGE_BASE_RELOCATION, pImageBaseRelocation, pImageBaseRelocation->SizeOfBlock);
  }

  Log(L_INFO, _T("Relocation done\n"));

  return S_OK;
}

HRESULT RunPe(
  _In_ LPVOID peImage
)
/*
  Fix image base and transfer control
  peImage - image in memory
*/
{
  // get headers of image im memory
  PIMAGE_DOS_HEADER DOSHeader = (PIMAGE_DOS_HEADER)peImage;
  PIMAGE_NT_HEADERS NtHeader = RVA_TO_VA(PIMAGE_NT_HEADERS, peImage, DOSHeader->e_lfanew);
  // fix image base
  NtHeader->OptionalHeader.ImageBase = (DWORD)peImage;
  DWORD peEntryPoint = RVA_TO_VA(DWORD, peImage, NtHeader->OptionalHeader.AddressOfEntryPoint);

  Log(L_INFO, _T("Jumping to entry point at 0x%x\n"), peEntryPoint);

  _asm
  {
    mov eax, [peEntryPoint]
    jmp eax // to the entry point
  }

  // will not be executed because ExitProcess is called in
  Log(L_INFO, _T("Program finished\n"));

  return S_OK;
}

int _tmain(int argc, PTCHAR argv[])
{
  if (argc != 3)
  {
    _tprintf(_T("Usage: PELoader <logfile> {<image.exe>} \n"));
    return 1;
  }

  SIMPLE_PE pe;
  pe.pImportFuncsList = NULL;
  HRESULT result;

  try
  {
    IF_FAIL_LEAVE(
      InitLogger(argv[1])
    );

    IF_FAIL_LEAVE(
      GetImage(
        argv[2],
        &pe.hFile,
        &pe.hFileMapping,
        &pe.dwFileSize,
        &pe.pImageBase
      )
    );

    IF_FAIL_LEAVE(
      CheckHeaders(
        pe.pImageBase,
        pe.dwFileSize
      )
    );

    IF_FAIL_LEAVE(
      AllocPe(
        pe.pImageBase,
        &pe.peImage
      )
    );

    IF_FAIL_LEAVE(
      LoadSections(
        pe.pImageBase,
        pe.peImage
      )
    );

    IF_FAIL_LEAVE(
      FixImportTable(
        pe.peImage,
        &pe.pImportFuncsList
      )
    );

    IF_FAIL_LEAVE(
      FixRelocationTable(
        pe.peImage
      )
    );

    // TODO Process TLS

    IF_FAIL_LEAVE(
      RunPe(
        pe.peImage
      )
    );
  }
  finally
  {
    // executed in case of error only because of exit process
    // do not be afraid of leaks because OS will cleanup us
    FreeMemory(pe.peImage, pe.pImportFuncsList);

    CloseHandles(pe.hFile, pe.hFileMapping, pe.pImageBase);
  }
  
  return 0;
}