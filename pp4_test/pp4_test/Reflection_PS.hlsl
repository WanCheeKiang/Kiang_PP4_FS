#include "ShaderIncludes.hlsl"

cbuffer ConstantBuffer : register(b0)
{
    matrix mWorld;
    matrix mView;
    matrix mProjection;
    float4 outputColor;

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

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float3 Normal : NORMAL;
    float2 TexCoord : TEXCOORD;
    float3 WorldPos : WORLDPOS;
};
TextureCube CubeMap : register(t0);
SamplerState ObjSamplerState : register(s0);



float4 main(PS_INPUT input) : SV_TARGET
{
    float3 cam_dir = normalize(CameraPos.xyz - input.WorldPos);
    float3 reflectVec = reflect(-cam_dir,input.Normal);

    return float4(CubeMap.Sample(ObjSamplerState, reflectVec).xyz, 1.0f);

}
