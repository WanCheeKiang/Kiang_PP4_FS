cbuffer ConstantBuffer : register(b0)
{
    matrix mWorld;
    matrix mView;
    matrix mProjection;
    float4 outputColor;
};

SamplerState ObjSamplerState : register(s0);
TextureCube CubeMap : register(t0);

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float3 TexCoord : TEXCOORD;
};

float4 main(PS_INPUT input) :SV_Target
{
    return CubeMap.Sample(ObjSamplerState, input.TexCoord);
}