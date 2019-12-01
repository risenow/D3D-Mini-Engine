#pragma once

#include "Graphics/ConstantsPreprocessing.h"

struct InstVSConsts
{
    DECLARE_FLOAT4X4 model;
};

#ifndef CPU_SIDE_CODE
StructuredBuffer< InstVSConsts > inst;
#endif