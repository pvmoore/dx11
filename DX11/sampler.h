#pragma once

namespace dx11 {

class Sampler final {
	D3D11_FILTER _filter = D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	D3D11_TEXTURE_ADDRESS_MODE _addressMode = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP;
	uint _anisotropy = 1;
public:
	ComPtr<ID3D11SamplerState> sampler;
	bool isInitialised = false;

	Sampler& filter(D3D11_FILTER f) {
		_filter = f;
		return *this;
	}
	Sampler& addressMode(D3D11_TEXTURE_ADDRESS_MODE mode) {
		_addressMode = mode;
		return *this;
	}
	Sampler& anisotropy(uint value) {
		assert(value >= 1 && value <= 16);
		_anisotropy = value;
		return *this;
	}
	void init(ComPtr<ID3D11Device> device) {
		if(_anisotropy>1) _filter = D3D11_FILTER::D3D11_FILTER_ANISOTROPIC;

		D3D11_SAMPLER_DESC samplerDesc;
		samplerDesc.Filter = _filter;
		samplerDesc.AddressU = _addressMode;
		samplerDesc.AddressV = _addressMode;
		samplerDesc.AddressW = _addressMode;
		samplerDesc.MipLODBias = 0.0f;
		samplerDesc.MaxAnisotropy = 1;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		samplerDesc.MinLOD = -FLT_MAX;
		samplerDesc.MaxLOD = FLT_MAX;

		throwOnDXError(device->CreateSamplerState(&samplerDesc, sampler.GetAddressOf()));
		isInitialised = true;
	}
};

} /// dx11