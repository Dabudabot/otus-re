#pragma once

#include "PELoader.h"
#include <time.h>

typedef enum _LOG_TYPE
{

  L_INFO,
  L_ERROR,
  L_CRITICAL

} LOG_TYPE, *PLOG_TYPE;

FILE* gLog;

HRESULT InitLogger(PTCHAR Filename);
VOID Log(LOG_TYPE type, PTCHAR format, ...);