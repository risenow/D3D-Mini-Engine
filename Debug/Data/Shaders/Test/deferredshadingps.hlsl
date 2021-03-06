#include "deferredshadingpsconstants.h"

SamplerState SampleType : register(s0);
SamplerComparisonState ShadowSampler : register(s1);

Texture2D gbufferPos: register( t0 );
#ifdef SAMPLES_COUNT
Texture2DMS<float4, SAMPLES_COUNT> gbufferNormal : register( t1 );
Texture2DMS<float4, SAMPLES_COUNT> gbufferColor : register( t2 );
#else
Texture2D gbufferNormal : register(t1);
Texture2D gbufferColor : register(t2);
#endif
TextureCube reflEnv : register(t3);
Texture2D ssao : register(t4);
Texture2D shadowMap : register(t5);

bool isCorrectNormal(float3 n)
{
    return (n.x + n.y + n.z) != 0.0;
}

float3 F0()
{
    return f0roughness.xyz;
}

float Roughness()
{
    return f0roughness.w;
}

float3 schlick(float hl)
{
    float f = pow(1.0 - hl, 5.0);
    float3 f0 = f0roughness.xyz;
    return f0 + (float3(1.0, 1.0, 1.0) - f0)*f;
}

float3 schlick_epic(float hl)
{
#ifdef OPTIMIZED_SCHLICK
    float f = exp2((-5.55473*hl-6.98316)*hl);
    float3 f0 = f0roughness.xyz;
    return f0 + (float3(1.0, 1.0, 1.0) - f0)*f;
#else
    return schlick(hl);
#endif
}

float Dggx(float3 h, float3 n, float a)
{
    float rsqr = a*a;
    float nhsqr = saturate(pow(dot(n, h), 2.0));
    
    return isCorrectNormal(n)?rsqr/(3.14*pow(nhsqr*rsqr - nhsqr + 1, 2.0)):0.0;
}

float Dggxtrow_epic(float hn, float a)
{
    float asqr = a*a;
    float nhsqr = saturate(hn*hn);
    float denom = nhsqr*asqr - nhsqr + 1;
    //return isCorrectNormal(n)?asqr/(3.14*denom*denom)):0.0;
    return asqr/(3.14*denom*denom);
}

float Gggx_epic(float3 nv, float rough)
{
    float k = ((rough + 1)*(rough + 1))/8;
    return nv/(nv * (1 - k) + k);
}

float Gct(float3 l, float3 v, float3 h, float3 n)
{
    float NH = dot(n, h);
    float VH = dot(v, h);
    
    float t1 = 2*NH*dot(v, n)/VH;
    float t2 = 2*NH*dot(n, l)/VH;    
    return min(min(1, t1), t2);
}

float Gggx(float nv, float a)
{
    float cosThetaN = nv;
    float cosTheta_sqr = saturate(cosThetaN*cosThetaN);
    float tan2 = ( 1 - cosTheta_sqr ) / cosTheta_sqr;
    float GP = 2 / ( 1 + sqrt( 1 + a*a * tan2 ) );
    return GP;
}
float Gsmith(float nl, float nv, float a)
{
    return Gggx(nl, a)*Gggx(nv, a);
}
float Gsmith_epic(float nl, float nv, float a)
{
    return Gggx_epic(nl, a)*Gggx_epic(nv, a);
}

float3 CookTorrance(float nl, float nv, float hn, float hv, float3 wv, float3 wn, float3 diff, float a)
{
    float roughness = Roughness();
    a = roughness*roughness;
    
    float3 zeros = float3(0.0, 0.0, 0.0);

    if (nl <= 0.0) return zeros;
    if ((hv/nv) <= 0.0) return zeros;
    //if (hv <= 0.0) return zeros;
 
    float3 r = reflect(wv, normalize(wn));
    float3 envColor = reflEnv.Sample(SampleType, r);
 
    float g = Gsmith_epic(nl, nv, roughness);
    float d = Dggxtrow_epic(hn, a);
    float3 s = schlick_epic(hv);
    
    float3 spec = (g * d * s * 0.25) / (4.0*nv*nl);

    return 3.14*max(spec, 0.0);
}    

#define GAMMA_CORRECTION
//#define REINHARD

