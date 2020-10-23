#include "ssaoconstants.h"
SamplerState SampleType;
#ifdef SAMPLES_COUNT
Texture2DMS<float4, SAMPLES_COUNT> gbufferNormal : register(t0);
Texture2DMS<float4, SAMPLES_COUNT> gbufferPos : register(t1);
#else
Texture2D gbufferNormal : register(t0);
Texture2D gbufferPos    : register(t1);
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
    int3 tc = round(int3(uv * gbufferSize.xy, 0));
    float4 vNormalDepth = gbufferNormal.Sample(SampleType, float2(uv.x, uv.y - 1.0/gbufferSize.y)).xyzw;
#endif
    return vNormalDepth;
}

float4 ReadPos(float2 uv)
{
#ifdef SAMPLES_COUNT
    float4 vPos = gbufferPos.Load(int2(uv * gbufferSize.xy), 0);
#else
    int3 tc = int3(uv * gbufferSize.xy, 0);
    float4 vPos = gbufferPos.Load(tc);//gbufferNormal.Sample(SampleType, uv).xyzw;
#endif
    return vPos;
}

float AO(in float2 uv, float3 n, float4 origin) : SV_TARGET
{
    float4 pos = ReadPos(uv).xyzw;
    float3 d = normalize(pos.xyz - origin.xyz);
    float dist = length(d) * 1.5;
    return max(0.0, dot(n, d) + 0.01)*(1.0/(1.0 + dist))*1.0;
}
float3 getOrth(float3 _n, float3 _planeVector)
{
    return _n;
}

float3 GetNormalByScrenD(float dzdx, float dzdy)
{
    float3 n1 = float3(1.0/gbufferSize.x, 0.0, dzdx);
    float3 n2 = float3(0.0, 1.0/gbufferSize.y, dzdy);
    return cross(n2, n1);
}

#define ORIENTED_HEMISPHERE

float4 PSEntry(in float2 uv_ : TEXCOORD0) : SV_TARGET
{

    float2 uv = float2(uv_.x, 1.0 - uv_.y);
    float4 vNormalDepth = ReadNormalLinDepth(uv);

    float z = vNormalDepth.w;
    float2 ndcXY = uv * 2.0 - 1.0;
    float4 origin = ReadPos(uv).xyzw;//float4(-ndcXY.x * projFactors.x * z, ndcXY.y * projFactors.y * z, z, 1.0f);
    float3 n = normalize(vNormalDepth.xyz);
    //float3 n = -normalize(cross(normalize(ddy_fine(origin.xyz)), normalize(ddx_fine((origin.xyz)))));
    float3 r = normalize(float3(0.0, 1.0, 0.0));//(rv[int(abs(nrand(uv)* (RAND_VECTOR_COUNT - 1)))]);
    float3 t = normalize(r - dot(n, r)*n);
    float3 b = normalize(cross(t, n));//order?
    
    
    float4 Opr = mul(projection, origin);
    Opr /= Opr.w;
    Opr = (Opr + float4(1.0, 1.0, 0.0, 0.0))*0.5;
    int3 Otc = int3(uv * gbufferSize.xy, 0);
    
    float3x3 TBN = float3x3(t, b, n);

    float4 pos = ReadPos(uv).xyzw;
    float radius = 0.9f;

    float ao = 0.0;
    float sampleCount = SSAO_SAMPLE_COUNT;
    
    #ifdef ORIENTED_HEMISPHERE
    for (int i = 0; i < SSAO_SAMPLE_COUNT; i++)
    {
        float3 kernelVec = normalize(kernelSamples[i].xyz);
        
        kernelVec = mul(kernelVec, TBN) * radius;
        float4 Svs = (origin + float4(kernelVec.x, kernelVec.y, kernelVec.z, 0.0));
        float4 Spr = mul(projection, Svs);
        float dbg = Spr.x * gbufferSize.x;
        Spr /= Spr.w;
        Spr = float4((Spr.x + 1.0)*0.5, (Spr.y + 1.0)*0.5, Spr.z, Spr.w);
        Spr.y = 1.0 - Spr.y;
        
        float sampleDepth = ReadNormalLinDepth(Spr.xy).w;
        float potential = max((abs(sampleDepth) < abs(Svs.z) - 0.85 - 0.0*(abs(n.y)) ? 1.0 : 0.0), 0.0);
        float rangeCheck = ((kernelVec.z) < 1.0 && potential < 0.1) ? (abs(origin.z - sampleDepth) < radius*1.5 ? 1.0 : 0.0) : 1.0;
        //float outOfRange = (Spr.x < 0.0 || Spr.x > 1.0 || Spr.y < 0.0 || Spr.y > 1.0) ? 1.0 : 0.0;
        
        if (Spr.x > 1.0 || Spr.y > 1.0 || Spr.x < 0.0 || Spr.y < 0.0015 || rangeCheck < 0.13)
        {
            sampleCount--;
            continue;
        }
        
        
        ao += potential;//naive way to do the things, optimum = ray marching + weighing by angle
    }
    sampleCount = max(sampleCount, 1);
    ao /= sampleCount;
    ao = 1.0 - ao;
    #else
    float offset = 2.0;
    ao += AO(uv - getOrth(float3(offset, 0.0, 0.0), r).xy / gbufferSize.xy, n, pos);
    ao += AO(uv - getOrth(float3(0.0, offset, 0.0), r).xy / gbufferSize.xy, n, pos);
    ao += AO(uv + getOrth(float3(offset, 0.0, 0.0), r).xy / gbufferSize.xy, n, pos);
    ao += AO(uv + getOrth(float3(0.0, offset, 0.0), r).xy / gbufferSize.xy, n, pos);
    
    ao /= 1.0;
    ao = 1.0 - ao;
    #endif

    return ao;
}