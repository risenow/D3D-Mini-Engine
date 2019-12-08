#include "basicvsconstants.h"

void VSEntry(in float4 pos : POSITION0, in float4 icolor : COLOR0, out float4 opos : SV_POSITION, out float4 color : NORMAL0)
{
    color = icolor;
    float4 vpos = mul(view, pos);
    opos = mul(projection, vpos);
	//return opos;
}