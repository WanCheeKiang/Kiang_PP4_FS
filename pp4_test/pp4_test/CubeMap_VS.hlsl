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
    float3 TexCoord : TEXCOORD;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    float4 pos = input.Pos;
    pos.w = 1.f;
    pos = mul(pos, mWorld);
    pos = mul(pos, mView);
    output.Pos = mul(pos, mProjection).xyww;
    output.TexCoord = input.Pos.xyz;
    return output;
    
}