[numthreads(1, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
}
struct Light
{
	float3 dir;
	float4 ambient;
	float4 diffuse;
	float4 Color;
};

cbuffer cbPerFrame
{
	Light light;
};

cbuffer cbPerObject
{
	float4x4 WVP;
	float4x4 World;
	float4 outputColor;
};

Texture2D ObjTexture;
SamplerState ObjSamplerState;


struct PS_INPUT
{
	float4 Pos : SV_POSITION;
	float3 Normal : NORMAL;
	float2 TexCoord : TEXCOORD0;
	float3 WorldPos : TEXCOORD1;

};

float4 DirectionalLightPS(PS_INPUT input) : SV_TARGET
{
	float4 finalColor = 0;

	finalColor += saturate(dot(light.dir, input.Normal)*light.Color);
	finalColor.a = 1;
	return finalColor;
}
float PSsolid(PS_INPUT input) : SV_Target
{
	return outputColor;
}