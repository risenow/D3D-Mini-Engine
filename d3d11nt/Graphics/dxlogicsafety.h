#pragma once
#include <Windows.h>
#include "dxerrorcodesinterpreteutil.h"
#include "System/Log.h"
#include <assert.h>

#define D3D_HR_OP(op) { HRESULT hr = op; if (hr != S_OK){ LOG("Operation error: " + D3DHRtoStr(hr) + " Line: " + std::to_string(__LINE__)); /*if (!IsDXGIStatus(hr)) popAssert(false);*/ }}