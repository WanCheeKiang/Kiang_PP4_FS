
struct DirectionalLight
{
	float4 ambient;
	float4 diffuse;
	float3 pos;
	float space;
	float3 dir;
	float align;
};
struct PointLight
{

	float4 diffuse;
	float3 dir;
	float range;
	float3 pos;
	float align;
};

struct SpotLight
{
	float4 diffuse;
	float3 dir;
	float range;
	float3 pos;
	float InConeRatio;
    float OutConeRatio;
    float3 align;
};