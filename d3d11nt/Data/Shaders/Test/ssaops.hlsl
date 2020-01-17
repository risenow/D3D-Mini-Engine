Texture2D gbufferNormal : register(t0);

float4 PSEntry(float4 pos : SV_POSITION, in float2 uv : TEXCOORD0) : SV_TARGET
{
    float4 vNormalDepth = gbufferNormal.Load(int2(tc_ * gbufferSize.xy), 0);

    float z = vNormalDepth.w;
    float3 n = vNormalDepth.xyz;
    float3 r = normalize(r);
    float3 t = r - dot(n, r)*r;
    float3 b = cross(n, b);//order?

    float4 origin = float4(-pos.x * projFactors.x * z, -pos.y * projFactors.y * z, z, 1.0f);

    float3x3 TBN = float3x3(t, b, n);

    float radius = 1.0f;

    float ao = 0.0;
    for (int i = 0; i < samplesCount; i++)
    {
        float2 Spr = projection * (origin + mul(csamples[i], TBN)*radius);
        ao += gbufferNormal.Load(Spr.xy, 0).w < z;//naive way to do the things, optimum = ray marching + weighing by angle
    }
    ao /= samplesCount;

    return color;
}