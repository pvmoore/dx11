
struct PrintfBufType final {
    uint flags;
    uint counter;
    float f[1024 * 1024];
};
RWStructuredBuffer<PrintfBufType> BufferOut : register(u0);

[numthreads(64, 1, 1)]
void CSMain(uint3 gid	    : SV_DispatchThreadID, 
			uint3 threadId  : SV_GroupThreadID,
			uint3 groupId   : SV_GroupID,
			uint groupIndex : SV_GroupIndex)
{
	uint tid = gid.x;


}
