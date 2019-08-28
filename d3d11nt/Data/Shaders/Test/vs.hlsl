#include "vsconstants.h"
#include "psconstants.h"


void VSEntry(in float4 iPos : POSITION 
            ,in float3 iNormal : NORMAL0
#ifdef BATCH
        ,in uint iMaterial : TEXCOORD0

#endif
        ,out float4 oPos : SV_POSITION
	    //,out uint oMaterial : TEXCOORD0
        ,out float4 diffuseRoughness : COLOR0
        ,out float4 oVPos : POSITION1
        ,out float4 oICenter : POSITION2
        ,out float3 stc : TEXCOORD1
        ,out float3 oVNormal : NORMAL0)
{
    uint width;
    uint height;
    uint levels;

    //float4x4 viewProjection1 = mul((view), (projection));

    oVNormal = mul(iNormal, (float3x3)view);
    oVPos = mul(iPos, view);;

    float3 viewVec = normalize(iPos.xyz - wpos.xyz);

    float3 r = reflect(viewVec, normalize(iNormal));
    stc = r;
	
    
    float4 vPos = mul(iPos, view);
    oPos = mul(vPos, projection);
    //float4 vPos = mul(view, iPos);
    //oPos = mul(projection, vPos);
#ifdef BATCH
    diffuseRoughness =PSConstsStructured[iMaterial].colorRoughness;//float4(PSConstsStructured[iMaterial].coef, 1.0);
#endif
}