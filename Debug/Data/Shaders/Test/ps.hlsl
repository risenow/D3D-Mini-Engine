
#include "vsconstants.h"
#include "psconstants.h"


SamplerState SampleType;

TextureCube cubemap: register( t0 );

struct PS_OUTPUT
{
#ifdef GBUFFER_PASS
    float4 Color: SV_Target2;
    float4 Normal: SV_Target1;
    float4 Position : SV_Target0;
#else
    float4 Color: SV_Target0;
#endif
};

PS_OUTPUT PSEntry(float4 pos : SV_POSITION
#ifdef BATCH
            , in float4 diffuse : COLOR0
            , in float4 vPos : POSITION1
            , in float4 iCenter : POSITION2
            , in float3 stc : TEXCOORD1
            , in float3 vnormal : NORMAL0
            //, in uint iMaterial : TEXCOORD0
#endif
            )// : SV_Target
{
#ifdef BATCH

    
    //float4 diffuse = float4(1.0, 0.0, 0.0, 1.0);
    PS_OUTPUT output;
    
#ifndef GBUFFER_PASS
    const float4 lightpos = float4(0.0, 0.0, -2.0, 1.0);
    float3 lightvec = normalize(vPos.xyz - mul(lightpos, view).xyz);
    float coef = dot(normalize(lightvec), normalize(vnormal));//(0.1*pow(length(lightvec),2.0));

    float reflectionweight = 0.6;
    output.Color = (diffuse * (1.0 - reflectionweight) + cubemap.Sample(SampleType, stc).xyzw * reflectionweight) *  coef;
#else
    output.Color = diffuse;
    output.Normal = float4(vnormal, 1.0);
    output.Position = vPos;
#endif
    
	return output;
#else
    return float4(1.0, 0.0, 0.0, 1.0);
#endif
}