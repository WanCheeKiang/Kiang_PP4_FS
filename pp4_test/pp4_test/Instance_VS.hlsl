#include"ShaderIncludes.hlsl"

cbuffer InstanceObject : register(b0)
{
    matrix mWorld[4];
    matrix mView;
    matrix mProjection;
    matrix space;
};

cbuffer cbPerFrame : register(b1)
{
    DirectionalLight directLight;
    PointLight ptLight;
    SpotLight stLight;
    float time;
    float3 align;
    float4 CameraPos;
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

VS_OUTPUT main(VS_INPUT input, uint id : SV_INSTANCEID)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;
    input.Pos.w = 1.0f;

    output.Pos = mul(input.Pos, mWorld[id]);
 
    output.WorldPos = output.Pos.xyz;
    output.Pos = mul(output.Pos, mView);
    output.Pos = mul(output.Pos, mProjection);

    output.Normal = mul(float4(input.Normal, 0), (mWorld[id])).xyz;
    output.TexCoord = input.TexCoord;

    	
    return output;
}