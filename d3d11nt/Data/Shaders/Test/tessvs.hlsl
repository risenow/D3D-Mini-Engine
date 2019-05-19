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

////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
HullInputType VSEntry(VertexInputType input)
{
    HullInputType output;

    // Pass the vertex position into the hull shader.
    output.position = input.position;
    output.material = input.material;//*PSConstsStructured[input.material].coef.x;

    // Pass the input color into the hull shader.
    //output.color = float4(PSConstsStructured[input.material].coef, 1.0);//input.color;
    

    return output;
}