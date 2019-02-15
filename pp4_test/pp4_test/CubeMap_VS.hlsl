cbuffer ConstantBuffer : register(b0)
{
    matrix mWorld;
    matrix mView;
    matrix mProjection;
    float4 outputColor;
};
Texture2D ObjTexture;
SamplerState ObjSamplerState;
TextureCube CubeMap;

struct VS_INPUT
{
    float4 Pos : POSITION;
    float3 Normal : NORMAL;
    float2 TexCoord : TEXCOORD;
};

struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float3 TexCoord : TEXCOORD;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    input.Pos.w = 1.0f;
    output.Pos = mul(input.Pos, mWorld);
    output.Pos = mul(input.Pos, mView);
    output.Pos = mul(input.Pos, mProjection).xyww;
    output.TexCoord = input.Pos;
    return output;
    
}