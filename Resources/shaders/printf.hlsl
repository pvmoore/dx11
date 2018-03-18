
RWByteAddressBuffer PrintfBuffer : register(u7);
#include "debug_inc.hlsl"

[numthreads(64, 1, 1)]
void CSMain(uint3 gid	    : SV_DispatchThreadID, 
			uint3 threadId  : SV_GroupThreadID,
			uint3 groupId   : SV_GroupID,
			uint groupIndex : SV_GroupIndex)
{
	uint tid = gid.x;

    if(printBarrier()) {
        //print(3.14);
        printi(50);
        printi(77);
        print(7.14);
        printu(0xffffeeaa);
        print(uint2(3,4));
        print(int2(-3,-4));
        print(float2(4.1, 5.2));

        print(uint3(5, 6, 7));
        print(int3(-5, -6, -7));
        print(float3(4.1, 5.2, 6.3));

        print(uint4(5, 6, 7, 8));
        print(int4(-5, -6, -7, -8));
        print(float4(4.1, 5.2, 6.3, 7.4));

        matrix m = { 
            0,1,2,3,
            4,5,6,7,
            8,9,10,11,
            12,13,14,15
        };
        printm(m);

        float4x2 m2 = {
            0,1,
            2,3,
            4,5,
            6,7
        };
        printm(m2);

        float2x4 m3 = {
            0,1,2,3,
            4,5,6,7
        };
        printm(m3);

        float4x3 m4 = {
            0,1,2,
            3,4,5,
            6,7,8,
            9,10,11
        };
        printm(m4);

        float3x4 m5 = {
            0,1,2,3,
            4,5,6,7,
            8,9,10,11
        };
        printm(m5);

        float2x2 m6 = {
            0,1,
            2,3
        };
        printm(m6);

        float3x3 m7 = {
            0,1,2,
            3,4,5,
            6,7,8
        };
        printm(m7);
    }
}
