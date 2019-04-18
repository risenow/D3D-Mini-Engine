#include "vsconstants.h"
#include "psconstants.h"

struct VertexInputType
{
    float4 position : POSITION;
    uint material : TEXCOORD;
};

struct HullInputType
{
    float4 position : POSITION;
    uint material : TEXCOORD;
};

Texture2D<float4> tex0 : register( t0 );

////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
HullInputType VSEntry(VertexInputType input)
{
    HullInputType output;

	uint width;
	uint height;
	uint levels;

	tex0.GetDimensions(0, width, height, levels);

	uint3 sampleCoord = uint3(input.position.x*(float)width, input.position.y*(float)height, 0);

	float heightp = tex0.Load(sampleCoord);

    // Pass the vertex position into the hull shader.
    output.position = input.position;
	output.material = input.material;

    // Pass the input color into the hull shader.
    //output.color = float4(PSConstsStructured[input.material].coef, 1.0);//input.color;
    

    return output;
}