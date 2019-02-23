
struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float2 TexCoord : TEXCOORD;
};

VS_OUTPUT main(uint vI : SV_VERTEXID)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;
    output.TexCoord = float2(vI & 1, vI >> 1); //you can use these for texture coordinates later
    output.Pos = float4((output.TexCoord.x - 0.5f) * 2, -(output.TexCoord.y - 0.5f) * 2, 0, 1);
    return output;

}