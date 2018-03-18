#pragma once
///
///	Display SDF text.
///
///	Note: Supporting unicode
///		wstring c = String::toWString(u8"€");
///     for(auto it : c) { 
///			// this provides the code point value which can then be used on a sdf (extended fnt file)
///		}
///
namespace dx11 {

class Text {
	struct Vertex final {
		float2 pos;
		float2 uv;	
		rgba color;
		float size;
	}; static_assert(9 * 4 == sizeof(Vertex));
	struct Constants final {
		matrix viewProj;
		rgba dropShadowColour   = rgba{0, 0, 0, 0.75f};
		float2 dropShadowOffset = float2{-0.0025f, 0.0025f};
		float2 _pad;
	}; static_assert(24 * 4 == sizeof(Constants) && sizeof(Constants)%16==0);
	struct TextChunk final {
		string text;
		rgba colour;
		float size;
		int x, y;
	};
	
	ComPtr<ID3D11InputLayout> inputLayout;
	ComPtr<ID3D11SamplerState> sampler;
	ComPtr<ID3D11BlendState> blendState;
	VertexBuffer<Vertex> vertexBuffer;
	ConstantBuffer<Constants> constantBuffer;
	VertexShader vertexShader = {};
	PixelShader pixelShader = {}, dsPixelShader = {};

