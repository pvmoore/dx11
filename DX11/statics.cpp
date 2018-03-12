#include "_pch.h"

using namespace core;
#include "_exported.h"

/// Set some statics

FILE* Log::fp = nullptr;
dx11::DX11* dx11::DX11::self = nullptr;

/// Our singleton Log
Log singletonLog;

void throwOnDXError(HRESULT hr, const char* msg) {
	if(FAILED(hr)) {
		string m;
		if(msg) m += msg;
		throw std::runtime_error(String::format("An error occurred: %s", m.c_str()));
	}
}
string toString(DirectX::XMVECTOR v) {
	char buf[256];
	sprintf_s(buf, "[%.3f, %.3f, %.3f, %.3f]", v.m128_f32[0], v.m128_f32[1], v.m128_f32[2], v.m128_f32[3]);
	return string(buf);
}
string toString(DirectX::XMMATRIX m) {
	char buf[1024];
	sprintf_s(buf, "\n%s\n%s\n%s\n%s",
			  toString(m.r[0]).c_str(),
			  toString(m.r[1]).c_str(),
			  toString(m.r[2]).c_str(),
			  toString(m.r[3]).c_str());
	return string(buf);
}

/// Include implementation of stb_image here
#define STB_IMAGE_IMPLEMENTATION
#include "../External/stb_image.h"
