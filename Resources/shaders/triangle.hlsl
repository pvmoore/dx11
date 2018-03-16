
/// Intrinsic functions:
/// https://msdn.microsoft.com/en-us/library/windows/desktop/ff471376(v=vs.85).aspx

cbuffer ConstantBuffer : register(b0) {
	float c_value = 0;
	float3 _pad;
};
cbuffer MatrixBuffer : register(b1) {
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projMatrix;
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

	result.position = mul(worldMatrix, float4(input.position, 0, 1));
	result.position = mul(viewMatrix, result.position);
	result.position = mul(projMatrix, result.position);

	result.color = saturate(input.color + c_value);
	result.uv    = input.uv;
	return result;
}
float4 PSMain(PSInput input) : SV_TARGET {
	return texture1.Sample(sampler1, input.uv) * input.color;
}
