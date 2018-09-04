#include "_pch.h"
#include "_exported.h"

namespace dx11 {

using namespace core;

static vector<string> getOptionsAsString(uint options) {
    vector<string> array;
    if(options&D3DCOMPILE_ENABLE_STRICTNESS) array.emplace_back("D3DCOMPILE_ENABLE_STRICTNESS");
    if(options&D3DCOMPILE_PARTIAL_PRECISION) array.emplace_back("D3DCOMPILE_PARTIAL_PRECISION");
    if(options&D3DCOMPILE_AVOID_FLOW_CONTROL) array.emplace_back("D3DCOMPILE_AVOID_FLOW_CONTROL");
    if(options&D3DCOMPILE_PREFER_FLOW_CONTROL) array.emplace_back("D3DCOMPILE_PREFER_FLOW_CONTROL");
    if(options&D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR) array.emplace_back("D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR");
    if(options&D3DCOMPILE_PACK_MATRIX_ROW_MAJOR) array.emplace_back("D3DCOMPILE_PACK_MATRIX_ROW_MAJOR");
    if(options&D3DCOMPILE_WARNINGS_ARE_ERRORS) array.emplace_back("D3DCOMPILE_WARNINGS_ARE_ERRORS");
    if(options&D3DCOMPILE_ALL_RESOURCES_BOUND) array.emplace_back("D3DCOMPILE_ALL_RESOURCES_BOUND");
    if(options&D3DCOMPILE_DEBUG) array.emplace_back("D3DCOMPILE_DEBUG");
    if(options&D3DCOMPILE_SKIP_OPTIMIZATION) array.emplace_back("D3DCOMPILE_SKIP_OPTIMIZATION");
    if(options&D3DCOMPILE_DEBUG_NAME_FOR_SOURCE) array.emplace_back("D3DCOMPILE_DEBUG_NAME_FOR_SOURCE");
    if(options&D3DCOMPILE_OPTIMIZATION_LEVEL0) array.emplace_back("D3DCOMPILE_OPTIMIZATION_LEVEL0");
    if(options&D3DCOMPILE_OPTIMIZATION_LEVEL3) array.emplace_back("D3DCOMPILE_OPTIMIZATION_LEVEL3");
    if(options&D3DCOMPILE_SKIP_VALIDATION) array.emplace_back("D3DCOMPILE_SKIP_VALIDATION");
    return array;
}
static uint getDefaultOptions() {
	uint compileOpts =
		D3DCOMPILE_ENABLE_STRICTNESS |
		D3DCOMPILE_PARTIAL_PRECISION |
		//D3DCOMPILE_AVOID_FLOW_CONTROL |
		//D3DCOMPILE_PREFER_FLOW_CONTROL |
		D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR |	
		D3DCOMPILE_WARNINGS_ARE_ERRORS |
        D3DCOMPILE_ALL_RESOURCES_BOUND;

#ifdef _DEBUG
	compileOpts |= 
        D3DCOMPILE_DEBUG |
		D3DCOMPILE_SKIP_OPTIMIZATION |
		D3DCOMPILE_DEBUG_NAME_FOR_SOURCE;
#else
    compileOpts |= 
        D3DCOMPILE_OPTIMIZATION_LEVEL3 |
        0;
#endif
	return compileOpts;
}
VertexShader Shaders::makeVS(const wstring& filename, const ShaderArgs& args) const {
    ComPtr<ID3D11VertexShader> sh;
    auto entry = args._entry.empty() ? "VSMain" : args._entry;
    auto target = args._target.empty() ? "vs_5_0" : args._target;
    auto blob = compile(filename, entry, target, args._defines.data(), getOptions(args), args._verbose);
    if(blob) {
        throwOnDXError(dx11.device->CreateVertexShader(
            blob->GetBufferPointer(),
            blob->GetBufferSize(),
            nullptr,
            sh.GetAddressOf()
        ));
    }
    return {sh, blob};
}
HullShader Shaders::makeHS(const wstring& filename, const ShaderArgs& args) const {
    ComPtr<ID3D11HullShader> sh;
    auto entry = args._entry.empty() ? "HSMain" : args._entry;
    auto target = args._target.empty() ? "hs_5_0" : args._target;
    auto blob = compile(filename, entry, target, args._defines.data(), getOptions(args), args._verbose);
    if(blob) {
        throwOnDXError(dx11.device->CreateHullShader(
            blob->GetBufferPointer(),
            blob->GetBufferSize(),
            nullptr,
            sh.GetAddressOf()
        ));
    }
    return {sh, blob};
}
DomainShader Shaders::makeDS(const wstring& filename, const ShaderArgs& args) const {
    ComPtr<ID3D11DomainShader> sh;
    auto entry = args._entry.empty() ? "DSMain" : args._entry;
    auto target = args._target.empty() ? "ds_5_0" : args._target;
    auto blob = compile(filename, entry, target, args._defines.data(), getOptions(args), args._verbose);
    if(blob) {
        throwOnDXError(dx11.device->CreateDomainShader(
            blob->GetBufferPointer(),
            blob->GetBufferSize(),
            nullptr,
            sh.GetAddressOf()
        ));
    }
    return {sh, blob};
}
GeometryShader Shaders::makeGS(const wstring& filename, const ShaderArgs& args) const {
    ComPtr<ID3D11GeometryShader> sh;
    auto entry = args._entry.empty() ? "GSMain" : args._entry;
    auto target = args._target.empty() ? "gs_5_0" : args._target;
    auto blob = compile(filename, entry, target, args._defines.data(), getOptions(args), args._verbose);
    if(blob) {
        throwOnDXError(dx11.device->CreateGeometryShader(
            blob->GetBufferPointer(),
            blob->GetBufferSize(),
            nullptr,
            sh.GetAddressOf()
        ));
    }
    return {sh, blob};
}
PixelShader Shaders::makePS(const wstring& filename, const ShaderArgs& args) const {
    ComPtr<ID3D11PixelShader> sh;
    auto entry = args._entry.empty() ? "PSMain" : args._entry;
    auto target = args._target.empty() ? "ps_5_0" : args._target;
    auto blob = compile(filename, entry, target, args._defines.data(), getOptions(args), args._verbose);
    if(blob) {
        throwOnDXError(dx11.device->CreatePixelShader(
            blob->GetBufferPointer(),
            blob->GetBufferSize(),
            nullptr,
            sh.GetAddressOf()
        ));
    }
    return {sh, blob};
}
ComputeShader Shaders::makeCS(const wstring& filename, const ShaderArgs& args) const {
    ComPtr<ID3D11ComputeShader> sh;
    auto entry   = args._entry.empty() ? "CSMain" : args._entry;
    auto target  = args._target.empty() ? "cs_5_0" : args._target;
    auto blob    = compile(filename, entry, target, args._defines.data(), getOptions(args), args._verbose);
    if(blob) {
        throwOnDXError(dx11.device->CreateComputeShader(
            blob->GetBufferPointer(),
            blob->GetBufferSize(),
            nullptr,
            sh.GetAddressOf()
        ));
    }
    return {sh, blob};
}
uint Shaders::getOptions(const ShaderArgs& args) const {
    uint options = args._options == 0 ? getDefaultOptions() : args._options;
    options |= args._optionsAdd;
    options &= ~args._optionsRemove;
    return options;
}
ComPtr<ID3DBlob> Shaders::compile(const wstring& filename, 
                                  const string& entry, 
                                  const string& target, 
                                  const D3D_SHADER_MACRO* defines,
                                  uint options,
                                  bool verbose) const 
{
	if(!File::exists(filename)) {
		throw std::runtime_error(String::format("Shader file '%s' does not exist", 
                                 WString::toString(filename).c_str()).c_str());
	}
	Log::format("Compiling shader %s", WString::toString(filename).c_str());

    if(options==0) options = getDefaultOptions();

	ComPtr<ID3DBlob> blob;
	ComPtr<ID3DBlob> errors;
	auto hr = D3DCompileFromFile(
		filename.c_str(),
		defines,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		entry.c_str(),
		target.c_str(),
		options,
		0,
		blob.GetAddressOf(),
		errors.GetAddressOf()
	);
	if(FAILED(hr)) {
		string msg = "Shader compilation error: ";
		if(errors) {
			string str = (const char*)errors->GetBufferPointer();
			Log::format("%s", str.c_str());
			msg += str;
		}
		throw std::runtime_error(msg);
	}
    if(verbose) {
        Log::format("\tCompiled successfully using options 0x%x", options);
        for(auto& it : getOptionsAsString(options)) {
            Log::write("\t", it);
        }
    }
	return blob;
}

} /// dx11