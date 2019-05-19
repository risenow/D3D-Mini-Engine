//#include "Data/Shaders/Test/psconstants.h"
#include "psconstants.h"
#include "vsconstants.h"

SamplerState SampleType;

TextureCube cubemap: register( t0 );
//Texture2D<uint4> test : register(t1);

float4 PSEntry(float4 pos : SV_POSITION
#ifdef BATCH
			, in float4 color : COLOR0
			, in float4 wPos : POSITION1
			, in float4 iCenter : POSITION2
			, in float3 stc : TEXCOORD1
			, in float3 vsNormal : NORMAL0
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
	//float3 r = reflect(normalize(vsPos.xyz), normalize(vsPos.xyz-iCenter.xyz));
	//float l = sqrt(r.x*r.x+r.y*r.y+(r.z+1)*(r.z+1));
	//float2 smtc = float2(r.x/(2*l)+0.5, 1.0 - (r.y/(2*l)+0.5));
	//float3 tc3=length(float3(vsPos.x, vsPos.y, vsPos.z+1);
	return cubemap.Sample(SampleType, stc).xyzw;//*test.Load(int3(0, 0, 0));//color;//float4(PSConstsStructured[material].coef, 1.0);
	//return float4(0.0, 1.0, 0.0, 1.0);
#else
    return float4(1.0, 0.0, 0.0, 1.0);
#endif
}