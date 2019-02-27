#include "ShaderIncludes.hlsl"

cbuffer cbPerFrame : register(b1)
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
Texture2D ObjTexture1 : register(t1);
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
    clip(diffuse.a - 0.25);
    float3 normal = normalize(input.Normal);
    float specularPower = 246.0f;

    float3 viewDirection = normalize(CameraPos.xyz - input.WorldPos.xyz);

    float4 finalColor = directLight.ambient * diffuse;
	{
       
        float3 dir = normalize(-directLight.dir);
        float lightIntensity = saturate(dot(input.Normal, dir));
        float specular = 0.0f;
        if (lightIntensity > 0.0f)
        {
            finalColor += directLight.ambient * lightIntensity;
            float3 reflection = normalize(reflect(dir, input.Normal));
            specular = pow(saturate(dot(viewDirection, reflection)), specularPower);
        }

        finalColor +=(dot(dir, normal) * directLight.diffuse * diffuse);
        finalColor += specular * float4(1.0f, 1.0f, 1.0f, 1.0f);

    }
	// point light
	{
        float3 dir = (ptLight.pos - input.WorldPos);
        float distance = length(dir);
        dir = normalize(dir);

        float amountLight = saturate(dot(dir, input.Normal));

        float lightIntensity = saturate(dot(input.Normal, normalize(-dir)));
        float specular = 0.0f;
        if (lightIntensity > 0.0f)
        {
            finalColor += amountLight* lightIntensity;
            float3 reflection = normalize(reflect(normalize(-dir), input.Normal));
            specular = pow(saturate(dot(viewDirection, reflection)), specularPower);
        }

        if (amountLight > 0.0f)
        {
            float attenuation = 1.0f - saturate(distance / ptLight.range);

            finalColor += amountLight * diffuse * ptLight.diffuse * attenuation;
            finalColor += specular * float4(1.0f, 1.0f, 1.0f, 1.0f);
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
float4 PS_Soild(PS_INPUT input) : SV_Target
{
    return outputColor;
}

float4 emissive(PS_INPUT input) : SV_Target
{
    float4 FinalColor;
    float4 Color1 = ObjTexture.Sample(ObjSamplerState, input.TexCoord);
    float4 Color2 = ObjTexture1.Sample(ObjSamplerState, input.TexCoord);
    FinalColor = Color1 + Color2;
    return FinalColor;
}