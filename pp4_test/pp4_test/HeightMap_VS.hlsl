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
    float4 Pos : SV_POSITION;
    float3 Normal : NORMAL;
    float2 TexCoord : TEXCOORD;
    float3 WorldPos : WORLDPOS;
};
Texture2D inputTex : register(t0);
SamplerState ObjSamplerState : register(s0);
VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    output.Pos = mul(float4(input.Pos.xyz, 1), mWorld);
    output.Pos.y = inputTex.SampleLevel(ObjSamplerState, input.TexCoord,0).r*5.0f; //heigth mapset up
    output.WorldPos = output.Pos.xyz;
    output.Pos = mul(output.Pos, mView);
    output.Pos = mul(output.Pos, mProjection);
    output.Normal = mul(float4(input.Normal, 0), (mWorld)).xyz;
    output.TexCoord = input.TexCoord*30;
    return output;
}