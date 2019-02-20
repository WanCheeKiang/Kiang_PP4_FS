#include "ShaderIncludes.hlsl"

cbuffer cbPerFrame :register(b1)
{
	DirectionalLight directLight;
	PointLight ptLight;
	SpotLight stLight;
	float time;
	float3 align;
    float4 CameraPos;
};

cbuffer ConstantBuffer : register(b0)
{
	matrix mWorld;
	matrix mView;
	matrix mProjection;
	float4 outputColor;
  

};
Texture2D ObjTexture : register(t0);
SamplerState ObjSamplerState : register(s0);

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

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	output.Pos = mul(float4(input.Pos.xyz, 1), mWorld);
    output.WorldPos = output.Pos.xyz;
	output.Pos = mul(output.Pos, mView);
	output.Pos = mul(output.Pos, mProjection);
	output.Normal = mul(float4(input.Normal,0), (mWorld)).xyz;
    output.TexCoord = input.TexCoord;

	return output;
}
