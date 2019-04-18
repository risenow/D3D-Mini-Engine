#include "vsconstants.h"

void VSEntry(in float4 iPos : POSITION
             
        
#ifdef BATCH
        ,in uint iMaterial : TEXCOORD0
		
#endif
,out float4 oPos : SV_POSITION
	    ,out uint oMaterial : TEXCOORD0
		)
{
    // Bypass
	//iPos.z=1.0;
    oPos = iPos;// mul(viewProjection, iPos)*1;//*-100000000;
#ifdef BATCH
	oMaterial = iMaterial;//float(iMaterial)/2;
#endif
    //oPos =  mul(iPos, viewProjection);
    //oPos =  iPos;
}