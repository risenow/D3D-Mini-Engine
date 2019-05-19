//#pragma pack_matrix( row_major )

#include "vsconstants.h"
#include "psconstants.h"
	float4x4 mul_(float4x4 m1, float4x4 m2)
	{
		float4 SrcA0 = m1[0];
		float4 SrcA1 = m1[1];
		float4 SrcA2 = m1[2];
		float4 SrcA3 = m1[3];

		float4 SrcB0 = m2[0];
		float4 SrcB1 = m2[1];
		float4 SrcB2 = m2[2];
		float4 SrcB3 = m2[3];

		float4x4 Result;//(uninitialize);
		Result[0] = SrcA0 * SrcB0[0] + SrcA1 * SrcB0[1] + SrcA2 * SrcB0[2] + SrcA3 * SrcB0[3];
		Result[1] = SrcA0 * SrcB1[0] + SrcA1 * SrcB1[1] + SrcA2 * SrcB1[2] + SrcA3 * SrcB1[3];
		Result[2] = SrcA0 * SrcB2[0] + SrcA1 * SrcB2[1] + SrcA2 * SrcB2[2] + SrcA3 * SrcB2[3];
		Result[3] = SrcA0 * SrcB3[0] + SrcA1 * SrcB3[1] + SrcA2 * SrcB3[2] + SrcA3 * SrcB3[3];
		return Result;
	}

void VSEntry(in float4 iPos : POSITION
             
        
#ifdef BATCH
        ,in uint iMaterial : TEXCOORD0
		
#endif
,out float4 oPos : SV_POSITION
	   // ,out uint oMaterial : TEXCOORD0
		,out float4 color : COLOR0
		,out float4 oWPos : POSITION1
		,out float4 oICenter : POSITION2
		,out float3 stc : TEXCOORD1
		,out float3 oVsNormal : NORMAL0		)
{
uint width;
	uint height;
	uint levels;

	//oICenter = vsCenter;//mul( float4(0.0, 0.0, -3.0, 1.0), view);
	float3 normal = iPos.xyz - wcenter.xyz;//float3(0.0, 0.0, 1.0);
    // Bypass
	//iPos.z=1.0;
    //oPos = iPos;
	float4x4 viewProjection1 = mul((view), (projection));
        //float determinant_ = determinant((float3x3)(normalMatrix));
	//oVsNormal = vsNormal;//determinant_*normalize(mul(normal, (float3x3)(normalMatrix)));
	//float len = length(oVsNormal);
	//oVsPos = len*mul(iPos, view)*1;//*-100000000;

	float3 r = reflect(normalize(iPos.xyz - wpos.xyz), normalize(normal.xyz));
	//float l = sqrt(r.x*r.x+r.y*r.y+(r.z+1)*(r.z+1));
	stc = r;//float2(r.x/(2*l)+0.5, (r.y/(2*l)+0.5));
	
	oPos = mul(iPos, viewProjection);//mul(oVsPos, projection);
#ifdef BATCH
	//oMaterial = iMaterial;//float(iMaterial)/2;
	color =float4(PSConstsStructured[iMaterial].coef, 1.0);
#endif
    //oPos =  mul(iPos, viewProjection);
    //oPos =  iPos;
}