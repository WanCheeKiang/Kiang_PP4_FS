
cbuffer ConstantBuffer : register(b0)
{
    matrix mWorld;
    matrix mView;
    matrix mProjection;
    float4 outputColor;
};
struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float2 TexCoord : TEXCOORD;
};
Texture2D ObjTexture[2] : register(t0);
SamplerState ObjSamplerState : register(s0);

float4 main(PS_INPUT input):SV_Target
{
    float4 color1;
    float4 color2;
    float4 blendColor;

    color1 = ObjTexture[0].Sample(ObjSamplerState, input.TexCoord);
    color2 = ObjTexture[1].Sample(ObjSamplerState, input.TexCoord);
    blendColor = color1 * color2 * 2.0f;

    blendColor = saturate(blendColor);

    return blendColor;
}