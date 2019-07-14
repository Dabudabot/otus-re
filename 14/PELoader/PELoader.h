#pragma once

#include <Windows.h>
#include <tchar.h>
#include <assert.h>
#include <stdio.h>

#include "Logger.h"
#include "Macro.h"

typedef struct _IMPORT_FUNC_LIST
{
  PVOID Next;
  PTCHAR  DllName;
  PTCHAR  FunctionName;
  DWORD   FunctionAddr;
} IMPORT_FUNC_LIST, *PIMPORT_FUNC_LIST;

typedef struct _RELOCATION
{
  WORD    Offset : 12;
  WORD    Type : 4;
} RELOCATION, *PRELOCATION;

typedef struct _SIMPLE_PE
{
  DWORD  dwFileSize;
  HANDLE hFile;
  HANDLE hFileMapping;

  PVOID pImageBase;                     // image as is on disk
  PIMPORT_FUNC_LIST pImportFuncsList;

  LPVOID peImage;                     // image in memory

} SIMPLE_PE, *PSIMPLE_PE;