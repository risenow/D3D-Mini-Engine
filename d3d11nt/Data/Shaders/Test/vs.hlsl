#include "vsconstants.h"
#include "psconstants.h"

void VSEntry(in float4 iPos : POSITION
             
        
#ifdef BATCH
        ,in uint iMaterial : TEXCOORD0
		
#endif
,out float4 oPos : SV_POSITION
	   // ,out uint oMaterial : TEXCOORD0
		,out float4 color : COLOR0
		)
{
uint width;
	uint height;
	uint levels;

    // Bypass
	//iPos.z=1.0;
    //oPos = iPos;
	oPos = mul(iPos, viewProjection)*1;//*-100000000;
#ifdef BATCH
	//oMaterial = iMaterial;//float(iMaterial)/2;
	color =float4(PSConstsStructured[iMaterial].coef, 1.0);
#endif
    //oPos =  mul(iPos, viewProjection);
    //oPos =  iPos;
}