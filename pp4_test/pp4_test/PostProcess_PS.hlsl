struct PS_INPUT
{
    float4 Pos : POSITION;
    float3 Normal : NORMAL;
    float2 TexCoord : TEXCOORD;
};

Texture2D ObjTexture : register(t0);
SamplerState ObjSamplerState : register(s0);

float4 main(PS_INPUT input) :SV_Target
{
    float4 FinalColor = ObjTexture.Sample(ObjSamplerState, input.TexCoord);
    FinalColor.rgb = (FinalColor.r + FinalColor.g + FinalColor.b) / 3;
    FinalColor.a = 1.0f;
    return FinalColor;
}