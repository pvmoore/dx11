#pragma once

class ExampleShaderPrintf final : public BaseExample {
    static constexpr int N = 64;
    static constexpr int workgroupSize = 64;

    struct PrintfBufType final {
        uint flags;
        uint counter;
        float f[1024*1024];
    };

    RWStructuredBuffer<PrintfBufType> printfBuffer;
    ComputeShader computeShader;
    
    PrintfBufType scratch;
public:
    void init(HINSTANCE hInstance, int cmdShow) final override {
        params.title = L"DX11 Shader Printf Example";
        params.width = 1000;
        params.height = 600;
        params.windowMode = WindowMode::WINDOWED;
        params.vsync = false;
        BaseExample::init(hInstance, cmdShow);
    }
    void setup() final override {
        Log::format("Application setup");

        computeShader = dx11.shaders.getCS(L"../Resources/shaders/printf.hlsl");

        printfBuffer.init(dx11.device, 1);

        Log::format("Application setup finished");
    }
    void update(const FrameResource& frame) {

    }
    void render(const FrameResource& frame) final override {
        update(frame);

        auto context = frame.context;

        context->CSSetShader(computeShader, nullptr, 0);

        context->CSSetUnorderedAccessViews(0, 1, printfBuffer.uav.GetAddressOf(), nullptr);

        context->Dispatch(N / workgroupSize, 1, 1);

        /// Copy to staging and read the results
       /* context->CopyResource(stagingRead.handle.Get(), out.handle.Get());
        stagingRead.read(context, scratch);
        for(int i = 0; i < N; i++) {
            assert(scratch[i].i == i * 2);
            assert(scratch[i].f == (float)(i * 2) + 10.0f);
        }*/

        /// Unset our shader
        context->CSSetShader(nullptr, nullptr, 0);
    }
};
