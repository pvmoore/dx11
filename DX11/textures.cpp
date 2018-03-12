#include "_pch.h"
#include "_exported.h"

namespace dx11 {

using namespace core;

Texture2D Textures::load(const wstring& filename) {
	wstring ext = filename.size() > 3 ? filename.substr(filename.size() - 4) : L"";
	bool isDDS = ext==L".dds";

	if(isDDS) {
		return loadDDS(filename);
	} else {
		throw std::runtime_error("Only DDS and PNG textures are supported");
	}
}
Texture2D Textures::loadDDS(const wstring& filename) {
	Texture2D texture;
	HRESULT hr = DirectX::CreateDDSTextureFromFile(
		dx11.device.Get(),
		dx11.context.Get(),
		filename.c_str(),
		(ID3D11Resource**)texture.texture.GetAddressOf(),
		texture.srv.GetAddressOf()
	);
	if(FAILED(hr)) {
		throw std::runtime_error("Texture load error: " + Win::HRESULTToString(hr));
	} else {
		Log::format("Loaded texture %s", WString::toString(filename).c_str());
	}
	return texture;
}

} /// dx11