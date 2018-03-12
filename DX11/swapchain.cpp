#include "_pch.h"
#include "_exported.h"

namespace dx11 {

using namespace core;

void SwapChain::init() {
	DXGI_SWAP_CHAIN_DESC1 scd = {};
	scd.Width = dx11.params.width;
	scd.Height = dx11.params.height;
	scd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.SampleDesc.Count = 1;
	scd.SampleDesc.Quality = 0;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.BufferCount = 2;	

	/// Better swapEffects are only available for win 8 or 10
	scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	
	auto hr = dx11.factory->CreateSwapChainForHwnd(
		dx11.device.Get(),
		dx11.hwnd,
		&scd,
		nullptr,
		nullptr,
		swapChain.GetAddressOf()
	);
	if(FAILED(hr)) {
		throw std::runtime_error("Could not create swap chain: " + Win::HRESULTToString(hr));
	}

	/// Get the back buffer
	throwOnDXError(swapChain->GetBuffer(0, IID_PPV_ARGS(backBuffer.GetAddressOf())));

	throwOnDXError(dx11.device->CreateRenderTargetView(backBuffer.Get(), nullptr, frameResource.renderTargetView.GetAddressOf()));
 
	dx11.context->OMSetRenderTargets(1, frameResource.renderTargetView.GetAddressOf(), nullptr);
}
void SwapChain::present() {
	HRESULT result;
	if(dx11.params.vsync) {
		/// Lock to screen refresh rate.
		result = swapChain->Present(1, 0);
	} else {
		/// Present as fast as possible.
		result = swapChain->Present(0, 0);
	}
}

} /// dx11
