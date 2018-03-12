#include "_pch.h"
#include "_exported.h"

namespace dx11 {

using namespace core;

DX11::DX11() {
	self = this;
}
DX11::~DX11() {
	Log::write("Deleting DX11");
	if(hwnd && hInstance) {
		ChangeDisplaySettings(nullptr, 0);
		DestroyWindow(hwnd);
		UnregisterClass(WINDOW_CLASS.c_str(), hInstance);
	}
}
void DX11::init(HINSTANCE hInstance, 
				InitParams params, 
				InputEventHandler* eventHandler) 
{
	this->hInstance    = hInstance;
	this->params	   = params;
	this->eventHandler = eventHandler;

	createWindow();
	createDevice();
	swapChain.init();
}
void DX11::run() {
	MSG msg;
	float delta = 1;
	auto lastFrameTimestamp = high_resolution_clock::now();
	uint prev5Seconds;
	uint prevSecond = prev5Seconds = (uint)(lastFrameTimestamp.time_since_epoch().count() * 1e-9);

	while(true) {
		if(PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if(msg.message == WM_QUIT) break;
			continue;
		}

		FrameResource& frame = swapChain.prepare();
		frame.device = device;
		frame.context = context;
		frame.number = frameNumber;
		frame.delta = delta;

		/// Let the client render now
		eventHandler->render(frame);
		swapChain.present();

		/// Update timing info
		auto timestamp = high_resolution_clock::now();
		ulong frameNsecs = (timestamp - lastFrameTimestamp).count();
		lastFrameTimestamp = timestamp;

		delta = (float)(double)(frameNsecs * 1e-9);
		frameNumber++;

		double totalSeconds = timestamp.time_since_epoch().count() * 1e-9;
		if((uint)totalSeconds > prevSecond) {
			/// Every second
			prevSecond = (uint)totalSeconds;
			double ms = frameNsecs * 1e-6;
			double fps = 1000.0 / ms;
			Log::format("Frame [%llu] Delta: %.4f, Elapsed: %.3fms, FPS: %.3f",
				frameNumber, delta, ms, fps);
			/// Every 5 seconds
			if((uint)totalSeconds>prev5Seconds+5) {
				prev5Seconds = (uint)totalSeconds;
				DXGI_QUERY_VIDEO_MEMORY_INFO memoryInfo;
				adapter->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &memoryInfo);
				Log::format("\tSystem mem usage .. ?");
				Log::format("\tGPU mem usage ..... %llu MB (of %llu MB budget)",
					memoryInfo.CurrentUsage/(1024*1024),
					memoryInfo.Budget/(1024*1024));
				/// Add fps to window title
				if(params.windowMode==WindowMode::WINDOWED) {
					wstring s = params.title + wstring(L" : ") +
						std::to_wstring((uint)fps) + wstring(L" FPS");
					setTitle(s.c_str());
				}
			}
		}
	}
}
void DX11::createWindow() {
	Log::write("Creating window");
	WNDCLASSEXW wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = windowProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = NULL;
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = WINDOW_CLASS.c_str();
	wcex.hIconSm = NULL;
	wcex.hIcon = NULL;
	RegisterClassExW(&wcex);

#if WINVER >= _WIN32_WINNT_WIN10
	/// Disable auto scaling of the window (Windows 10 only)
	SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
	uint dpi = GetDpiForSystem();
	log("\tDesktop DPI = %u (%u%% scaling)", dpi, (dpi*100)/96);
#else
	/// Manually set DPI Awareness in the manifest
	Log::format("\tWindows version < 10 : Set DPI awareness manually");
#endif
	auto screenWidth = GetSystemMetrics(SM_CXSCREEN);
	auto screenHeight = GetSystemMetrics(SM_CYSCREEN);
	Log::format("\tScreen size is %ux%u", screenWidth, screenHeight);

	DWORD dwExStyle;
	DWORD dwStyle;
	RECT windowRect = {};

	if(params.windowMode==WindowMode::WINDOWED_FULLSCREEN) {
		dwExStyle = WS_EX_APPWINDOW;
		dwStyle = WS_POPUP;
		/// Make a small window initially and resize it later
		windowRect.right = 100;
		windowRect.bottom = 100;
	} else {
		windowRect.left = (screenWidth - params.width) / 2;
		windowRect.top = (screenHeight - params.height) / 4;
		windowRect.right = windowRect.left + params.width;
		windowRect.bottom = windowRect.top + params.height;
		dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
		dwStyle = WS_OVERLAPPEDWINDOW;
		Log::format("\tWindow rect: (%d,%d) - (%d,%d)", windowRect.left, windowRect.top, windowRect.right, windowRect.bottom);
	}

	hwnd = CreateWindowEx(
		dwExStyle,
		WINDOW_CLASS.c_str(),
		params.title.c_str(),
		dwStyle | WS_CLIPSIBLINGS |	WS_CLIPCHILDREN,
		windowRect.left,
		windowRect.top,
		windowRect.right-windowRect.left,
		windowRect.bottom-windowRect.top,
		HWND_DESKTOP,
		nullptr,
		hInstance,
		nullptr);

	if(!hwnd) {
		throw std::runtime_error("Failed to create window");
	}

	if(params.windowMode==WindowMode::WINDOWED_FULLSCREEN) {
		/// Adjust window to fit nearest monitor
		HMONITOR hMonitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
		MONITORINFOEX monitorInfo = {};
		monitorInfo.cbSize = sizeof(MONITORINFOEX);
		GetMonitorInfo(hMonitor, &monitorInfo);

		SetWindowPos(hwnd, HWND_TOPMOST,
					 monitorInfo.rcMonitor.left,
					 monitorInfo.rcMonitor.top,
					 monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left,
					 monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top,
					 SWP_FRAMECHANGED | SWP_NOACTIVATE | SWP_HIDEWINDOW);
		Log::format("\tWindow rect: (%d,%d) - (%d,%d)", monitorInfo.rcMonitor.left, monitorInfo.rcMonitor.top, monitorInfo.rcMonitor.right, monitorInfo.rcMonitor.bottom);
		/// Update these for creating the back buffers later
		params.width = monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left;
		params.height = monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top;
	}
	Log::format("\tCreated %ux%d window '%s'", params.width, params.height, WString::toString(params.title).c_str());
	UpdateWindow(hwnd);
}
void DX11::createDevice() {
	Log::write("Creating DXGI factory");
	uint factoryFlags = 0;
#ifdef _DEBUG
	factoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
#endif
	throwOnDXError(CreateDXGIFactory2(factoryFlags, IID_PPV_ARGS(factory.GetAddressOf())));

	selectAdapter();

	Log::write("Creating device");
	uint creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#ifdef _DEBUG
	creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	const D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_11_0
	};

	D3D_FEATURE_LEVEL level;

	auto hr = D3D11CreateDevice(
		adapter.Get(),
		D3D_DRIVER_TYPE_UNKNOWN,
		nullptr,
		creationFlags,
		featureLevels,
		_countof(featureLevels),
		D3D11_SDK_VERSION,
		device.GetAddressOf(),
		&level,
		context.GetAddressOf()
	);
	if(FAILED(hr)) {
		throw std::runtime_error("Could not initialize Direct3D: " + Win::HRESULTToString(hr));
	}
	if(level==D3D_FEATURE_LEVEL_11_0) {
		Log::write("\tFeature level 11.0 selected");
	}

	D3D11_VIEWPORT vp = {};
	vp.Width    = (float)params.width;
	vp.Height   = (float)params.height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0.0f; // -(float)(params.width/2); //0.0f;
	vp.TopLeftY = 0.0f; // -(float)(params.height/2); //0.0f;
	context->RSSetViewports(1, &vp);

	Log::format("================================================== DX11 ready");
}
void DX11::selectAdapter() {
	Log::write("Enumerating adapters");
	ComPtr<IDXGIAdapter1> adapter1;
	char str[128] = {};
	ulong maxMemory = 0;
	uint selected = 0;
	for(uint i = 0; factory->EnumAdapters1(i, adapter1.ReleaseAndGetAddressOf()) != DXGI_ERROR_NOT_FOUND; i++) {
		Log::format("\tAdapter %d {", i);
		DXGI_ADAPTER_DESC1 desc;
		adapter1->GetDesc1(&desc);
		bool isSoftware = desc.Flags&DXGI_ADAPTER_FLAG_SOFTWARE;
		wcstombs_s(nullptr, str, 128, desc.Description, 128);
		Log::format("\t\tVendorId: %u", desc.VendorId);
		Log::format("\t\tDeviceId: %u", desc.DeviceId);
		Log::format("\t\tSubSysId: %u", desc.SubSysId);
		Log::format("\t\tRevision: %u", desc.Revision);
		Log::format("\t\tDedicatedVideoMemory: %lld", desc.DedicatedVideoMemory);
		Log::format("\t\tDedicatedSystemMemory: %lld", desc.DedicatedSystemMemory);
		Log::format("\t\tSharedSystemMemory   : %lld", desc.SharedSystemMemory);
		Log::format("\t\tDescription: %s", str);
		Log::format("\t\tType: %s", isSoftware ? "SOFTWARE" : "HARDWARE");
		Log::format("\t}");
		if(!isSoftware && desc.DedicatedVideoMemory > maxMemory) {
			maxMemory = desc.DedicatedSystemMemory;
			adapter1.As(&adapter);
			selected = i;
		}
	}
	Log::format("\tSelected adapter %u", selected);
}
/// static 
LRESULT DX11::windowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	const auto getKeyMod = [=]()->KeyMod {
		return ((wParam&MK_CONTROL) ? KeyMod::CTRL : KeyMod::NONE) |
			((wParam&MK_SHIFT) ? KeyMod::SHIFT : KeyMod::NONE);
	};
	const auto handleMouseButton = [=](int button, MouseClick click) {
		if(self->eventHandler) {
			POINT pos = {LOWORD(lParam), HIWORD(lParam)};
			self->eventHandler->mouseButton(button, pos, getKeyMod(), click);
		}
	};
	const auto handleKey = [=](bool pressed) {
		if(self->eventHandler) {
			const bool isRepeat = lParam&0x40000000;
			if(pressed && isRepeat) return;
			self->eventHandler->key((int)wParam, pressed);
		}
	};
	switch(message) {
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_KEYDOWN:
			if(wParam==VK_ESCAPE) PostQuitMessage(0);
			handleKey(true);
			break;
		case WM_KEYUP:
			handleKey(false);
			break;
		case WM_MOUSEMOVE:
			if(self->eventHandler) {
				self->eventHandler->mouseMove({LOWORD(lParam), HIWORD(lParam)}, getKeyMod());
			}
			self->mouseState.pos = {LOWORD(lParam), HIWORD(lParam)};
			break;
		case WM_MOUSEWHEEL:
			if(self->eventHandler) {
				self->eventHandler->mouseWheel((short)HIWORD(wParam), getKeyMod());
			}
			break;
		case WM_LBUTTONDBLCLK:
			handleMouseButton(0, MouseClick::DBLCLICK);
			break;
		case WM_MBUTTONDBLCLK:
			handleMouseButton(1, MouseClick::DBLCLICK);
			break;
		case WM_RBUTTONDBLCLK:
			handleMouseButton(2, MouseClick::DBLCLICK);
			break;
		case WM_LBUTTONDOWN:
			handleMouseButton(0, MouseClick::PRESS);
			break;
		case WM_LBUTTONUP:
			handleMouseButton(0, MouseClick::RELEASE);
			break;
		case WM_MBUTTONDOWN:
			handleMouseButton(1, MouseClick::PRESS);
			break;
		case WM_MBUTTONUP:
			handleMouseButton(1, MouseClick::RELEASE);
			break;
		case WM_RBUTTONDOWN:
			handleMouseButton(2, MouseClick::PRESS);
			break;
		case WM_RBUTTONUP:
			handleMouseButton(2, MouseClick::RELEASE);
			break;
			//case WM_POWERBROADCAST:
		case WM_ACTIVATEAPP:
			if(wParam) {
				/// activate
			} else {
				/// deactivate
			}
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

} /// dx11
