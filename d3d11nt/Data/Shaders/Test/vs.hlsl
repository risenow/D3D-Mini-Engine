
#include "vsconstants.h"
#include "psconstants.h"


void VSEntry(in float4 iPos : POSITION 
#ifdef BATCH
        ,in uint iMaterial : TEXCOORD0

#endif
        ,out float4 oPos : SV_POSITION
	   // ,out uint oMaterial : TEXCOORD0
        ,out float4 color : COLOR0
        ,out float4 oWPos : POSITION1
        ,out float4 oICenter : POSITION2
        ,out float3 stc : TEXCOORD1
        ,out float3 oVsNormal : NORMAL0)
{
    uint width;
    uint height;
    uint levels;

    float3 normal = iPos.xyz - wcenter.xyz;
    float4x4 viewProjection1 = mul((view), (projection));

    float3 r = reflect(normalize(iPos.xyz - wpos.xyz), normalize(normal.xyz));
    stc = r;
	
    oWPos = mul(translatedview, iPos);
    oPos = mul(projection, oWPos);
#ifdef BATCH
    color =float4(PSConstsStructured[iMaterial].coef, 1.0);
#endif
}