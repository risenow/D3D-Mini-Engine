#include "Data/Shaders/Test/constants.h"

void VSEntry(float4 iPos : POSITION,
        out float4 oPos : SV_POSITION)
{
    // Bypass
    oPos =  mul(viewProjection, iPos);
    //oPos =  mul(iPos, viewProjection);
    //oPos =  iPos;
}