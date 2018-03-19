#pragma once

namespace dx11 {

struct FrameResource final {
	ulong number;			/// frame number
    ulong nsecs;            /// Total nsecs since run() was called
	float delta;			/// multiply by this to meet per second rate (1fps = 1, 2fps = 0.5, 0.5fps=2)
	ComPtr<ID3D11RenderTargetView> renderTargetView;
	ComPtr<ID3D11Device> device;
	ComPtr<ID3D11DeviceContext> context;

    ulong secondsSinceStart() const { return nsecs / 1'000'000'000; }
};

class SwapChain final {
	class DX11& dx11;
	ComPtr<IDXGISwapChain1> swapChain;
	ComPtr<ID3D11Texture2D> backBuffer;
	FrameResource frameResource;
public:
	SwapChain(DX11& dx11) : dx11(dx11) {}
	void init();
	FrameResource& prepare() { return frameResource; }
	void present();
};

} /// dx11