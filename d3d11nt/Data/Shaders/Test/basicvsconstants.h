#pragma once

#include "Graphics/ConstantsPreprocessing.h"

DECLARE_CONSTANT_STRUCTURE BasicVSConsts
{
    DECLARE_FLOAT4X4 model;
    DECLARE_FLOAT4X4 projection;
    DECLARE_FLOAT4X4 view;
};
