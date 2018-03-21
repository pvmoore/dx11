#pragma once
///
/// Todo: 
///		This recompiles from file each time a shader is created. If many shaders are in the same
///		file then D3DCompile can be used instead to compile once and then the required shaders
///		created from that.
///
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
struct ShaderArgs final {
    friend class Shaders;
private:
    vector<D3D_SHADER_MACRO> _defines = {{nullptr, nullptr}};
    //vector<ID3DInclude> _includes;
    string _target;
    string _entry;
    uint _options = 0;
    uint _optionsAdd = 0;
    uint _optionsRemove = 0;
    bool _verbose = false;
public:
    auto& define(const char* name, const char* value) {
        _defines.back() = {name, value};
        _defines.push_back({nullptr, nullptr});
        return *this;
    }
    //auto& include(const ID3DInclude& incl) { throw std::runtime_error("Not implemented"); return *this; }
    auto& entry(const string& entry) { _entry = entry; return *this; }
    auto& target(const string& t) { _target = t; return *this; }
    auto& options(uint absolute, uint add=0, uint remove=0) { 
        _options = absolute; 
        _optionsAdd = add;
        _optionsRemove = remove;
        return *this; 
    }
    auto& verbose() { _verbose = true; return *this; }
};
//======================================================================================
class Shaders final {
	class DX11& dx11;
public:
	Shaders(DX11& dx11) : dx11(dx11) {}
	HullShader makeHS(const wstring& filename, const ShaderArgs& args) const;
	DomainShader makeDS(const wstring& filename, const ShaderArgs& args) const;
	GeometryShader makeGS(const wstring& filename, const ShaderArgs& args) const;
	PixelShader makePS(const wstring& filename, const ShaderArgs& args) const;
    VertexShader makeVS(const wstring& filename, const ShaderArgs& args) const;
    ComputeShader makeCS(const wstring& filename, const ShaderArgs& args) const;
private:
    uint getOptions(const ShaderArgs& args) const;
	ComPtr<ID3DBlob> compile(const wstring& filename, 
                             const string& entry, 
                             const string& target, 
                             const D3D_SHADER_MACRO* defines,
                             uint options,
                             bool verbose) const;
};

} /// dx11