
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
Texture2D ObjTexture1 : register(t0);
Texture2D ObjTexture2 : register(t1);
SamplerState ObjSamplerState : register(s0);

float4 main(PS_INPUT input):SV_Target
{
    float4 color1;
    float4 color2;
    float4 blendColor;

    color1 = ObjTexture1.Sample(ObjSamplerState, input.TexCoord);
    color2 = ObjTexture2.Sample(ObjSamplerState, input.TexCoord);
    blendColor = color1 * color2;

    blendColor = saturate(blendColor);

    return blendColor;
}