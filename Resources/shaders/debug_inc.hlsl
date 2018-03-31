/**
 *	Add the following to your shader:
 *	RWByteAddressBuffer PrintfBuffer : register(u7); // to use reg 7
 *  #include "debug_inc.hlsl"
 *
 *	Encoding:
 *  byte 0 	    = Type
 *  byte 1 	    = components eg 1 = scalar, 4 = vec4
 *  (if type==6): byte 2 is columns of matrix
 *  (if type==6): byte 3 is rows of matrix
 *  byte 2..len = value(s)
 *
 * | Type | Value       | Size(in uints per component) |
 * |------|-------------|----|
 * | 0    | char   		| 1  |
 * | 1    | uint   		| 1  |
 * | 2    | int    		| 1  |
 * | 3    | float  		| 1  |
 * | 4    | ulong  		| 2  |
 * | 5    | slong   	| 2  |
 * | 6    | float (mat) | 1  | *special - num components followed by mat cols and rows
 * | 7    | set suffix  | na | *special - sets suffix for subsequent calls (default is LF 10)
 *
 * eg. 
 * - `1,1, 17` is a uint -> 17
 * - `1,2, 3,7,)` is uint2(3,7)
 * - `3,16, ...` is a mat4
 * - `3,4, ...` is a float4
 * - `6,8,2,4, ...` is a mat2x4 followed by 8 floats
 *
 * Example shader usage:
 * if(printBarrier()) { print(3.14); }
 * 
 */
bool printBarrier() {
    uint orig;
    PrintfBuffer.InterlockedOr(0, 1, orig);
	return orig==0;
}	
uint _addToCounter(uint val) {
    uint i;
    PrintfBuffer.InterlockedAdd(4, val, i);
    return i;
}
void printSetSuffix(int s) {
    uint i = _addToCounter(3);
	if(i>4000000) return;
    i+=2;
    PrintfBuffer.Store3(i*4, uint3(7, 0, s)); // type, components, suffix
}
void _doprint(uint v, uint type) {
    uint i = _addToCounter(3);
	if(i>4000000) return;
    i+=2;
    PrintfBuffer.Store3(i*4, uint3(type, 1, v));    // type, 1 component, value
}
void _doprint(uint2 v, uint type) {
    uint i = _addToCounter(4);
	if(i>4000000) return;
    i+=2;
    PrintfBuffer.Store4(i*4, uint4(type, 2, v.x, v.y));
}
void _doprint(uint3 v, uint type) {
    uint i = _addToCounter(5);
	if(i>4000000) return;
    i+=2;
    PrintfBuffer.Store(i*4, type);
    PrintfBuffer.Store4(i*4+4, uint4(3, v.x, v.y, v.z));
}
void _doprint(uint4 v, uint type) {
    uint i = _addToCounter(6);
	if(i>4000000) return;
    i+=2;
    PrintfBuffer.Store2(i * 4, uint2(type, 4));
    PrintfBuffer.Store4(i * 4 + 8, v);
}
void printm(matrix v) {
    uint i = _addToCounter(20);
	if(i>4000000) return;
    i+=2;
    PrintfBuffer.Store4(i*4, uint4(6, 16, 4, 4));
    i+=4;
    for(uint j = 0; j < 16; j++) {
        PrintfBuffer.Store((i+j)*4, asuint(v[j%4][j/4]));
    }
}
void printm(float4x2 v) { // 4 rows, 2 cols (row-major)
    uint i = _addToCounter(12);
	if(i>4000000) return;
    i += 2;
    PrintfBuffer.Store4(i * 4, uint4(6, 8, 2, 4));
    i += 4;
	for(uint j=0; j<8; j++) {  
        PrintfBuffer.Store((i+j)*4, asuint(v[j%4][j/4]));
	}
}
void printm(float2x4 v) { // 2 rows, 4 cols (row-major)
    uint i = _addToCounter(12);
	if(i>4000000) return;
    i+=2;
    PrintfBuffer.Store4(i * 4, uint4(6, 8, 4, 2));
    i += 4;
	for(uint j=0; j<8; j++) {  
        PrintfBuffer.Store((i + j) * 4, asuint(v[j % 2][j / 2]));
	}
}
void printm(float4x3 v) { // 4 rows, 3 cols (row-major)
    uint i = _addToCounter(16);
	if(i>4000000) return;
    i += 2;
    PrintfBuffer.Store4(i * 4, uint4(6, 12, 3, 4));
    i+=4;
	for(uint j=0; j<12; j++) {  
        PrintfBuffer.Store((i + j) * 4, asuint(v[j % 4][j / 4]));
	}
}
void printm(float3x4 v) { // 3 rows, 4 cols (row-major)
    uint i = _addToCounter(16);
	if(i>4000000) return;
    i+=2;
    PrintfBuffer.Store4(i * 4, uint4(6, 12, 4, 3));
    i += 4;
	for(uint j=0; j<12; j++) {  
        PrintfBuffer.Store((i + j) * 4, asuint(v[j % 3][j / 3]));
	}
}
void printm(float2x2 v) { // 2 rows, 2 cols (row-major)
    uint i = _addToCounter(8);
	if(i>4000000) return;
    i+=2;
    PrintfBuffer.Store4(i * 4, uint4(6, 4, 2, 2));
    i += 4;
	for(uint j=0; j<4; j++) {  
        PrintfBuffer.Store((i + j) * 4, asuint(v[j % 2][j / 2]));
	}
}
void printm(float3x3 v) { // 3 rows, 3 cols (row-major)
    uint i = _addToCounter(8);
	if(i>4000000) return;
    i+=2;
    PrintfBuffer.Store4(i * 4, uint4(6, 9, 3, 3));
    i += 4;
	for(uint j=0; j<9; j++) {  
        PrintfBuffer.Store((i + j) * 4, asuint(v[j % 3][j /3]));
	}
}
void printc(uint v) {
    _doprint(v, 0);
}
void printu(uint v) {
    _doprint(v, 1);
}
void printi(int v) {
    _doprint(v, 2);
}
void print(float v) {
    _doprint(asuint(v), 3);
}
void print(float2 v) {
    _doprint(uint2(asuint(v.x), asuint(v.y)), 3);
}
void print(int2 v) {
    _doprint(uint2(v), 2);
}
void print(uint2 v) {
    _doprint(v, 1);
}
void print(float3 v) {
    _doprint(uint3(asuint(v.x), asuint(v.y), asuint(v.z)), 3);
}
void print(int3 v) {
    _doprint(uint3(v), 2);
}
void print(uint3 v) {
    _doprint(v, 1);
}
void print(float4 v) {
    _doprint(uint4(asuint(v.x), asuint(v.y), asuint(v.z), asuint(v.w)), 3);
}
void print(int4 v) {
    _doprint(uint4(v), 2);
}
void print(uint4 v) {
    _doprint(v, 1);
}

