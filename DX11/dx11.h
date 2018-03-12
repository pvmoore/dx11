#pragma once

namespace dx11 {

enum KeyMod : uint { NONE = 0, CTRL = 1, SHIFT = 2 };
enum MouseClick : uint { PRESS, RELEASE, DBLCLICK };
enum WindowMode : uint { WINDOWED, WINDOWED_FULLSCREEN }; // FULLSCREEN
inline KeyMod operator|(KeyMod a, KeyMod b) {
	return (KeyMod)((uint)a | (uint)b);
}
class InputEventHandler {
public:
	virtual void key(int vkCode, bool pressed) = 0;
	virtual void mouseMove(POINT pos, KeyMod mod) = 0;
	virtual void mouseButton(int button, POINT pos, KeyMod mod, MouseClick click) = 0;
	virtual void mouseWheel(int delta, KeyMod mod) = 0;
	virtual void render(const FrameResource& frame) = 0;
};
struct MouseState final {
	int2 pos;
};
struct InitParams final {
	uint width = 400;
	uint height = 400;
	WindowMode windowMode = WindowMode::WINDOWED;
	bool vsync = false;
	wstring title = L"No title";
	wstring shadersDirectory = L"./";
};
//========================================================================================
class DX11 final {
	static DX11* self;
	const wstring WINDOW_CLASS = L"DX11_WINDOW_CLASS";
public:
	HINSTANCE hInstance = nullptr;
	HWND hwnd = nullptr;
	InitParams params = {};
	MouseState mouseState = {};
	InputEventHandler* eventHandler;
	ulong frameNumber = 0;
	ComPtr<IDXGIFactory2> factory;
	ComPtr<IDXGIAdapter3> adapter;
	ComPtr<ID3D11Device> device;
	ComPtr<ID3D11DeviceContext> context;
	class Shaders shaders{*this};
	class Textures textures{*this};
	class Fonts fonts{*this};
	class SwapChain swapChain{*this};

	DX11();
	~DX11();
	inline uint2 windowSize() const { return {params.width, params.height}; }

	void init(HINSTANCE hInstance, InitParams params, InputEventHandler* eventHandler);
	void run();
	void setTitle(wstring title) {
		SetWindowText(hwnd, title.c_str());
	}
	void setIcon(wstring filename) {
		HANDLE icon = LoadImage(hInstance, filename.c_str(), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE);
		if(icon) {
			SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)icon);
			SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)icon);
		}
	}
	void showWindow(int cmdShow) const {
		ShowWindow(hwnd, cmdShow);
		SetForegroundWindow(hwnd);
		SetFocus(hwnd);
	}
	void showCursor(bool show) const {
		ShowCursor(show);
	}
	inline int2 mousePos() const {
		return mouseState.pos;
	}
private:
	void selectAdapter();
	void createWindow();
	void createDevice();
	static LRESULT CALLBACK windowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

} /// dx11