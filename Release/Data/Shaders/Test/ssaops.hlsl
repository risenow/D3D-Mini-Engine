float4 PSEntry(float4 pos : SV_POSITION, in float2 uv : TEXCOORD0) : SV_TARGET
{
    float depth = ReadDepth(uv);
    return color;
}