#pragma once

class ExampleComputeToTexture final : public BaseExample {
	static constexpr uint2 TEXSIZE = {512, 512};
	static constexpr int WG_X	   = 8;
	static constexpr int WG_Y	   = 8;

	Sampler sampler;
	RWTexture2D targetTexture;
	StructuredBuffer<float> readBuffer;
	ComputeShader computeShader;
	Quad quad;
	Camera2D camera;
public:
	void init(HINSTANCE hInstance, int cmdShow) final override {
		params.title = L"DX11 Compute To Texture Example";
		params.width = 1000;
		params.height = 600;
		params.windowMode = WindowMode::WINDOWED;
		params.vsync = false;
		BaseExample::init(hInstance, cmdShow);
	}
	void setup() final override {
		Log::format("Application setup");

		camera.init(dx11.windowSize());

		vector<D3D_SHADER_MACRO> defines;
		defines.push_back({"WG_X", "8"});
		defines.push_back({"WG_Y", "8"});
		defines.push_back({});
		computeShader = dx11.shaders.getCS(L"../Resources/shaders/compute_to_texture.hlsl", "CSMain", defines.data());

		targetTexture.init(dx11.device, TEXSIZE, DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM);

		sampler.init(dx11.device);

		quad.init(dx11, 1)
			.camera(camera)
			.quad({10,10}, TEXSIZE)
			.sampler(sampler.sampler)
			.texture(targetTexture.srv);

		const int N = TEXSIZE.x*TEXSIZE.y;
		float* indata1 = new float[N];

		for(int i = 0; i < N; i++) {
			indata1[i] = (float)(i%256) / 255.0f;
		}

		readBuffer.init(dx11.device, N);
		readBuffer.write(dx11.context, indata1);

		delete[] indata1;

		Log::format("Application setup finished");
	}
	void update(const FrameResource& frame) {
		quad.update(frame);
	}
	void render(const FrameResource& frame) final override {
		update(frame);

		auto context = frame.context;

		context->CSSetShader(computeShader, nullptr, 0);

		ID3D11ShaderResourceView* srvs[] = {readBuffer.view.Get()};
		context->CSSetShaderResources(0, 1, srvs);

		ID3D11UnorderedAccessView* uavs[] = {targetTexture.uav.Get()};
		context->CSSetUnorderedAccessViews(0, 1, uavs, nullptr);

		context->Dispatch(TEXSIZE.x / WG_X, TEXSIZE.y / WG_Y, 1);

		// Unbind the uav before using it as an srv in the quad rendering
		ID3D11UnorderedAccessView* nulluavs[] = {nullptr};
		context->CSSetUnorderedAccessViews(0, 1, nulluavs, nullptr);
		
		// Render the texture to the screen
		quad.render(frame);
	}
};

