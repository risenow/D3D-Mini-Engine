#include "basicinstvsconstants.h

float4 VSEntry(in float4 pos : POSITION0, in float4 icolor, out float4 ocolor) : SV_POSITION
{
    ocolor = icolor;
    float4 vpos = mul(view, wpos);
    float4 opos = mul(projection, vpos);
	return opos;
}