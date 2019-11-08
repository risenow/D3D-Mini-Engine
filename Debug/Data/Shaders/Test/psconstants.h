#include "C:/D3D11Engine/D3D-Mini-Engine/d3d11nt/Graphics/ConstantsPreprocessing.h"

//static const float test = 0.5f;

DECLARE_MATERIAL_CONSTANT_STRUCTURE PSConsts
{
    DECLARE_FLOAT4 colorRoughness;
    DECLARE_FLOAT4 offset1;
    DECLARE_FLOAT4 offset2;
    DECLARE_FLOAT4 offset3;
    //DECLARE_FLOAT4X4 offset2;
};

DEFINE_MATERIAL_CONSTANT_STRUCTURE(PSConstsStructured, PSConsts)