struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float3 Normal : NORMAL;
    float2 TexCoord : TEXCOORD;
    float3 WorldPos : WORLDPOS;
};

Texture2D ObjTexture : register(t0);
SamplerState ObjSamplerState : register(s0);

float4 main(PS_INPUT input) :  SV_Target
{
    float4 FinalColor;
    
    FinalColor = ObjTexture.Sample(ObjSamplerState, input.TexCoord);
    FinalColor.a *= 0.5;
    clip(FinalColor.a > 0.1);
    
    return FinalColor;

}