//#include "Data/Shaders/Test/psconstants.h"
#include "psconstants.h"
#include "vsconstants.h"

SamplerState SampleType;

TextureCube cubemap: register( t0 );

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
#ifdef BATCH
	return cubemap.Sample(SampleType, stc).xyzw;
#else
    return float4(1.0, 0.0, 0.0, 1.0);
#endif
}