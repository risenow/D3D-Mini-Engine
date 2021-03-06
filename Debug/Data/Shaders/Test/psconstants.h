#include "Graphics/ConstantsPreprocessing.h"

//static const float test = 0.5f;

DECLARE_MATERIAL_CONSTANT_STRUCTURE PSConsts
{
    DECLARE_FLOAT4 colorRoughness;
    DECLARE_FLOAT4 vLightPos;
    DECLARE_FLOAT4 pscoffset1;
    DECLARE_FLOAT4 pscoffset2;
    //DECLARE_FLOAT4 pscoffset3;
    //DECLARE_FLOAT4X4 offset2;
};

DEFINE_MATERIAL_CONSTANT_STRUCTURE(PSConstsStructured, PSConsts)