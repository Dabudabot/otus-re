#pragma once

#ifndef try
#define try __try
#endif

#ifndef finally
#define finally __finally
#endif

#ifndef leave
#define leave __leave
#endif

#ifndef HERROR
#define HERROR(_s) (S_OK != _s)
#endif

// add offset to base and parse to ptype
#define RVA_TO_VA(ptype, base, offset) \
		(ptype)(((DWORD_PTR)(base)) + (offset))

// leave try block if _exp returned false, BOOL bFlag must be defined
#define IF_FALSE_LEAVE(_exp)  \
          bFlag = _exp;       \
          if (!bFlag) leave;

// leave try block if _result of _exp is INVALID_HANDLE_VALUE or NULL
// also sets HRESULT result (must be defined) to E_UNEXPECTED
#define IF_NULL_LEAVE(_result, _exp) \
          _result = _exp; \
          if (NULL == _result || INVALID_HANDLE_VALUE == _result) { \
            result = E_UNEXPECTED; \
            leave; \
          }

// leave try block if _exp not returned S_OK, HRESULT result must be defined
#define IF_FAIL_LEAVE(_exp)  \
          result = _exp;       \
          if (HERROR(result)) leave;