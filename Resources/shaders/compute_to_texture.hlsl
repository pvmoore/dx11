///
///	Write to 2D texture.
///
/// WG_X = workgroup size X
/// WG_Y = workgroup size Y

StructuredBuffer<float> dataIn : register(t0);

RWTexture2D<unorm float4> targetTexture : register(u0);

// compare to this version
//RWStructuredBuffer<unorm float4> target : register(u0);

[numthreads(WG_X, WG_Y, 1)]
void CSMain(uint3 gid	    : SV_DispatchThreadID,
			uint3 threadId  : SV_GroupThreadID,
			uint3 groupId   : SV_GroupID,
			uint groupIndex : SV_GroupIndex)
{
	uint2 pos = gid.xy;
	uint textureWidth, textureHeight;
	targetTexture.GetDimensions(textureWidth, textureHeight);
	uint readPos = pos.x + pos.y*textureWidth;

	float a = dataIn[readPos];

	targetTexture[pos] = float4(a, 0.2, 0.2, 1); 
}