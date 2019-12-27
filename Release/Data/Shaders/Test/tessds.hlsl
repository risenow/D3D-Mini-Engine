#include "vsconstants.h"
#include "psconstants.h"

struct ConstantOutputType
{
    float edges[3] : SV_TessFactor;
    float inside : SV_InsideTessFactor;
};

struct HullOutputType
{
    float4 position : POSITION;
    uint material: TEXCOORD;
};
//The output of the domain shader goes to the pixel shader. This was previously in the vertex shader in the original tutorial that we are modifying.

struct PixelInputType
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

Texture2D<uint4> tex0 : register( t0 );

////////////////////////////////////////////////////////////////////////////////
// Domain Shader
////////////////////////////////////////////////////////////////////////////////
[domain("tri")]

PixelInputType DSEntry(ConstantOutputType input, float3 uvwCoord : SV_DomainLocation, const OutputPatch<HullOutputType, 3> patch)
{
    float4 vertexPosition;
    PixelInputType output;
 

    // Determine the position of the new vertex.
    vertexPosition = uvwCoord.x * patch[0].position + uvwCoord.y * patch[1].position + uvwCoord.z * patch[2].position;
    
    uint width;
    uint height;
    uint levels;
    tex0.GetDimensions(0, width, height, levels);

    uint3 sampleCoord = uint3(vertexPosition.x*(float)width, vertexPosition.y*(float)height, 0);

    float height_ = (float)tex0.Load(sampleCoord).r/255.0;
    vertexPosition.z += height_/2.0; 
    // Calculate the position of the new vertex against the world, view, and projection matrices.
    float4 vsVertexPosition = mul(view, vertexPosition);
    output.position = mul(projection, vsVertexPosition);//mul(float4(vertexPosition, 1.0f), worldMatrix);
    //output.position = mul(output.position, viewMatrix);
    //output.position = mul(output.position, projectionMatrix);

    // Send the input color into the pixel shader.
#ifdef BATCH
    output.color = float4(PSConstsStructured[patch[0].material].colorRoughness.xyz, 1.0)* uvwCoord.x + float4(PSConstsStructured[patch[1].material].colorRoughness.xyz, 1.0) * uvwCoord.y + float4(PSConstsStructured[patch[2].material].colorRoughness.xyz, 1.0) * uvwCoord.z;//patch[0].color;
#else
    output.color = colorRoughness;
#endif

    return output;
}
