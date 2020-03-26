#include "ssaoconstants.h"
SamplerState SampleType;
#ifdef SAMPLES_COUNT
Texture2DMS<float4, SAMPLES_COUNT> gbufferNormal : register(t0);
#else
Texture2D gbufferNormal : register(t0);
#endif

float nrand(float2 n)
{
    return frac(sin(dot(n.xy, float2(12.9898, 78.233))) * 43758.5453);
}

float4 ReadNormalLinDepth(float2 uv)
{
#ifdef SAMPLES_COUNT
    float4 vNormalDepth = gbufferNormal.Load(int2(uv * gbufferSize.xy), 0);
#else
    int3 tc = int3(uv * gbufferSize.xy, 0);
    float4 vNormalDepth = gbufferNormal.Load(tc);//gbufferNormal.Sample(SampleType, uv).xyzw;
#endif
    return vNormalDepth;
}

float4 PSEntry(in float2 uv_ : TEXCOORD0) : SV_TARGET
{

    float2 uv = float2(uv_.x, 1.0 - uv_.y);
    float4 vNormalDepth = ReadNormalLinDepth(uv);

    float z = vNormalDepth.w;
    float3 n = normalize(vNormalDepth.xyz);
    //n = float3(0.0, 1.0, 0.0);
    //float3 r = normalize(rv[int(abs(nrand(uv)* (RAND_VECTOR_COUNT - 1)))]);
    float3 r = normalize(float3(1.0, 1.0, 1.0));//float3(0.0, 0.0, 1.0);//(rv[int(abs(nrand(uv)* (RAND_VECTOR_COUNT - 1)))]);
    float3 t = normalize(r - dot(n, r)*n);
    float3 b = normalize(cross(t, n));//order?

    float2 ndcXY = uv * 2.0 - 1.0;
    float4 origin = float4(-ndcXY.x * projFactors.x * z, -ndcXY.y * projFactors.y * z, z, 1.0f);
    
    float4 Opr = mul(projection, origin);    
    Opr /= Opr.w;
    Opr = (Opr + float4(1.0, 1.0, 0.0, 0.0))*0.5;


    float3x3 TBN = float3x3(t, b, n);

    float radius = 0.4f + (Opr.x + Opr.y)/100000000000000.0;
    float3 sphN = float3(0.0, 0.0, 1.0);
    float3 sphN1 = mul(sphN, TBN);
    float3 sphN2 = mul(TBN, sphN) ;
    float3 or2 = origin + float3(0.0, -1.0, -2.0);
    float4 O2pr = mul(projection, or2);    
    O2pr /= O2pr.w;
    O2pr = (O2pr + float4(1.0, 1.0, 0.0, 0.0))*0.5;
    int2 orTc = int2(O2pr.xy*gbufferSize.xy);
    float ao = 0.0 + length(sphN1)/100000000.0 +length(sphN2)/100000000.0 + orTc.x/10000000000.0 + orTc.y/100000000.0;
    float sampleCount = SSAO_SAMPLE_COUNT;
    for (int i = 0; i < SSAO_SAMPLE_COUNT; i++)
    {
        float3 kernelVec = normalize(kernelSamples[i].xyz);
        kernelVec.z = -(kernelVec.z);
        //kernelVec = mul((TBN), kernelVec) * radius;
        kernelVec = mul(kernelVec, TBN) * radius;
        float4 Svs = (origin + float4(kernelVec.x, kernelVec.y, kernelVec.z, 0.0));
        float4 Spr = mul(projection, Svs);
        
        Spr /= Spr.w;
        Spr = (Spr + float4(1.0, 1.0, 0.0, 0.0))*0.5;
        //Spr.y = 1.0 - Spr.y;
        float dbg = Spr.x * gbufferSize.x;
        int2 ntc = int2(Spr.xy * gbufferSize.xy);
        float sampleDepth = ReadNormalLinDepth(Spr.xy).w + dbg/100000000000.0;
        float rangeCheck = abs(origin.z - sampleDepth) < radius ? 1.0 : 0.0;
        //float outOfRange = (Spr.x < 0.0 || Spr.x > 1.0 || Spr.y < 0.0 || Spr.y > 1.0) ? 1.0 : 0.0;
        
        //if (Spr.y >= 0.0 && Spr.y <= 1.0 && Spr.x >= 0.0 && Spr.x <= 1.0)
        //{
        if (abs(sampleDepth) < abs(Svs.z))
        ao += 1.0;
        //ao += (abs(sampleDepth) < abs(Svs.z) ? 1.0 : 0.0)  ;//naive way to do the things, optimum = ray marching + weighing by angle
        
        //}
        //else
        //sampleCount--;
    }
    ao /= sampleCount;
    //ao = pow(ao, 2.0);
    ao = 1.0 - ao;
    return ao;
}