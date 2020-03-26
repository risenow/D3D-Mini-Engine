#include "basicvsconstants.h"

void VSEntry(in float4 pos : POSITION0, out float4 opos : SV_POSITION )
{
    float4x4 cM;
    float4x4 lM;

    float4 vpos = mul(cView, pos);
    vpos = mul(cProj, vpos);
    vpos = mul(lView, vpos);
    vpos = mul(lProj, vpos);
    opos = vpos;//FINALE
}