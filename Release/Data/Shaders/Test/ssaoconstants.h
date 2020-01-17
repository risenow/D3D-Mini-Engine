#include "Graphics/ConstantsPreprocessing.h"

#define SSAO_SAMPLE_COUNT 32

DECLARE_CONSTANT_STRUCTURE SSAOPSConsts
{
    DECLARE_FLOAT4 projFactors;
    DECLARE_FLOAT4 gbufferSize;
    DECLARE_FLOAT4 kernelSamples[SSAO_SAMPLE_COUNT];
    //DECLARE_FLOAT roughness;
    DECLARE_FLOAT4X2 padd1;
    //DECLARE_FLOAT padd2;
    //DECLARE_FLOAT3X3 padd2;
    //DECLARE_FLOAT padd4;
};