#include "Graphics/ConstantsPreprocessing.h"

DECLARE_CONSTANT_STRUCTURE DeferredShadingPSConsts
{
    DECLARE_FLOAT4X4 invView;
    DECLARE_FLOAT4 vLightPos;
    DECLARE_FLOAT4 projFactors;
    DECLARE_FLOAT4 wCamPos;
    DECLARE_FLOAT4 f0roughness;
    DECLARE_FLOAT4 diffuseOverride;
    DECLARE_FLOAT4 gbufferSize;
    //DECLARE_FLOAT roughness;
    DECLARE_FLOAT4X2 padd1;
    //DECLARE_FLOAT padd2;
    //DECLARE_FLOAT3X3 padd2;
    //DECLARE_FLOAT padd4;
};