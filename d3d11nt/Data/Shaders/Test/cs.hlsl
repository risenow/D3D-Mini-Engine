RWTexture2D<float4> backbuffer;

groupshared float4 lds[1024];

static float gauss[] =
	{
		0.028532,	0.067234,	0.124009,	0.179044,	0.20236
		//0.06136f, 0.25f, 0.39f
	};
// 0.028532	0.067234	0.124009	0.179044	0.20236
float4 LoadLDS(uint2 c)
{
return lds[c.y*32 + c.x];
}

#ifdef HORIZONTAL
#define offset_x 1
#define offset_y 0
#endif
#ifdef VERTICAL
#define offset_x 0
#define offset_y 1
#endif
[numthreads(32,32,1)]
void CSEntry(uint3 Gid : SV_GroupID, uint3 Tid : SV_GroupThreadID, uint3 id : SV_DispatchThreadID)
{
lds[Tid.y*32 + Tid.x] = backbuffer[id.xy];

GroupMemoryBarrierWithGroupSync();


float4 color = LoadLDS(Tid.xy) * 0.20236f;


//#ifdef HORIZONTAL
float temp[4];
for (uint i = 0; i < 4; i++)
color += LoadLDS(uint2(clamp((int2(Tid.xy) - int2(offset_x*(i+1), offset_y*(i+1))), 0, 31))) * gauss[3 - i];
//#endif
for (uint i = 0; i < 4; i++)
color += LoadLDS(uint2(clamp((int2(Tid.xy) + int2(offset_x*(i+1), offset_y*(i+1))), 0, 31))) * gauss[3 - i];

//uint2 temp1 = uint2(clamp((int2(Tid.xy) - int2(1, 0)), 0, 31));
//uint2 temp2 = uint2(clamp((int2(Tid.xy) - int2(2, 0)), 0, 31));
//uint2 temp3 = uint2(clamp((int2(Tid.xy) - int2(3, 0)), 0, 31));
//uint2 temp4 = uint2(clamp((int2(Tid.xy) - int2(4, 0)), 0, 31));
//uint2 temp2 = uint2(clamp((int2(Tid.xy) - int2(5, 0)), 0, 31));
//float4 color = LoadLDS(clamp(Tid.xy, 0, 31)) * 0.39f;
//float4 color = LoadLDS(Tid.xy)*1.0;//(0.0, 0.0, 0.0, 1.0);

//color += LoadLDS(temp1 ) * 0.179044f;
//color += LoadLDS(temp2 ) * 0.124009f;
//color += LoadLDS(temp3 ) * 0.067234f;
//color += LoadLDS(temp4 ) * 0.028532f;

//temp1 = uint2(clamp((int2(Tid.xy) + int2(1, 0)), 0, 31));
//temp2 = uint2(clamp((int2(Tid.xy) + int2(2, 0)), 0, 31));
//temp3 = uint2(clamp((int2(Tid.xy) + int2(3, 0)), 0, 31));
//temp4 = uint2(clamp((int2(Tid.xy) + int2(4, 0)), 0, 31));

//color += LoadLDS(temp1 ) * 0.179044f;
//color += LoadLDS(temp2 ) * 0.124009f;
//color += LoadLDS(temp3 ) * 0.067234f;
//color += LoadLDS(temp4 ) * 0.028532f;

backbuffer[id.xy] = color;


//backbuffer[id.xy] = float4(1.0, 0.0, 0.0, 1.0);
}