#include"ShaderIncludes.hlsl"

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
    FinalColor = (FinalColor.r + FinalColor.b + FinalColor.g) / 3;
    return FinalColor;
}
float4 NoEffect(PS_INPUT input) : SV_Target
{
    float4 FinalColor;
    FinalColor = ObjTexture.Sample(ObjSamplerState, input.TexCoord);
    return FinalColor;
}