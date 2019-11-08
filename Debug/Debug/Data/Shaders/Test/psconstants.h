#include "Graphics/ConstantsPreprocessing.h"

//static const float test = 0.5f;

DECLARE_MATERIAL_CONSTANT_STRUCTURE PSConsts
{
    DECLARE_FLOAT3 coef;
    DECLARE_FLOAT offset1;
    DECLARE_FLOAT4X4 offset2;
};

DEFINE_MATERIAL_CONSTANT_STRUCTURE(PSConstsStructured, PSConsts)