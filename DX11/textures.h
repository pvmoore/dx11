#pragma once
///
///	https://github.com/Microsoft/DirectXTex
///
///
namespace dx11 {

//================================================================================ Texture2D
class Texture2D final {
public:
	ComPtr<ID3D11Texture2D> texture;
	ComPtr<ID3D11ShaderResourceView> srv;

	void init(ComPtr<ID3D11Device> device, uint2 size, DXGI_FORMAT format, uint bytesPerPixel, const void* initData) {
		D3D11_TEXTURE2D_DESC desc = {};
		desc.Width = size.x;
		desc.Height = size.y;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = format;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA* initDataPtr = nullptr;
		D3D11_SUBRESOURCE_DATA srd = {};
		if(initData) {
			srd.pSysMem          = initData;
			srd.SysMemPitch      = size.x * bytesPerPixel;
			srd.SysMemSlicePitch = size.x * size.y * bytesPerPixel;
			initDataPtr          = &srd;
		}
		throwOnDXError(device->CreateTexture2D(&desc, initDataPtr, texture.GetAddressOf()));

		D3D11_SHADER_RESOURCE_VIEW_DESC srvdesc = {};
		srvdesc.Format = format;
		srvdesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvdesc.Texture2D.MipLevels = 1;

		throwOnDXError(device->CreateShaderResourceView(texture.Get(), &srvdesc, srv.GetAddressOf()));
	}
};
//================================================================================ RWTexture2D
class RWTexture2D final {
public:
	ComPtr<ID3D11Texture2D> texture;
	ComPtr<ID3D11UnorderedAccessView> uav;
	ComPtr<ID3D11ShaderResourceView> srv;

	void init(ComPtr<ID3D11Device> device, uint2 size, DXGI_FORMAT format, uint bytesPerPixel, const void* initData = nullptr) {
		D3D11_TEXTURE2D_DESC desc = {};
		desc.Width = size.x;
		desc.Height = size.y;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = format;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage     = D3D11_USAGE::D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_UNORDERED_ACCESS |
						 D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA* initDataPtr = nullptr;
		D3D11_SUBRESOURCE_DATA srd = {};
		if(initData) {
			srd.pSysMem = initData;
			srd.SysMemPitch = size.x * bytesPerPixel;
			srd.SysMemSlicePitch = size.x * size.y * bytesPerPixel;
			initDataPtr = &srd;
		}
		throwOnDXError(device->CreateTexture2D(&desc, initDataPtr, texture.GetAddressOf()));

		D3D11_UNORDERED_ACCESS_VIEW_DESC uavdesc = {};
		uavdesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
		uavdesc.Format = format;
		uavdesc.Texture2D.MipSlice = 0;

		throwOnDXError(device->CreateUnorderedAccessView(texture.Get(), &uavdesc, uav.GetAddressOf()));

		D3D11_SHADER_RESOURCE_VIEW_DESC srvdesc = {};
		srvdesc.Format = format;
		srvdesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvdesc.Texture2D.MipLevels = 1;

		throwOnDXError(device->CreateShaderResourceView(texture.Get(), &srvdesc, srv.GetAddressOf()));
	}
};
//================================================================================ RWTexture2D
class Textures final {
	DX11& dx11;
public:
	Textures(DX11& dx11) : dx11(dx11) {}
	Texture2D load(const wstring& filename);
private:
	Texture2D loadDDS(const wstring& filename);
};

} /// dx11