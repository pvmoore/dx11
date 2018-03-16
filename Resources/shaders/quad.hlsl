
cbuffer MatrixBuffer : register(b0) {
	matrix viewProj;
};
struct VSInput {
	float2 position	: POSITION;
	float4 color	: COLOR;
	float2 uv		: TEXCOORD;
};
struct PSInput {
	float4 position : SV_POSITION;
	float4 color	: COLOR;
	float2 uv	    : TEXCOORD;
};

Texture2D texture1    : register(t0);
SamplerState sampler1 : register(s0);

PSInput VSMain(VSInput input) {
	PSInput result;
	result.position = mul(viewProj, float4(input.position, 0, 1));
	result.color    = input.color;
	result.uv       = input.uv;
	return result;
}
float4 PSMain(PSInput input) : SV_TARGET {
	return texture1.Sample(sampler1, input.uv) * input.color;
}
