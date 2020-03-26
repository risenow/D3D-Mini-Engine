#include "Graphics/ConstantsPreprocessing.h"

#define SSAO_SAMPLE_COUNT 32
#define RAND_VECTOR_COUNT 8

DECLARE_CONSTANT_STRUCTURE SSAOPSConsts
{
    DECLARE_FLOAT4X4 projection;
    DECLARE_FLOAT4X4 invProjection;
    DECLARE_FLOAT4 projFactors;
    DECLARE_FLOAT4 gbufferSize; //24
    DECLARE_FLOAT4 kernelSamples[SSAO_SAMPLE_COUNT]; //24 + 4*32
    DECLARE_FLOAT4 rv[RAND_VECTOR_COUNT]; // 24 + 4*32 + 4*8
    //DECLARE_FLOAT roughness;
    DECLARE_FLOAT4X2 padd1;
    //DECLARE_FLOAT padd2;
    //DECLARE_FLOAT3X3 padd2;
    //DECLARE_FLOAT padd4;
};