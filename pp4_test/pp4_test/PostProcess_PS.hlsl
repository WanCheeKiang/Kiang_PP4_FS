#include"ShaderIncludes.hlsl"
cbuffer cbPerFrame : register(b0)
{
    DirectionalLight directLight;
    PointLight ptLight;
    SpotLight stLight;
    float time;
    float3 align;
    float4 CameraPos;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float2 TexCoord : TEXCOORD;
};

Texture2D ObjTexture : register(t0);
SamplerState ObjSamplerState : register(s0);

float4 main(PS_INPUT input) : SV_Target
{
    float4 FinalColor;
    
    FinalColor = ObjTexture.Sample(ObjSamplerState, input.TexCoord);
    FinalColor = (FinalColor.r + FinalColor.b + FinalColor.g) / 3; //black and white
    return FinalColor;
}
float4 NoEffect(PS_INPUT input) : SV_Target
{
    float4 FinalColor;
    FinalColor = ObjTexture.Sample(ObjSamplerState, input.TexCoord);
    return FinalColor;
}

float4 warp(PS_INPUT input ) : SV_Target
{
    float4 FinalColor;
    float2 Tex = input.TexCoord - 0.5f;
    float Amp = 0.15f; //amplitude
    float speed = 1.f;
    float offset = 10.0f;
    float x = Amp * sin(speed * time + Tex*offset);
   // FinalColor = ObjTexture.Sample(ObjSamplerState, Tex * x + 0.5f);
    FinalColor = ObjTexture.Sample(ObjSamplerState, input.TexCoord + x);
    return FinalColor;
}