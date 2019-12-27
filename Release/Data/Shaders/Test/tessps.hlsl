#include "psconstants.h"

struct PixelInputType
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

float4 PSEntry(PixelInputType input) : SV_Target
{
    //float c = 0.0;
    //for (int i = 0; i < 4; i++)
    //for (int j = 0; j < 4; j++)
    //{
    //    c += viewProjection[i][j];
    //}
    return input.color;//float4(PSConstsStructured[material].coef, 1.0);
    //return float4(0.0, 1.0, 0.0, 1.0);
//#else
    //return float4(1.0, 0.0, 0.0, 1.0);
//#endif
}