cbuffer ConstantBuffer : register(b0)
{
    matrix mWorld;
    matrix mView;
    matrix mProjection;
    float4 outputColor;
  

};

struct VS_INPUT
{
    float4 Pos : POSITION;
    float3 Normal : NORMAL;
    float2 TexCoord : TEXCOORD;
};

struct VS_OUTPUT
{
    float4 Pos : POSITION;
    float3 Normal : NORMAL;
    float2 TexCoord : TEXCOORD;
};

Texture2D ObjTexture : register(t0);
SamplerState ObjSamplerState : register(s0);

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output =(VS_OUTPUT)0;

    output.Normal = mul(float4(input.Normal, 0), (mWorld)).xyz;
    output.TexCoord = input.TexCoord;

    return output;
}