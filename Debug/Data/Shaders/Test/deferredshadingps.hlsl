//#include "Data/Shaders/Test/psconstants.h"
#include "deferredshadingpsconstants.h"
//#include "vsconstants.h"

SamplerState SampleType;

Texture2D gbufferPos: register( t0 );
Texture2D gbufferNormal : register( t1 );
Texture2D gbufferColor : register( t2 );

//struct PS_OUTPUT
//{
 //   float4 Color: SV_Target0;
//};

float4 PSEntry(
             in float2 tc : TEXCOORD0
            ) : SV_Target
{   
    float4 vPos = gbufferPos.Sample(SampleType, tc);
    float3 vNormal = gbufferNormal.Sample(SampleType, tc);
    float4 diffuse = gbufferColor.Sample(SampleType, tc);
    
    //const float4 lightpos = float4(0.0, 0.0, -2.0, 1.0);
    float3 lightVec = normalize(vPos.xyz - vLightPos.xyz);
    float coef = dot(normalize(lightVec), normalize(vNormal));//(0.1*pow(length(lightvec),2.0));

    float reflectionWeight = 0.6;
    return (diffuse * coef);//(1.0 - reflectionWeigheight) + cubemap.Sample(SampleType, stc).xyzw * reflectionweight) *  coef;
}