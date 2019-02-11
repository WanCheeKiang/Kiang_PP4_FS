#include "ShaderIncludes.hlsl"

cbuffer cbPerFrame : register(b1)
{
    DirectionalLight directLight;
    PointLight ptLight;
    SpotLight stLight;
    float time;
    float3 align;
};



Texture2D ObjTexture : register(t0);
SamplerState ObjSamplerState : register(s0);


struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float3 Normal : NORMAL;
    float2 TexCoord : TEXCOORD;
    float3 WorldPos : WORLDPOS;
};

float4 main(PS_INPUT input) : SV_TARGET
{

    float4 diffuse = ObjTexture.Sample(ObjSamplerState, input.TexCoord);
    float3 normal = normalize(input.Normal);
    float4 finalColor = directLight.ambient * diffuse;
	{
	//finalColor = diffuse * directLight.ambient;
        float3 dir = normalize(-directLight.dir);
        finalColor += dot(dir, normal) * directLight.diffuse * diffuse;
    }
	// point light
	{
        float3 dir = (ptLight.pos - input.WorldPos);
        float distance = length(dir);
        dir = normalize(dir);

        float amountLight = saturate(dot(dir, input.Normal));

        if (amountLight > 0.0f)
        {
            float attenuation = 1.0f - saturate(distance / ptLight.range);
            finalColor += amountLight * diffuse * ptLight.diffuse * attenuation;
        }
    }
	//spot light
	{
        float3 dir = (stLight.pos - input.WorldPos);
        float distance = length(dir);
        dir = normalize(dir);
        float surfaceRatio = saturate(dot(-dir, normalize(stLight.dir)));
        float amountLight = saturate(dot(dir, input.Normal));
		
        if (amountLight > 0.0f)
        {
            float attenuation1 = 1.0f - saturate(distance / stLight.range);
            float attenuation2 = 1.0f - saturate((stLight.InConeRatio - surfaceRatio) / (stLight.InConeRatio - stLight.OutConeRatio));
            finalColor += amountLight * diffuse * stLight.diffuse * attenuation1 * attenuation2;
        }


    }
        finalColor.a = diffuse.a;

        finalColor.b += floor(cos(time + input.WorldPos.x) + 1) * 0.4f;


        return float4(finalColor);
    }

//float4 main2(PS_INPUT input) : SV_TARGET
//{
//	float4 finalColor = 0;
//	input.Normal = normalize(input.Normal);
//	float4  diffuse = ObjTexture.Sample(ObjSamplerState, input.TexCoord);
//	float3 LighttoPixVec = ptLight.pos - input.WorldPos;
//	float distance = length(LighttoPixVec);
//	float3 finalAmbient = diffuse * ptLight.ambient;
//	if (distance > ptLight.range)
//	{
//		return float4(finalAmbient, diffuse.a);
//	}
//
//	LighttoPixVec /= distance;
//
//	float amountLight = dot(LighttoPixVec, input.Normal);
//
//	if (amountLight > 0.0f)
//	{
//		finalColor = amountLight * diffuse * ptLight.diffuse;
//		finalColor /= ptLight.att[0] + (ptLight.att[1] * distance) + (ptLight.att[2] * (distance*distance));
//	}
//	finalColor = saturate(finalColor + float4(finalAmbient,1));
//	finalColor.a = diffuse.a;
//	return finalColor;
//}
//