float4 PSEntry(
             in float2 tc : TEXCOORD0 
#ifdef SAMPLES_COUNT
            ,uint i : SV_SampleIndex
#endif
            ) : SV_Target
{   
    float2 tc_ = float2(tc.x, 1.0 - tc.y);
    float4 vPos1 = gbufferPos.Sample(SampleType, tc_);

#ifdef SAMPLES_COUNT
    float4 vNormalDepth = gbufferNormal.Load(int2(tc_ * gbufferSize.xy), i);
#else
    float4 vNormalDepth = gbufferNormal.Sample(SampleType, tc_).xyzw;
#endif

    if (vNormalDepth.x == 0.0 && vNormalDepth.y == 0.0 && vNormalDepth.z == 0.0)
        return float4(0.0, 0.0, 0.0, 1.0);

    float z = vNormalDepth.w;
    float3 vNormal = normalize(vNormalDepth.xyz);

#ifdef SAMPLES_COUNT
    float4 diffuseRoughness = gbufferColor.Load(int2(tc_ * gbufferSize.xy), i);
#else
    float4 diffuseRoughness = gbufferColor.Sample(SampleType, tc_);
#endif
    float3 diffuse = diffuseRoughness.rgb;
#ifdef OVERRIDE_DIFFUSE
    diffuse = diffuseOverride.rgb;
#endif
    float rough = diffuseRoughness.a;
    //_f0 = gbufferColor.Sample(SampleType, tc_);
    //float4 diffuse = gbufferColor.Sample(SampleType, tc_);
    float2 ndcXY = tc * 2.0 - 1.0;
    //float4 vPos = float4(-ndcXY.x * projFactors.x * z, -ndcXY.y * projFactors.y * z, z, 1.0f);//for right handed //to use data from texture to check for error here, ERROR IS HERE
    float4 vPos = float4(ndcXY.x * projFactors.x * z, ndcXY.y * projFactors.y * z, z, 1.0f);//for left handed
    float4 vPos2 = float4(ndcXY.x * projFactors.x * z, -ndcXY.y * projFactors.y * z, z, 1.0f);
    float3 l = normalize(-(vPos.xyz - vLightPos.xyz));
    float3 v = normalize(-vPos.xyz);
    
    float4 wPos = mul(invView, vPos);//mul(invView, vPos2); // to use correct vPos
    float3 wNormal = mul(vNormal, (float3x3)invView);
    float3 wViewVec = normalize(wPos.xyz - wCamPos.xyz);
    
    float3 h = normalize(v + l);
    
    float nl = dot(vNormal, l);
    float nv = dot(vNormal, v);
    float hn = dot(h, vNormal);
    float hv = dot(h, v);
    
    //float reflectionWeight = 0.6;
    float ao = ssao.Sample(SampleType, tc_)*0.25 + length(vPos1)*0.0000001;
    float3 specular = CookTorrance(nl, nv, hn, hv, wViewVec, wNormal, diffuse, rough);
    float4 result = float4(specular + diffuse * ao + diffuse * max(nl, 0.0), 1.0);
    
    float4 shadowmapPos = mul(shadowsView, wPos); //there is no sense to seek for mistake there
    shadowmapPos /= shadowmapPos.w;
    shadowmapPos.xy = (shadowmapPos.xy + float2(1.0, 1.0)) * 0.5;//(shadowmapPos + float4(1.0, 1.0, 0.0, 0.0)) * 0.5;
    shadowmapPos.y = 1.0 - shadowmapPos.y;
    float shdepth = shadowMap.Sample(SampleType, shadowmapPos.xy).r;

    float shadowFactor = 0.0;
    float2 pixelSize = float2(1.0, 1.0) / shadowmapSize.xy;
    
    float currentXOffset = -1.0 * pixelSize.x;
    for (int i = 0; i < 5; i++)
    {
        float currentYOffset = -1.0 * pixelSize.y;
        for (int j = 0; j < 5; j++)
        {
            shadowFactor += min((shadowMap.SampleCmpLevelZero(ShadowSampler, shadowmapPos.xy + float2(currentXOffset, currentYOffset), shadowmapPos.z - 0.001).r + 0.35), 1.0);
            currentYOffset += 0.5 * pixelSize.y;
        }
        
        currentXOffset += 0.5 * pixelSize.x;
    }

    /*for (float _x = -1.0 * pixelSize.x; _x < 1.0 * pixelSize.x + 0.001; _x += 0.5 * pixelSize.x)
        for (float _y = -1.0 * pixelSize.y; _y < 1.0 * pixelSize.y + 0.001; _y += 0.5 * pixelSize.y)
            shadowFactor += min((shadowMap.SampleCmpLevelZero(ShadowSampler, shadowmapPos.xy + float2(_x, _y), shadowmapPos.z - 0.001).r + 0.35), 1.0) + shdepth * 0.00001;
    */ 
    //curDepth
    shadowFactor /= 25.0;

    //shadowFactor = min((shadowMap.SampleCmpLevelZero(ShadowSampler, shadowmapPos.xy, shadowmapPos.z - 0.001).r + 0.2), 1.0) + shdepth * 0.00001; //curDepth
#ifdef REINHARD
    result = result / (result / 2.4 + 1.0);
#endif
#ifdef GAMMA_CORRECTION
    result = pow(result, 1.0 / 2.2);
#endif
    //gamma correction
    return (result/0.6) * shadowFactor;
}
