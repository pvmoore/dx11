#pragma once

class ExampleCompute final : public BaseExample {
	struct BufType final {
		int i;
		float f;
	};
	struct Constants final {
		float value;
		float3 _pad;
	}; static_assert(sizeof(Constants)%16==0);

	StructuredBuffer<BufType> in1, in2;
	RWStructuredBuffer<BufType> out;
	StagingReadBuffer<BufType> stagingRead;
	ConstantBuffer<Constants> constantBuffer;
    ComputeShader computeShader;
	static constexpr int N = 1024;
	static constexpr int workgroupSize = 64;
	BufType scratch[N];
public:
	void init(HINSTANCE hInstance, int cmdShow) final override {
		params.title = L"DX11 Compute Example";
		params.width = 1000;
		params.height = 600;
		params.windowMode = WindowMode::WINDOWED;
		params.vsync = false;
		BaseExample::init(hInstance, cmdShow);
	}
	void setup() final override {
		Log::format("Application setup");

        ShaderArgs args{};
        args.verbose();
        computeShader = dx11.shaders.makeCS(L"../Resources/shaders/compute.hlsl", args);

		BufType* indata1 = new BufType[N];
		BufType* indata2 = new BufType[N];

		for(int i = 0; i < N; i++) {
			indata1[i] = {i, (float)i};
			indata2[i] = {i, (float)i};
		}

		constantBuffer.data.value = 10.0f;
		constantBuffer.init(dx11.device);

		in1.init(dx11.device, N);
		in2.init(dx11.device, N);
		out.init(dx11.device, N);
		stagingRead.init(dx11.device, N);

		in1.write(dx11.context, indata1);
		in2.write(dx11.context, indata2);

		delete[] indata1;
		delete[] indata2;

		Log::format("Application setup finished");
	}
	void update(const FrameResource& frame) {

	}
	void render(const FrameResource& frame) final override {
		update(frame);

		auto context = frame.context;

		context->CSSetShader(computeShader.handle.Get(), nullptr, 0);

		ID3D11ShaderResourceView* views[] = {in1.view.Get(), in2.view.Get()};
		context->CSSetShaderResources(0, 2, views);

		context->CSSetUnorderedAccessViews(0, 1, out.uav.GetAddressOf(), nullptr);

		context->CSSetConstantBuffers(0, 1, constantBuffer.handle.GetAddressOf());
	
		context->Dispatch(N/workgroupSize,1,1);

		/// Copy to staging and read the results
		context->CopyResource(stagingRead.handle.Get(), out.handle.Get());
		stagingRead.read(context, scratch);
		for(int i = 0; i < N; i++) {
			assert(scratch[i].i==i*2);
			assert(scratch[i].f==(float)(i*2)+10.0f);
		}

		context->CSSetShader(nullptr, nullptr, 0);
	}
};
