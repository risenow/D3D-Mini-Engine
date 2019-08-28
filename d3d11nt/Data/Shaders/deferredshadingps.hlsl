#include "deferredshadingpsconstants.h"

SamplerState SampleType;

Texture2D gbufferPos: register( t0 );
Texture2D gbufferNormal : register( t1 );
Texture2D gbufferColor : register( t2 );
TextureCube reflEnv : register(t3);
//float _f0;

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

float3 shlick(float3 l, float3 h)
{
    float f = pow(1.0 - dot(h,l), 5.0);
    return F0()*0.05 + (1.0 - F0()*0.05)*f;//(1-pow(saturate(dot(h,l)), 5.00));
}

float Dggx(float3 h, float3 n, float a)
{
    float rsqr = a*a;
    float nhsqr = saturate(pow(dot(n, h), 2.0));
    
    return isCorrectNormal(n)?rsqr/(3.14*pow(nhsqr*rsqr - nhsqr + 1, 2.0)):0.0;
}

float Gct(float3 l, float3 v, float3 h, float3 n)
{
    float NH = dot(n, h);
    float VH = dot(v, h);
    float t1 = 2*NH*dot(v, n)/VH;
    float t2 = 2*NH*dot(n, l)/VH;    
    return min(min(1, t1), t2);
}

float Gggx(float3 e, float3 n, float a)
{
    float cosThetaN = dot(n, e);
    float cosTheta_sqr = saturate(cosThetaN*cosThetaN);
    float tan2 = ( 1 - cosTheta_sqr ) / cosTheta_sqr;
    float GP = 2 / ( 1 + sqrt( 1 + a*a * tan2 ) );
    return GP;
    //float ne = saturate(dot(n, e));// + 0.0001;
    //float rsqr = roughness*roughness;
    //return (2*ne)/(ne + sqrt(rsqr + (1+rsqr)*(ne*ne)));
}
float Gsmith(float3 l, float3 v, float3 h, float a)
{
    return Gggx(l, h, a)*Gggx(v, h, a);
}

float3 CookTorrance(float3 v, float3 n, float3 l, float3 wv, float3 wn, float a)
{
    float3 zeros = float3(0.0, 0.0, 0.0);
    

    if (!isCorrectNormal(v)) return zeros;
    float NL = dot(n, l);
    if (NL <= 0.0) return zeros;
    float NV = dot(n, v);
    if (NV <= 0.0) return zeros;
 
    float3 r = reflect(wv, normalize(wn));
    float3 envColor = reflEnv.Sample(SampleType, r);
    
    v = normalize(v);
    n = normalize(n);
    l = normalize(l);
    float3 h = normalize(v + l);
    //float3(Dggx(h, n), Dggx(h, n), Dggx(h, n));
    //float3(Gsmith(l, v, n)/NV, Gsmith(l, v, n)/NV, Gsmith(l, v, n)/NV);
 
    float g = Gsmith(l, v, n, a);//Gct(l, v, h, n);//
    float d = Dggx(h, n, a);
    float3 s = shlick(v, h);
    
    float spec = (g * d * s * 0.25) / dot(n, v);
    float spec2 = max(spec, float3(0, 0, 0));

    return /*s * envColor*NL/3.14 +*/ spec;//(1-s)*float3(1.0, 1.0, 0.0)*NL/3.14 + spec2;
    //return (Gsmith(l, v, n)*Dggx(h, n)*shlick(v, h)*0.25)/*(dot(n, l)*//dot(n, v);
}    

float4 PSEntry(
             in float2 tc : TEXCOORD0
            ) : SV_Target
{   
    float2 tc_ = float2(tc.x, 1.0 - tc.y);
    float4 vPos1 = gbufferPos.Sample(SampleType, tc_);
    float4 vNormalDepth = gbufferNormal.Sample(SampleType, tc_).xyzw;
    float z = vNormalDepth.w;

    if (z == 1.0)
        return 0.0;

    float3 vNormal = normalize(vNormalDepth.xyz);
    float4 diffuseRoughness = gbufferColor.Sample(SampleType, tc_);
    float3 diffuse = diffuseRoughness.rbg;
    float rough = diffuseRoughness.a;
    //_f0 = gbufferColor.Sample(SampleType, tc_);
    //float4 diffuse = gbufferColor.Sample(SampleType, tc_);
    float2 ndcXY = tc * 2.0 - 1.0;
    float4 vPos = float4(-ndcXY.x * projFactors.x * z, -ndcXY.y * projFactors.y * z, z, 1.0f);
    
    float3 l = normalize(-(vPos.xyz - vLightPos.xyz));
    float3 v = normalize(-vPos.xyz);
    
    float4 wPos = mul(vPos, invView);
    float3 wNormal = mul(vNormal, (float3x3)invView);
    float3 wViewVec = normalize(wPos.xyz - wCamPos.xyz);
    //stc = r;
    //float coef = dot(normalize(lightVec), isCorrectNormal(vNormal) ? normalize(vNormal) : float3(0.0, 0.0, 0.0));

    //float reflectionWeight = 0.6;
    return float4(CookTorrance(v, vNormal, l, wViewVec, wNormal, rough), 1.0);//(diffuse * coef);//(1.0 - reflectionWeigheight) + cubemap.Sample(SampleType, stc).xyzw * reflectionweight) *  coef;
}
