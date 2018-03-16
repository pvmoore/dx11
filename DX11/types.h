#pragma once

namespace dx11 {

using maths::int2;
using maths::int3;
using maths::int4;
using maths::uint2;
using maths::uint3;
using maths::uint4;
using maths::float2;
using maths::float3;
using maths::float4;
using maths::Camera;
using maths::Camera2D;
using maths::Camera3D;
using maths::matrix;

///================================================================================= rgba
struct rgba final {
	float r, g, b, a;

	rgba() = default;
	constexpr rgba(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) {}
	constexpr rgba(const float4& f) : r(f.x), g(f.y), b(f.z), a(f.w) {}
};
///================================================================================= Rect
struct Rect final {
	float x, y, width, height;

	constexpr float2 point() const { return {x, y}; }
	constexpr float2 dimension() const { return {width, height}; }
};

} /// dx11