
#include "vsconstants.h"
#include "psconstants.h"


void VSEntry(in float4 iPos : POSITION 
            ,in float3 iNormal : NORMAL0
#ifdef BATCH
        ,in uint iMaterial : TEXCOORD0

#endif
        ,out float4 oPos : SV_POSITION
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
#endif
}