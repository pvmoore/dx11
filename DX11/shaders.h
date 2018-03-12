#pragma once

namespace dx11 {

struct VertexShader final {
	ComPtr<ID3D11VertexShader> handle;
	ComPtr<ID3DBlob> blob;
	inline operator ID3D11VertexShader*() const { return handle.Get(); }
};
struct HullShader final {
	ComPtr<ID3D11HullShader> handle;
	ComPtr<ID3DBlob> blob;
	inline operator ID3D11HullShader*() const { return handle.Get(); }
};
struct DomainShader final {
	ComPtr<ID3D11DomainShader> handle;
	ComPtr<ID3DBlob> blob;
	inline operator ID3D11DomainShader*() const { return handle.Get(); }
};
struct GeometryShader final {
	ComPtr<ID3D11GeometryShader> handle;
	ComPtr<ID3DBlob> blob;
	inline operator ID3D11GeometryShader*() const { return handle.Get(); }
};
struct PixelShader final {
	ComPtr<ID3D11PixelShader> handle;
	ComPtr<ID3DBlob> blob;
	inline operator ID3D11PixelShader*() const { return handle.Get(); }
};
struct ComputeShader final {
	ComPtr<ID3D11ComputeShader> handle;
	ComPtr<ID3DBlob> blob;
	inline operator ID3D11ComputeShader*() const { return handle.Get(); }
};
//======================================================================================

class Shaders final {
	class DX11& dx11;
public:
	enum Target {
		_5_0
	};
	//vector<ID3DInclude> _includes;
	Target target = Target::_5_0;

	Shaders(DX11& dx11) : dx11(dx11) {}
	VertexShader getVS(const wstring& filename, string entry = "") const;
	HullShader getHS(const wstring& filename, string entry = "") const;
	DomainShader getDS(const wstring& filename, string entry = "") const;
	GeometryShader getGS(const wstring& filename, string entry = "") const;
	PixelShader getPS(const wstring& filename, string entry = "") const;
	ComputeShader getCS(const wstring& filename, string entry = "", D3D_SHADER_MACRO* defines=nullptr) const;
private:
	ComPtr<ID3DBlob> compile(const wstring& filename, string entry, string target, D3D_SHADER_MACRO* defines = nullptr) const;
};

} /// dx11