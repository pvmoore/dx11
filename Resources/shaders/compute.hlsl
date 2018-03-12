
cbuffer CBuffer : register(b0) {
	float c_value;
};

struct BufType {
	int i;
	float f;   
};
StructuredBuffer<BufType> Buffer0 : register(t0);
StructuredBuffer<BufType> Buffer1 : register(t1);
RWStructuredBuffer<BufType> BufferOut : register(u0);

//ByteAddressBuffer Buffer2 : register(t2);
//RWByteAddressBuffer Buffer2 : register(t2);

[numthreads(64, 1, 1)]
void CSMain(uint3 gid	    : SV_DispatchThreadID, 
			uint3 threadId  : SV_GroupThreadID,
			uint3 groupId   : SV_GroupID,
			uint groupIndex : SV_GroupIndex)
{
	uint tid = gid.x;
	BufferOut[tid].i = Buffer0[tid].i + Buffer1[tid].i;
	BufferOut[tid].f = Buffer0[tid].f + Buffer1[tid].f + c_value;
}

/*
cbuffer MatrixBuffer : register(b0) {
row_major matrix c_modelMatrix;
row_major matrix c_viewProj;
float3 c_lightPos;
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
	float4 position_worldspace = mul(float4(input.position, 1), c_modelMatrix);
	result.position = mul(position_worldspace, c_viewProj);

	result.normal_worldspace  = mul(float4(input.normal, 0), c_modelMatrix).xyz;
	result.toLight_worldspace = c_lightPos - position_worldspace.xyz;

	result.color    = input.color;
	result.uv       = input.uv;
	return result;
}
float4 PSMain(PSInput input) : SV_TARGET {
	float3 unitNormal = normalize(input.normal_worldspace);
	float3 unitToLight = normalize(input.toLight_worldspace);
	float NdotL = dot(unitNormal, unitToLight);
	float brightness = max(NdotL, 0.1) * 2;
	float4 diffuse = brightness * input.color;

	return texture1.Sample(sampler1, input.uv) * 
		   texture2.Sample(sampler1, input.uv) *
		   diffuse;
}
*/