
cbuffer MatrixBuffer : register(b0) {
	matrix c_viewProj;
	float4 c_dsColour;
	float2 c_dsOffset;
	float2 _pad;
};
struct VSInput {
	float2 position	: POSITION;
	float2 uv		: TEXCOORD;
	float4 color	: COLOR;
	float size      : SIZE;
};
struct PSInput {
	float4 position : SV_POSITION;
	float4 color	: COLOR;
	float2 uv	    : TEXCOORD;
	float size	    : SIZE;
};

Texture2D texture1    : register(t0);
SamplerState sampler1 : register(s0);

PSInput VSMain(VSInput input) {
	PSInput result;
	result.position = mul(c_viewProj, float4(input.position, 0, 1));
	result.color    = input.color;
	result.uv       = input.uv;
	result.size     = input.size;
	return result;
}
float4 PSMain(PSInput input) : SV_TARGET {
	float smoothing = (1.0 / (0.25*input.size));
	float distance  = texture1.Sample(sampler1, input.uv).r;
	float alpha     = smoothstep(0.5 - smoothing, 0.5 + smoothing, distance);
	return float4(input.color.rgb, input.color.a * alpha);
}
float4 PSMainDropShadow(PSInput input) : SV_TARGET {
	float2 offset = c_dsOffset;
	float smoothing = (1.0 / (0.25*input.size)) * input.size / 12;
	float distance = texture1.Sample(sampler1, input.uv - offset).r;
	float alpha = smoothstep(0.5 - smoothing, 0.5 + smoothing, distance);
	float4 col = c_dsColour;
	return float4(col.rgb, col.a * alpha);
}