	float size;
	rgba colour = rgba{1, 1, 1, 1};
	vector<TextChunk> textChunks;
	shared_ptr<Font> font;
	int maxCharacters;
	bool dropShadow;
	bool pipelineChanged = true;
	bool constantsChanged = true;
	bool isInitialised = false, cameraSet = false;
	int numCharacters = 0;
public:
	Text& init(DX11& dx11, shared_ptr<Font> font, bool dropShadow, int maxCharacters) {
		this->font = font;
		this->dropShadow = dropShadow;
		this->maxCharacters = maxCharacters;
		this->size = (float)font->size;

		setupPipeline(dx11);
		isInitialised = true;
		return *this;
	}
	Text& camera(Camera& cam) {
		constantBuffer.data.viewProj = cam.VP();
		cameraSet = true;
		constantsChanged = true;
		return *this;
	}
	Text& appendText(string text, int x = 0, int y = 0) {
		TextChunk chunk;
		chunk.text = text;
		chunk.colour = colour;
		chunk.size = size;
		chunk.x = x;
		chunk.y = y;
		textChunks.push_back(chunk);
		pipelineChanged = true;
		return *this;
	}
	Text& replaceText(int index, string& text) {
		throw std::runtime_error("implement me");
		return *this;
	}
	Text& clear() {
		textChunks.clear();
		pipelineChanged = true;
		return *this;
	}
	Text& setColour(rgba colour) {
		this->colour = colour;
		return *this;
	}
	Text& setSize(float size) {
		this->size = size;
		return *this;
	}
	Text& setDropShadowColour(rgba c) {
		constantBuffer.data.dropShadowColour = c;
		constantsChanged = true;
		return *this;
	}
	Text& setDropShadowOffset(float2 o) {
		constantBuffer.data.dropShadowOffset = o;
		constantsChanged = true;
		return *this;
	}
	void update(const FrameResource& frame) {
		assert(isInitialised && cameraSet);
		if(constantsChanged) updateConstants(frame);
		if(pipelineChanged) updatePipeline(frame);
	}
	void render(const FrameResource& frame) {
		assert(isInitialised && cameraSet);
		if(numCharacters == 0) return;

		auto context = frame.context;

		context->IASetInputLayout(inputLayout.Get());
		context->VSSetShader(vertexShader, nullptr, 0);

		uint strides = sizeof(Vertex);
		uint offsets = 0;
		context->IASetVertexBuffers(0, 1, vertexBuffer.handle.GetAddressOf(), &strides, &offsets);

		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		context->VSSetConstantBuffers(0, 1, constantBuffer.handle.GetAddressOf());

		context->PSSetSamplers(0, 1, sampler.GetAddressOf());
		context->OMSetBlendState(blendState.Get(), nullptr, 0xffffffff);
		context->PSSetShaderResources(0, 1, font->texture.srv.GetAddressOf());

		if(dropShadow) {
			// Draw drop shadow
			context->PSSetConstantBuffers(0, 1, constantBuffer.handle.GetAddressOf());
			context->PSSetShader(dsPixelShader, nullptr, 0);
			context->Draw(numCharacters * 6, 0);
		}
		// Normal
		context->PSSetShader(pixelShader, nullptr, 0);
		context->Draw(numCharacters * 6, 0);

		// Unset our srv
		ID3D11ShaderResourceView* nullsrvs[] = {nullptr};
		context->PSSetShaderResources(0, 1, nullsrvs);
	}
private:
	void updateConstants(const FrameResource& frame) {
		constantBuffer.write(frame.context);
		constantsChanged = false;
	}
	void updatePipeline(const FrameResource& frame) {
		pipelineChanged = false;
		numCharacters = countCharacters();
		if(numCharacters == 0) return;

		vector<Vertex> vertices;
		vertices.resize(numCharacters*6);

		auto v = 0;
		for(auto& c : textChunks) {
			auto maxY = c.size;
			float X = (float)c.x;
			float Y = (float)c.y;

	/*		if(unicodeAware) {
				import std.utf : byChar, byUTF, toUTF16;
				int i = 0;
				foreach(ch; c.text.toUTF16) {
					generateVertex(i++, ch);
				}
		} else  */
			int i = 0;
			for(auto& ch : c.text) {
				auto g      = font->getChar(ch);
				float ratio = (c.size / (float)font->size);

				float x = X + g.xoffset * ratio;
				float y = Y + g.yoffset * ratio;
				float w = g.width * ratio;
				float h = g.height * ratio;

				/// 0 --- 1
				/// | \   |
				/// |   \ |
				/// 3 --- 2
				int j = v * 6;
				vertices[j+0] = {{x,     y}, {g.u,  g.v}, c.colour, c.size};  // 0
				vertices[j+1] = {{x+w,   y}, {g.u2, g.v}, c.colour, c.size};  // 1
				vertices[j+2] = {{x+w, y+h}, {g.u2, g.v2}, c.colour, c.size}; // 2

				vertices[j+3] = {{x,     y}, {g.u,  g.v}, c.colour, c.size};  // 0
				vertices[j+4] = {{x+w, y+h}, {g.u2, g.v2}, c.colour, c.size}; // 2
				vertices[j+5] = {{x,   y+h}, {g.u,  g.v2}, c.colour, c.size}; // 3

				int kerning = 0;
				if(i+1<c.text.size()) {
					kerning = font->getKerning(ch, c.text[i + 1]);
				}

				X += (g.xadvance + kerning) * ratio;
				v++;
			}
		}
		uint numBytes = (uint)vertices.size() * sizeof(Vertex);
		vertexBuffer.write(frame.context, (void*)vertices.data(), 0, numBytes);
	}
	int countCharacters() {
		ulong total = 0;
		for(auto& c : textChunks) {
			//if(unicodeAware) {
			//	import std.utf : count;

			//	total += c.text.count;
			//} else {
				total += c.text.size();
			//}
		}
		assert(total <= maxCharacters);
		return (int)total;
	}
	void setupPipeline(DX11& dx11) {
		vertexBuffer.initDynamic(dx11.device, maxCharacters * 6);
		constantBuffer.init(dx11.device);

		const auto F32x1 = DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT;
		const auto F32x2 = DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT;
		const auto F32x4 = DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT;

		const D3D11_INPUT_ELEMENT_DESC layout[] = {
			{"POSITION", 0, F32x2, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, F32x2, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"COLOR",    0, F32x4, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"SIZE",     0, F32x1, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};

		vertexShader = dx11.shaders.getVS(dx11.params.shadersDirectory + L"text.hlsl");
		pixelShader  = dx11.shaders.getPS(dx11.params.shadersDirectory + L"text.hlsl");
		dsPixelShader = dx11.shaders.getPS(dx11.params.shadersDirectory + L"text.hlsl", "PSMainDropShadow");

		throwOnDXError(dx11.device->CreateInputLayout(
			layout, 4,
			vertexShader.blob->GetBufferPointer(),
			vertexShader.blob->GetBufferSize(),
			inputLayout.GetAddressOf()));

		D3D11_SAMPLER_DESC samplerDesc;
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.MipLODBias = 0.0f;
		samplerDesc.MaxAnisotropy = 1;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		samplerDesc.MinLOD = -FLT_MAX;
		samplerDesc.MaxLOD = FLT_MAX;

		throwOnDXError(dx11.device->CreateSamplerState(&samplerDesc, sampler.GetAddressOf()));

		D3D11_BLEND_DESC blendStateDesc = {};
		blendStateDesc.AlphaToCoverageEnable = FALSE;
		blendStateDesc.IndependentBlendEnable = FALSE;
		blendStateDesc.RenderTarget[0].BlendEnable = TRUE;
		blendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND::D3D11_BLEND_SRC_ALPHA;
		blendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND::D3D11_BLEND_INV_SRC_ALPHA;
		blendStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
		blendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND::D3D11_BLEND_ONE;
		blendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND::D3D11_BLEND_ZERO;
		blendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
		blendStateDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE::D3D11_COLOR_WRITE_ENABLE_ALL;

		throwOnDXError(dx11.device->CreateBlendState(&blendStateDesc, blendState.GetAddressOf()));
	}
};

} /// dx11