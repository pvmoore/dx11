#pragma once
///
///	Display textured quads.
///
namespace dx11 {

class Quad {
	struct Info final {
		float2 pos;
		float2 size;
		rgba color;
	};
	struct Vertex final {
		float2 pos;
		rgba color;
		float2 uv;
	}; static_assert(8*4==sizeof(Vertex));
	struct Constants final {
		matrix viewProj;
	}; static_assert(16 * 4 == sizeof(Constants) && sizeof(Constants) % 16 == 0);

	ComPtr<ID3D11InputLayout> inputLayout;
	ComPtr<ID3D11ShaderResourceView> _texture;
	ComPtr<ID3D11SamplerState> _sampler;
	VertexBuffer<Vertex> vertexBuffer;
	ConstantBuffer<Constants> constantBuffer;
	VertexShader vertexShader = {};
	PixelShader pixelShader = {};
	vector<Info> quads;
	rgba _color = rgba(1,1,1,1);
	uint maxVertices;
	bool pipelineChanged = true;
	bool constantsChanged = true;
	bool cameraSet = false;
	bool isInitialised = false;
public:
	Quad& init(DX11& dx11, uint maxQuads) {
		this->maxVertices = maxQuads*6;
		setupPipeline(dx11);
		isInitialised = true;
		return *this;
	}
	Quad& quad(float2 pos, float2 size) {
		quads.push_back({pos, size, _color});
		pipelineChanged = true;
		return *this;
	}
	Quad& clear() {
		quads.clear();
		pipelineChanged = true;
		return *this;
	}
	Quad& camera(Camera& cam) {
		constantBuffer.data.viewProj = cam.VP();
		cameraSet = true;
		constantsChanged = true;
		return *this;
	}
	Quad& color(rgba color) {
		_color = color;
		return *this;
	}
	Quad& sampler(ComPtr<ID3D11SamplerState> sampler) {
		_sampler = sampler;
		return *this;
	}
	Quad& texture(ComPtr<ID3D11ShaderResourceView> texture) {
		_texture = texture;
		return *this;
	}
	void update(const FrameResource& frame) {
		assert(isInitialised && cameraSet);
		if(constantsChanged) {
			updateConstants(frame);
		}
		if(pipelineChanged) {
			updatePipeline(frame);
		}
	}
	void render(const FrameResource& frame) {
		assert(isInitialised && cameraSet);

		auto context = frame.context;
		context->IASetInputLayout(inputLayout.Get());
		context->VSSetShader(vertexShader, nullptr, 0);
		context->PSSetShader(pixelShader, nullptr, 0);

		uint strides = sizeof(Vertex);
		uint offsets = 0;
		context->IASetVertexBuffers(0, 1, vertexBuffer.handle.GetAddressOf(), &strides, &offsets);

		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		context->VSSetConstantBuffers(0, 1, constantBuffer.handle.GetAddressOf());

		if(_texture && _sampler) {
			context->PSSetSamplers(0, 1, _sampler.GetAddressOf());
			context->PSSetShaderResources(0, 1, _texture.GetAddressOf());
		}

		context->Draw((uint)quads.size()*6, 0);

		// Clean up srv
		if(_texture && _sampler) {
			ID3D11ShaderResourceView* nullsrvs[] = {nullptr};
			context->PSSetShaderResources(0, 1, nullsrvs);
		}
	}
private:
	void updateConstants(const FrameResource& frame) {
		constantBuffer.write(frame.context);
		constantsChanged = false;
	}
	void updatePipeline(const FrameResource& frame) {
		/// 0 --- 1
		/// | \   |
		/// |   \ |
		/// 3 --- 2
		vector<Vertex> vertices;
		vertices.reserve(6*quads.size());
		for(auto& it : quads) {
			vertices.push_back({it.pos, it.color, {0.0f, 0.0f}});	// 0
			vertices.push_back({it.pos+float2(it.size.x, 0), it.color, {1.0f, 0.0f}});	// 1
			vertices.push_back({it.pos+it.size, it.color, {1.0f, 1.0f}});	// 2

			vertices.push_back({it.pos, it.color, {0.0f, 0.0f}});	// 0
			vertices.push_back({it.pos+it.size, it.color, {1.0f, 1.0f}});	// 2
			vertices.push_back({it.pos+float2(0, it.size.y), it.color, {0.0f, 1.0f}});	// 3
		}
		vertexBuffer.write(frame.context, vertices.data(), 0, (uint)vertices.size());
		pipelineChanged = false;
	}
	void setupPipeline(DX11& dx11) {
		vertexBuffer.initDynamic(dx11.device, maxVertices);
		constantBuffer.init(dx11.device);

		const auto F32x2 = DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT;
		const auto F32x4 = DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT;

		const D3D11_INPUT_ELEMENT_DESC layout[] = {
			{"POSITION", 0, F32x2, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"COLOR",    0, F32x4, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, F32x2, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};

        ShaderArgs args{};
        vertexShader = dx11.shaders.makeVS(dx11.params.shadersDirectory + L"quad.hlsl", args);
        pixelShader  = dx11.shaders.makePS(dx11.params.shadersDirectory + L"quad.hlsl", args);

		throwOnDXError(dx11.device->CreateInputLayout(
			layout, 3,
			vertexShader.blob->GetBufferPointer(),
			vertexShader.blob->GetBufferSize(),
			inputLayout.GetAddressOf()));
	}
};

} /// dx11