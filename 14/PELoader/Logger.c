#include "Logger.h"

HRESULT InitLogger(PTCHAR Filename)
{
  gLog = NULL;
  errno_t err = _tfopen_s(&gLog, Filename, _T("at")); // append text

  if (err || !gLog)
  {
    _tprintf(_T("Log opening failed\n"));
    return E_UNEXPECTED;
  }

  time_t t = time(NULL);
  struct tm tm;
  err = localtime_s(&tm, &t);

  if (err)
  {
    _tprintf(_T("Log opening failed - time\n"));
    return E_UNEXPECTED;
  }

  _TCHAR s[64];
  assert(_tcsftime(s, sizeof(s), "%c", &tm));

  _ftprintf(gLog, _T("\n\n"));
  _ftprintf(gLog, _T("-------------------\n"));
  _ftprintf(gLog, _T("-------------------\n"));
  _ftprintf(gLog, _T("%s\n"), s);
  _ftprintf(gLog, _T("-------------------\n"));
  _ftprintf(gLog, _T("PE Loader initiated\n"));
  _ftprintf(gLog, _T("-------------------\n"));

  _tprintf(_T("Log opened\n"));

  return S_OK;
}

VOID Log(LOG_TYPE type, PTCHAR format, ...)
{
  time_t t = time(NULL);
  struct tm tm;
  errno_t err = localtime_s(&tm, &t);

  if (err)
  {
    _tprintf_s(_T("Log failed - time\n"));
    return;
  }
  _TCHAR s[64];
  assert(_tcsftime(s, sizeof(s), "%c", &tm));

  va_list argptr;
  va_start(argptr, format);

  switch (type)
  {
  case L_CRITICAL:
  case L_ERROR:
    _tprintf_s(_T("[ERROR "));
    _ftprintf_s(gLog, _T("[ERROR "));
    break;
  default:
    _tprintf_s(_T("[INFO "));
    _ftprintf_s(gLog, _T("[INFO "));
    break;
  }

  _tprintf_s(_T("%s] "), s);
  _ftprintf_s(gLog, _T("%s] "), s);

  _vtprintf_s(format, argptr);
  _vftprintf_s(gLog, format, argptr);
  va_end(argptr);

  _tprintf_s(_T("\n"));
  _ftprintf_s(gLog, _T("\n"));
}