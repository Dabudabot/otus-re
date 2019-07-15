#pragma once

#include "PELoader.h"
#include <time.h>

//
//  Type of message in log
//
typedef enum _LOG_TYPE
{

  L_INFO,
  L_ERROR,
  L_CRITICAL

} LOG_TYPE, *PLOG_TYPE;

// File to log to
FILE* gLog;

/*
  Intialization of logging.
    Opening log file and streaming first message
    Filename - name of log file
*/
HRESULT InitLogger(PTCHAR Filename);

/*
  Logging message
    Print to console and file
    type    - message type
    format  - message format
    ...     - message parts
*/
VOID Log(LOG_TYPE type, PTCHAR format, ...);