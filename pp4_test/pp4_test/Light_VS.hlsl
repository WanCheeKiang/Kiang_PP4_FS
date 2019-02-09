[numthreads(1, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
}

struct Light
{
	float3 dir;
	float4 ambient;
	float4 diffuse;
};

cbuffer cbPerFrame : register(b1)
{
	Light light;
};

//cbuffer cbPerObject
//{
//	float4x4 WVP;
//	float4x4 World;
//};

cbuffer ConstantBuffer : register(b0)
{
	matrix mWorld;
	matrix mView;
	matrix mProjection;
	float4 outputColor;

};
Texture2D ObjTexture;
SamplerState ObjSamplerState;

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

VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	output.Pos = mul(float4(input.Pos.xyz, 1), mWorld);
	output.Pos = mul(output.Pos, mView);
	output.Pos = mul(output.Pos, mProjection);
	output.Normal = mul(float4(input.Normal,0), mWorld).xyz;

	output.TexCoord = input.TexCoord;

	return output;
}
