#pragma once

namespace dx11 {

///================================================================================= int2
struct int2 final {
	int x, y;

	int2() = default;
	constexpr int2(int x, int y) : x(x), y(y) {}

	constexpr int2 operator+(int2 o) const {
		return {x+o.x, y+o.y};
	}
	constexpr int2 operator-(int2 o) const {
		return {x-o.x, y-o.y};
	}
	constexpr bool operator==(int2 o) const {
		return x==o.x && y==o.y;
	}
	constexpr bool operator!=(int2 o) const {
		return x!=o.x || y!=o.y;
	}
};
///================================================================================= uint2
struct uint2 final {
	uint x, y;

	uint2() = default;
	constexpr uint2(uint x, uint y) : x(x), y(y) {}

	constexpr uint2 operator+(uint2 o) const {
		return {x+o.x, y+o.y};
	}
	constexpr uint2 operator-(uint2 o) const {
		return {x-o.x, y-o.y};
	}
	constexpr bool operator==(uint2 o) const {
		return x==o.x && y==o.y;
	}
	constexpr bool operator!=(uint2 o) const {
		return x!=o.x || y!=o.y;
	}
};
///================================================================================= float2
struct float2 final {
	float x, y;

	float2() = default;
	constexpr float2(float x, float y) : x(x), y(y) {}
	constexpr float2(XMVECTOR v) : x(v.m128_f32[0]), y(v.m128_f32[1]) {}
	constexpr float2(int2 i) : x(float(i.x)), y(float(i.y)) {}
	constexpr float2(uint2 i) : x(float(i.x)), y(float(i.y)) {} 

	constexpr float2 operator+(float o) const {
		return {x+o, y+o};
	}
	constexpr float2 operator+(float2 o) const {
		return {x+o.x, y+o.y};
	}
	constexpr float2 operator*(float o) const {
		return {x*o, y*o};
	}
	constexpr float2 operator*(float2 o) const {
		return {x*o.x, y*o.y};
	}
	constexpr void operator+=(float2 o) {
		x += o.x; y += o.y;
	}
	/// Implicitly convert to XMVECTOR
	constexpr operator XMVECTOR() const {
		return {x, y, 0, 0};
	}
	string toString() const {
		char buf[64];
		sprintf_s(buf, "[%.3f, %.3f]", x, y);
		return string(buf);
	}
};
///================================================================================= float3
struct float3 final {
	float x, y, z;

	float3() = default;
	constexpr float3(float x, float y, float z) : x(x), y(y), z(z) {}
	constexpr float3(XMVECTOR v) : x(v.m128_f32[0]), y(v.m128_f32[1]), z(v.m128_f32[2]) {}

	constexpr float3 operator+(float o) const {
		return {x+o, y+o, z+o};
	}
	constexpr float3 operator+(float3 o) const {
		return {x+o.x, y+o.y, z+o.z};
	}
	constexpr float3 operator*(float o) const {
		return {x*o, y*o, z*o};
	}
	constexpr float3 operator*(float3 o) const {
		return {x*o.x, y*o.y, z*o.z};
	}
	constexpr void operator+=(float3 o) {
		x += o.x; y += o.y; z += o.z;
	}
	/// Implicitly convert to XMVECTOR
	constexpr operator XMVECTOR() const {
		return {x, y, z, 0};
	}
	string toString() const {
		char buf[64];
		sprintf_s(buf, "[%.3f, %.3f, %.3f]", x, y, z);
		return string(buf);
	}
};
///================================================================================= float4
struct float4 final {
	float x, y, z, w;

	float4() = default;
	constexpr float4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
	constexpr float4(XMVECTOR v) : x(v.m128_f32[0]), y(v.m128_f32[1]), z(v.m128_f32[2]), w(v.m128_f32[3]) {}

	constexpr float4 operator+(float o) const {
		return {x + o, y + o, z + o, w + o};
	}
	constexpr float4 operator+(float4 o) const {
		return {x + o.x, y + o.y, z + o.z, w + o.w};
	}
	constexpr float4 operator*(float o) const {
		return {x*o, y*o, z*o, w*o};
	}
	constexpr float4 operator*(float4 o) const {
		return {x*o.x, y*o.y, z*o.z, w*o.w};
	}
	constexpr void operator+=(float4 o) {
		x += o.x; y += o.y; z += o.z; w += o.w;
	}
	/// Implicitly convert to XMVECTOR
	constexpr operator XMVECTOR() const {
		return {x, y, z, w};
	}
	string toString() const {
		char buf[64];
		sprintf_s(buf, "[%.3f, %.3f, %.3f, %.3f]", x, y, z, w);
		return string(buf);
	}
};
///================================================================================= rgba
struct rgba final {
	float r, g, b, a;

	rgba() = default;
	constexpr rgba(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) {}
	constexpr rgba(DirectX::XMVECTORF32 c) : r(c.f[0]), g(c.f[1]), b(c.f[2]), a(c.f[3]) {}

	/// Implicitly convert to XMVECTOR
	constexpr operator XMVECTOR() const {
		return {r,g,b,a};
	}
};
///================================================================================= Rect
struct Rect final {
	float x, y, width, height;

	constexpr float2 point() const { return {x, y}; }
	constexpr float2 dimension() const { return {width, height}; }
};

} /// dx11