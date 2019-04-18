//#include "Data/Shaders/Test/psconstants.h"
#include "psconstants.h"

float4 PSEntry(float4 pos : SV_POSITION
#ifdef BATCH
             , uint material : TEXCOORD
			 , in float4 color : COLOR0
#endif
			 ) : SV_Target
{
    //float c = 0.0;
    //for (int i = 0; i < 4; i++)
    //for (int j = 0; j < 4; j++)
    //{
    //    c += viewProjection[i][j];
    //}
#ifdef BATCH
	return color;//float4(PSConstsStructured[material].coef, 1.0);
	//return float4(0.0, 1.0, 0.0, 1.0);
#else
    return float4(1.0, 0.0, 0.0, 1.0);
#endif
}