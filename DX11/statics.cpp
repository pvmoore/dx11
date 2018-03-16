#include "_pch.h"

using namespace core;
#include "_exported.h"

/// Set some statics
dx11::DX11* dx11::DX11::self = nullptr;

void throwOnDXError(HRESULT hr, const char* msg) {
	if(FAILED(hr)) {
		string m;
		if(msg) m += msg;
		throw std::runtime_error(String::format("An error occurred: %s", m.c_str()));
	}
}

/// Include implementation of stb_image here
#define STB_IMAGE_IMPLEMENTATION
#include "../External/stb_image.h"
