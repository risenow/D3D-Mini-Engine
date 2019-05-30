#include "C:/D3D11Engine/D3D-Mini-Engine/d3d11nt/Graphics/ConstantsPreprocessing.h"

DECLARE_CONSTANT_STRUCTURE DeferredShadingPSConsts
{
    DECLARE_FLOAT4 vLightPos;
    DECLARE_FLOAT3X3 padd1;
    DECLARE_FLOAT3 padd2;
};