#pragma once

class Example2D final : public BaseExample {
	struct Vertex final {
		float2 pos;
		float4 color;
		float2 uv;
	}; static_assert(32==sizeof(Vertex));
	struct Constants final {
		float value;
		float3 _pad;
	}; static_assert(sizeof(Constants)%16==0);
	struct Constants2 final {
		matrix world;
		matrix view;
		matrix proj;
	}; static_assert(sizeof(Constants2)%16==0);
	VertexBuffer<Vertex> vertexBuffer;
	ConstantBuffer<Constants> constantBuffer;
	ConstantBuffer<Constants2> constantBuffer2;
	VertexShader vertexShader = {};
	PixelShader pixelShader = {};
	ComPtr<ID3D11InputLayout> inputLayout;

	Texture2D texture0;
	ComPtr<ID3D11SamplerState> sampler1;
	ComPtr<ID3D11RasterizerState> rasterizerState;

	Camera2D camera2d;

	Quad quad1;
    Text text;
	int mouseScroll = 0;
public:
	void init(HINSTANCE hInstance, int cmdShow) final override {
		params.title = L"DX11 2D Example";
		params.width = 1200;
		params.height = 800;
		params.windowMode = WindowMode::WINDOWED;
		params.vsync = false;
		BaseExample::init(hInstance, cmdShow);
	}
	void setup() final override {
		Log::format("Application setup");

		camera2d.init(dx11.windowSize());
		Log::format("camera2d: %s", camera2d.toString().c_str());

		setupPipeline();

		quad1.init(dx11, 10)
			.camera(camera2d)
			.color({1, 1, 1, 1})
			.sampler(sampler1)
			.texture(texture0.srv)
			.quad({450,250}, {100,100})
			.quad({560,250}, {150,150});

        text.init(dx11, dx11.fonts.get(L"segoe-ui-black"), true, 100)
            .camera(camera2d)
            .setSize(32)
            .appendText("I am some text 1234567890", 320, 2);

		Log::format("Application setup finished");
	}
	void mouseWheel(int delta, KeyMod mod) final override {
		mouseScroll = delta;
	}
	void update(const FrameResource& frame) {
		ulong number = frame.number/32;
		float dir = (float)((number/256)&1);
		float f = (number % 256) / 255.0f;

		constantBuffer.data.value = abs(dir-f);
		constantBuffer.write(frame.context);

		//constants2.world;
		//constantBuffer2.write(context, &constants2);

		bool cameraMoved = false;
		if(mouseScroll>0) {
			camera2d.zoomIn(0.05);
			cameraMoved = true;
		} else if(mouseScroll<0) {
			camera2d.zoomOut(0.05);
			cameraMoved = true;
		}
		mouseScroll = 0;
		if(cameraMoved) {
			quad1.camera(camera2d);
		}
		quad1.update(frame);
        text.update(frame);
	}
	void render(const FrameResource& frame) final override {
		auto context = frame.context;
		update(frame);
		context->OMSetRenderTargets(1, frame.renderTargetView.GetAddressOf(), nullptr);

		float clearColor[] = {0.2f, 0.0f, 0.2f, 0.0f};
		context->ClearRenderTargetView(frame.renderTargetView.Get(), clearColor);

		/// triangle pipeline
		context->IASetInputLayout(inputLayout.Get());
		context->VSSetShader(vertexShader, nullptr, 0);
		context->PSSetShader(pixelShader, nullptr, 0);

		uint strides = sizeof(Vertex);
		uint offsets = 0;
		context->IASetVertexBuffers(0, 1, vertexBuffer.handle.GetAddressOf(), &strides, &offsets);

		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		ID3D11Buffer* bufs[] = {constantBuffer.handle.Get(), constantBuffer2.handle.Get()};
		context->VSSetConstantBuffers(0, 2, bufs);

		context->PSSetShaderResources(0, 1, texture0.srv.GetAddressOf());

		/// draw 6 vertices, starting from vertex 0
		context->Draw(6, 0);

		quad1.render(frame);
        text.render(frame);
	}
private:
	void setupPipeline() {
		/// 0 --- 1
		/// | \   |
		/// |   \ |
		/// 3 --- 2
		const Vertex vertices[] = {
			{{0.0f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}}, // 0
			{{1.0f, 0.0f}, {0.0f, 1.0f, 0.0f, 1.0f}, {1.0f, 0.0f}}, // 1	
			{{1.0f, 1.0f}, {0.0f, 0.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}, // 2

			{{0.0f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}}, // 0
			{{1.0f, 1.0f}, {0.0f, 0.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}, // 2
			{{0.0f, 1.0f}, {0.0f, 1.0f, 0.0f, 1.0f}, {0.0f, 1.0f}}  // 3
		};
		const uint vertexBufferSize = sizeof(vertices);
	
		vertexBuffer.initImmutable(dx11.device, _countof(vertices), vertices);
		
		constantBuffer.data.value = 0;
		constantBuffer.init(dx11.device);

		constantBuffer2.data.world = 
			//matrix::rotateZ(toRadians(45)) *
			matrix::translate(float3{10, 10, 0}) *
			matrix::scale(float3{300, 300, 0});
		
		constantBuffer2.data.view = camera2d.V();
		constantBuffer2.data.proj = camera2d.P();
		constantBuffer2.init(dx11.device);

		const D3D11_INPUT_ELEMENT_DESC layout[] = {
			{"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 	    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};

        ShaderArgs args{};
        vertexShader = dx11.shaders.makeVS(L"../Resources/shaders/triangle.hlsl", args);
		pixelShader  = dx11.shaders.makePS(L"../Resources/shaders/triangle.hlsl", args);

		throwOnDXError(dx11.device->CreateInputLayout(
			layout, 3, 
			vertexShader.blob->GetBufferPointer(), 
			vertexShader.blob->GetBufferSize(), 
			inputLayout.GetAddressOf()));

		texture0 = dx11.textures.load(L"../Resources/images/birds.dds");

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

		/// Create and set our sampler and never change it
		throwOnDXError(dx11.device->CreateSamplerState(&samplerDesc, sampler1.GetAddressOf()));
		dx11.context->PSSetSamplers(0, 1, sampler1.GetAddressOf());

		/// Create the default rasteriser state
		D3D11_RASTERIZER_DESC drd{};
		drd.FillMode = D3D11_FILL_SOLID;
		drd.CullMode = D3D11_CULL_BACK;
		drd.FrontCounterClockwise = FALSE;
		drd.DepthClipEnable = FALSE;
		drd.ScissorEnable = FALSE;
		drd.MultisampleEnable = FALSE;
		drd.AntialiasedLineEnable = FALSE;
		throwOnDXError(dx11.device->CreateRasterizerState(&drd, rasterizerState.GetAddressOf()));

		dx11.context->RSSetState(rasterizerState.Get());
	}
};
