cbuffer ConstantBuffer : register(b0)
{
    matrix mWorld;
    matrix mView;
    matrix mProjection;
    float4 outputColor;
    float4 CameraPos;

};
cbuffer ReflectionBuffer :  register(b1)
{
    matrix reflectionMatrix;
};
struct VS_INPUT
{
    float4 Pos : POSITION;
    float2 texCoord : TEXCOORD;
};

struct VS_OUTPUT
{
    float4 Pos : POSITION;
    float4 ReflectionPos : TEXCOORD;
    float2 TexCoord : TEXCOORD;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    matrix reflectWorld;
    input.Pos.w = 1.0f;
    output.Pos = mul(input.Pos, mWorld);
    output.Pos = mul(input.Pos, mView);
    output.Pos = mul(input.Pos, mProjection);
    output.TexCoord = input.texCoord;

    reflectWorld = mul(reflectionMatrix, mProjection);
    reflectWorld = mul(mWorld, reflectWorld);

    output.ReflectionPos = mul(input.Pos, reflectWorld);

    return output;
}