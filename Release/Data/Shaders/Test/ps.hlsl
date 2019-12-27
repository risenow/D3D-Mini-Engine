
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
            , in float4 diffuseRoughness : COLOR0
            , in float4 vPos : POSITION1
            , in float3 stc : TEXCOORD1
            , in nointerpolation float3 iVLightPos : POSITION2
            , in /*nointerpolation*/ float3 vnormal : NORMAL0
            //, in uint iMaterial : TEXCOORD0
            )// : SV_Target
{
    float3 diffuse = diffuseRoughness.rgb;
    float rough = diffuseRoughness.a;
    
    PS_OUTPUT output;
    
#ifndef GBUFFER_PASS
    const float4 lightpos = float4(0.0, 0.0, -2.0, 1.0);
    float3 lightvec = normalize(-(vPos.xyz - iVLightPos.xyz));
    float coef = dot(normalize(lightvec), normalize(vnormal));

    float reflectionweight = 0.6;
    output.Color = float4((diffuse *coef), 1.0);// (1.0 - reflectionweight) + cubemap.Sample(SampleType, stc).xyzw * reflectionweight) *  coef;
#else
    output.Color = float4(diffuse, rough);
    output.Normal = float4(vnormal, vPos.z);
    output.Position = vPos;
#endif
    
	return output;
}