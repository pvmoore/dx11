
cbuffer MatrixBuffer : register(b0) {
	matrix c_modelMatrix;
	matrix c_viewProj;
	float3 c_lightPos;
	float _pad;
};
struct VSInput {
	float3 position	: POSITION;
	float3 normal	: NORMAL;
	float4 color	: COLOR;
	float2 uv		: TEXCOORD;
};
struct PSInput {
	float4 position : SV_POSITION;
	float3 normal_worldspace  : NORMAL1;
	float3 toLight_worldspace : TOLIGHT;
	float4 color	: COLOR;
	float2 uv	    : TEXCOORD;
};

Texture2D texture1    : register(t0);
Texture2D texture2    : register(t1);
SamplerState sampler1 : register(s0);

PSInput VSMain(VSInput input) {
	PSInput result;
	float4 position_worldspace = mul(c_modelMatrix, float4(input.position, 1));
	result.position = mul(c_viewProj, position_worldspace);

	result.normal_worldspace  = mul(c_modelMatrix, float4(input.normal, 0)).xyz;
	result.toLight_worldspace = c_lightPos - position_worldspace.xyz;

	result.color    = input.color;
	result.uv       = input.uv;
	return result;
}
float4 PSMain(PSInput input) : SV_TARGET {
	float3 unitNormal  = normalize(input.normal_worldspace);
	float3 unitToLight = normalize(input.toLight_worldspace);
	float NdotL		   = dot(unitNormal, unitToLight);
	float brightness   = max(NdotL, 0.1) * 2;
	float4 diffuse	   = brightness * input.color;

	return texture1.Sample(sampler1, input.uv) * 
		   texture2.Sample(sampler1, input.uv) *
		   diffuse;
}
