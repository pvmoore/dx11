#pragma once

class ExampleShaderPrintf final : public BaseExample {
    static constexpr int N = 64;
    static constexpr int workgroupSize = 64;

    ShaderPrintf shaderPrintf;
    ComputeShader computeShader;
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

        computeShader = dx11.shaders.makeCS(L"../Resources/shaders/printf.hlsl", {});

        shaderPrintf.init(dx11.device, 7, true)
                    .setFloatFormat("%.3f");

        Log::format("Application setup finished");
    }
    void update(const FrameResource& frame) {

    }
    void render(const FrameResource& frame) final override {
        update(frame);

        auto context = frame.context;

        context->CSSetShader(computeShader, nullptr, 0);

        shaderPrintf.before(frame);
        context->Dispatch(N / workgroupSize, 1, 1);
        shaderPrintf.after(frame);

        if(shaderPrintf.hasOutput()) {
            Log::write("##################### Printf output: \n", 
                       shaderPrintf.getOutput(), 
                       "#####################");
        }

        /// Unset our shader
        context->CSSetShader(nullptr, nullptr, 0);
    }
};
