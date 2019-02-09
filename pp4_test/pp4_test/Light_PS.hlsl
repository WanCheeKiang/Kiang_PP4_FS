
struct Light
{
	float3 dir;
	float4 ambient;
	float4 diffuse;

};

cbuffer cbPerFrame :register(b1)
{
	Light light;
};

cbuffer ConstantBuffer : register(b0)
{
	matrix mWorld;
	matrix mView;
	matrix mProjection;
	float4 outputColor;
	Light directLight;
};
//cbuffer cbPerObject
//{
//	float4x4 WVP;
//	float4x4 World;
//	float4 outputColor;
//};

Texture2D ObjTexture : register(t0);
SamplerState ObjSamplerState  : register(s0);


struct PS_INPUT
{
	float4 Pos : SV_POSITION;
	float3 Normal : NORMAL;
	float2 TexCoord : TEXCOORD;
	float3 WorldPos : WORLDPOS;
};

float4 main(PS_INPUT input) : SV_TARGET
{
	/*input.Normal = normalize(input.Normal);
	float4 diffuse = ObjTexture.Sample(ObjSamplerState, input.TexCoord);
	float3 finalColor;
	finalColor = diffuse * light.ambient;
	finalColor += saturate(dot(light.dir, input.Normal) * light.diffuse * diffuse);

	return float4(finalColor, diffuse.a);*/
	float4 finalColor = 0;
	float4  diffuse = ObjTexture.Sample(ObjSamplerState, input.TexCoord);
	//finalColor = diffuse * directLight.ambient;
	finalColor += saturate(dot(-directLight.dir, input.Normal) * directLight.diffuse * diffuse);
	finalColor.a = diffuse.a;

	return float4(finalColor);
}
float4 PSsolid(PS_INPUT input) : SV_TARGET
{
	return outputColor;
}