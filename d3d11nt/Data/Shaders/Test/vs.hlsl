#include "vsconstants.h"
#include "psconstants.h"


void VSEntry(in float4 iPos : POSITION 
            ,in float3 iNormal : NORMAL0
#ifdef TBN
            ,in float3 iTangent : NORMAL1
            ,in float3 iBitangent : NORMAL2
#endif
#ifdef BATCH // INCORRECT! (BEN GOLD)
        ,in uint iMaterial : TEXCOORD0

#else
#ifdef TEXCOORD
        ,in float2 iTc : TEXCOORD0
#endif
#endif
        ,out float4 oPos : SV_POSITION
#ifndef BATCH
#ifdef TEXCOORD
	    ,out float2 oTc : TEXCOORD0
#endif
#endif
        ,out float4 diffuseRoughness : COLOR0
        ,out float4 oVPos : POSITION1
        ,out float3 stc : TEXCOORD1
        ,out nointerpolation float3 oVLightPos : POSITION2
        ,out /*nointerpolation*/ float3 oVNormal : NORMAL0
#ifdef TBN
        ,out /*nointerpolation*/ float3 oVTangent : NORMAL1
        ,out /*nointerpolation*/ float3 oVBitangent : NORMAL2
#endif
)
{
    uint width;
    uint height;
    uint levels;

    oVNormal = mul((float3x3)view, iNormal);
#ifdef TBN
    oVTangent = mul((float3x3)view, iTangent);
    oVBitangent = mul((float3x3)view, iBitangent);
#endif
    oVPos = mul(view, iPos);

    float3 viewVec = normalize(iPos.xyz - wpos.xyz);

    float3 r = reflect(viewVec, normalize(iNormal));
    stc = r;
	
    float4 wPos = mul(model, iPos);
    float4 vPos = mul(view, wPos);
    oPos = mul(projection, vPos);
#ifdef BATCH
    diffuseRoughness = PSConstsStructured[iMaterial].colorRoughness;
    oVLightPos = PSConstsStructured[iMaterial].vLightPos.xyz;
#else
#ifdef TEXCOORD
    oTc = iTc;
#endif
    diffuseRoughness = colorRoughness;
    oVLightPos = vLightPos.xyz;
#endif
}