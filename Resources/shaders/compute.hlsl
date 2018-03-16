
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
