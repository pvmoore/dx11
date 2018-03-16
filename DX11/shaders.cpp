#include "_pch.h"
#include "_exported.h"

namespace dx11 {

using namespace core;

static uint getCompilerOptions() {
	uint compileOpts =
		D3DCOMPILE_ENABLE_STRICTNESS |
		D3DCOMPILE_PARTIAL_PRECISION |
		//D3DCOMPILE_AVOID_FLOW_CONTROL |
		//D3DCOMPILE_PREFER_FLOW_CONTROL |
		D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR |	
		D3DCOMPILE_OPTIMIZATION_LEVEL3 |
		D3DCOMPILE_WARNINGS_ARE_ERRORS |
		0;
#ifdef _DEBUG
	compileOpts |= D3DCOMPILE_DEBUG |
		D3DCOMPILE_SKIP_OPTIMIZATION |
		D3DCOMPILE_DEBUG_NAME_FOR_SOURCE |
		0;
#endif
	return compileOpts;
}
static string getTarget(Shaders::Target target, string prefix) {
	string s = prefix + "_5_0";
	//switch(shader.target) {
	//	default: s += "_5_0"; break;
	//}
	return s;
}

VertexShader Shaders::getVS(const wstring& filename, string entry) const {
	ComPtr<ID3D11VertexShader> vs;
	if(entry.empty()) entry = "VSMain";
	auto blob = compile(filename, entry, getTarget(target, "vs"));
	if(blob) {
		throwOnDXError(dx11.device->CreateVertexShader(
			blob->GetBufferPointer(),
			blob->GetBufferSize(),
			nullptr,
			vs.GetAddressOf()
		));
	}
	return {vs, blob};
}
HullShader Shaders::getHS(const wstring& filename, string entry) const {
	ComPtr<ID3D11HullShader> hs;
	if(entry.empty()) entry = "HSMain";
	auto blob = compile(filename, entry, getTarget(target, "ps"));
	if(blob) {
		throwOnDXError(dx11.device->CreateHullShader(
			blob->GetBufferPointer(),
			blob->GetBufferSize(),
			nullptr,
			hs.GetAddressOf()
		));
	}
	return {hs, blob};
}
DomainShader Shaders::getDS(const wstring& filename, string entry) const {
	ComPtr<ID3D11DomainShader> ds;
	if(entry.empty()) entry = "DSMain";
	auto blob = compile(filename, entry, getTarget(target, "ps"));
	if(blob) {
		throwOnDXError(dx11.device->CreateDomainShader(
			blob->GetBufferPointer(),
			blob->GetBufferSize(),
			nullptr,
			ds.GetAddressOf()
		));
	}
	return {ds, blob};
}
GeometryShader Shaders::getGS(const wstring& filename, string entry) const {
	ComPtr<ID3D11GeometryShader> gs;
	if(entry.empty()) entry = "GSMain";
	auto blob = compile(filename, entry, getTarget(target, "ps"));
	if(blob) {
		throwOnDXError(dx11.device->CreateGeometryShader(
			blob->GetBufferPointer(),
			blob->GetBufferSize(),
			nullptr,
			gs.GetAddressOf()
		));
	}
	return {gs, blob};
}
PixelShader Shaders::getPS(const wstring& filename, string entry) const {
	ComPtr<ID3D11PixelShader> ps;
	if(entry.empty()) entry = "PSMain";
	auto blob = compile(filename, entry, getTarget(target, "ps"));
	if(blob) {
		throwOnDXError(dx11.device->CreatePixelShader(
			blob->GetBufferPointer(),
			blob->GetBufferSize(),
			nullptr,
			ps.GetAddressOf()
		));
	}
	return {ps, blob};
}
ComputeShader Shaders::getCS(const wstring& filename, string entry, D3D_SHADER_MACRO* defines) const {
	ComPtr<ID3D11ComputeShader> cs;
	if(entry.empty()) entry = "CSMain";
	auto blob = compile(filename, entry, getTarget(target, "cs"), defines);
	if(blob) {
		throwOnDXError(dx11.device->CreateComputeShader(
			blob->GetBufferPointer(),
			blob->GetBufferSize(),
			nullptr,
			cs.GetAddressOf()
		));
	}
	return {cs, blob};
}
ComPtr<ID3DBlob> Shaders::compile(const wstring& filename, string entry, string target, D3D_SHADER_MACRO* defines) const {
	if(!File::exists(filename)) {
		throw std::runtime_error(String::format("Shader file '%s' does not exist", WString::toString(filename).c_str()).c_str());
	}
	Log::format("Compiling shader %s", WString::toString(filename).c_str());

	ComPtr<ID3DBlob> blob;
	ComPtr<ID3DBlob> errors;
	auto hr = D3DCompileFromFile(
		filename.c_str(),
		defines,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		entry.c_str(),
		target.c_str(),
		getCompilerOptions(),
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
	return blob;
}

} /// dx11