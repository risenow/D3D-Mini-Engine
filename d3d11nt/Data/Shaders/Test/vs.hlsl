
#include "vsconstants.h"
#include "psconstants.h"


void VSEntry(in float4 iPos : POSITION 
            ,in float3 iNormal : NORMAL0
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

    //float4x4 viewProjection1 = mul((view), (projection));

    float3 viewVec = normalize(iPos.xyz - wpos.xyz);

    float3 r = reflect(viewVec, normalize(iNormal));
    stc = r;
	
    oWPos = mul(view, iPos);
    oPos = mul(projection, oWPos);//mul(projection, oWPos);
#ifdef BATCH
    color =float4(PSConstsStructured[iMaterial].coef, 1.0);
#endif
}