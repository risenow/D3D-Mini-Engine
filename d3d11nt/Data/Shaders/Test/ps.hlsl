
#include "vsconstants.h"
#include "psconstants.h"

SamplerState SampleType;

Texture2D diffuseMap: register(t0);
Texture2D normalMap: register(t1);
//TextureCube cubemap: register( t1 );

struct PS_OUTPUT
{
#ifdef GBUFFER_PASS
    float4 PlainNormal: SV_Target3;
    float4 Color: SV_Target2;
    float4 Normal: SV_Target1;
    float4 Position : SV_Target0;
#else
    float4 Color: SV_Target0;
#endif
};

PS_OUTPUT PSEntry(float4 pos : SV_POSITION
#ifdef TEXCOORD
            , in float2 iTc : TEXCOORD0
#endif
            , in float4 diffuseRoughness : COLOR0
            , in float4 vPos : POSITION1
            , in float3 stc : TEXCOORD1
            , in nointerpolation float3 iVLightPos : POSITION2
            , in /*nointerpolation*/ float3 vnormal : NORMAL0
#ifdef TBN
            , in /*nointerpolation*/ float3 vtangent : NORMAL1
            , in /*nointerpolation*/ float3 vbitangent : NORMAL2
#endif
            //, in uint iMaterial : TEXCOORD0
            )
{
#ifdef TEXCOORD
    float4 diffData = diffuseMap.Sample(SampleType, iTc);
    if (diffData.a < 0.01)
    discard;
    
    float3 diffuse = diffData.rgb;
#else
    float3 diffuse = diffuseRoughness.rgb;
#endif
    //float3 diffuse = diffuseRoughness.rgb;
    float rough = diffuseRoughness.a;
    
    float3 normal = vnormal;
#ifdef TBN
#ifdef TEXCOORD
    float3x3 mTBN = float3x3(vtangent, vbitangent, vnormal);
    float3 surfNormal = normalMap.Sample(SampleType, iTc).rgb;
    surfNormal = normalize(surfNormal * 2.0 - 1.0);
    normal = normalize(mul(surfNormal, mTBN));
#endif
#endif

    PS_OUTPUT output;
    
#ifndef GBUFFER_PASS
    const float4 lightpos = float4(0.0, 0.0, -2.0, 1.0);
    float3 lightvec = normalize(-(vPos.xyz - iVLightPos.xyz));
    float coef = dot(normalize(lightvec), normalize(normal));

    float reflectionweight = 0.6;
    output.Color = float4((diffuse *coef), 1.0);// (1.0 - reflectionweight) + cubemap.Sample(SampleType, stc).xyzw * reflectionweight) *  coef;
#else
    output.Color = float4(diffuse, rough);
    output.Normal = float4(normal, vPos.z);
    output.Position = vPos;
    output.PlainNormal = float4(vnormal, vPos.z);
#endif
    
	return output;
}