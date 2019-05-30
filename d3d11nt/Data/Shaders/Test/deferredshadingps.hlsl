#include "deferredshadingpsconstants.h"

SamplerState SampleType;

Texture2D gbufferPos: register( t0 );
Texture2D gbufferNormal : register( t1 );
Texture2D gbufferColor : register( t2 );


float4 PSEntry(
             in float2 tc : TEXCOORD0
            ) : SV_Target
{   
    float4 vPos = gbufferPos.Sample(SampleType, tc);
    float3 vNormal = gbufferNormal.Sample(SampleType, tc);
    float4 diffuse = gbufferColor.Sample(SampleType, tc);
    
    float3 lightVec = normalize(vPos.xyz - vLightPos.xyz);
    float coef = dot(normalize(lightVec), normalize(vNormal));

    float reflectionWeight = 0.6;
    return (diffuse * coef);//(1.0 - reflectionWeigheight) + cubemap.Sample(SampleType, stc).xyzw * reflectionweight) *  coef;
}