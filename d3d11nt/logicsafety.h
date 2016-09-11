#pragma once
#include <Windows.h>
#include "errorcodesinterpreteutil.h"
#include "Log.h"
#include <assert.h>

#ifdef _DEBUG
#define popAssert(op) assert(op)  
#else
#define popAssert(op) op
#endif

#define WIN_HR_OP(op) { HRESULT hr = op; if (hr != S_OK){ LOG("Operation error: " + WinHRtoStr(hr) + " Line: " + std::to_string(__LINE__)); popAssert(false); }}
#define D3D_HR_OP(op) { HRESULT hr = op; if (hr != S_OK){ LOG("Operation error: " + D3DHRtoStr(hr) + " Line: " + std::to_string(__LINE__)); if (!IsDXGIStatus(hr)) popAssert(false); }}