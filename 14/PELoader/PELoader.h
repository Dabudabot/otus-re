#pragma once

#include <Windows.h>
#include <tchar.h>
#include <assert.h>
#include <stdio.h>

#include "Logger.h"
#include "Macro.h"

//
//  Imported functions just for log
//
typedef struct _IMPORT_FUNC_LIST
{
  PVOID   Next;         // pointer to next element of PIMPORT_FUNC_LIST
  PTCHAR  DllName;      // name of dll 
  PTCHAR  FunctionName; // name of the function
  DWORD   FunctionAddr; // address of the function
} IMPORT_FUNC_LIST, *PIMPORT_FUNC_LIST;

//
//  Relocation fix
//
typedef struct _RELOCATION
{
  WORD    Offset : 12;  // fix offset
  WORD    Type : 4;     // type of realocation
} RELOCATION, *PRELOCATION;

//
//  Global pe keeper
//
typedef struct _SIMPLE_PE
{
  DWORD  dwFileSize;    // File size on disk
  HANDLE hFile;         // handle to the file on disk
  HANDLE hFileMapping;  // mapped file on disk

  PVOID             pImageBase;         // image as is on disk
  PIMPORT_FUNC_LIST pImportFuncsList;   // imported functions

  LPVOID peImage;   // image in memory

} SIMPLE_PE, *PSIMPLE_PE;