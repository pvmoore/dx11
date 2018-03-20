#pragma once

namespace dx11 {

enum KeyMod : uint { NONE = 0, CTRL = 1, SHIFT = 2, LEFT_MB = 4, MIDDLE_MB = 8, RIGHT_MB = 16 };
enum MouseClick : uint { PRESS, RELEASE, DBLCLICK };
enum WindowMode : uint { WINDOWED, WINDOWED_FULLSCREEN }; // FULLSCREEN
enum Adapter : uint { HARDWARE, SOFTWARE };

inline std::string toString(Adapter a) { return std::string(a==Adapter::HARDWARE ? "HARDWARE" : "SOFTWARE"); }

inline KeyMod operator|(KeyMod a, KeyMod b) {
	return (KeyMod)((uint)a | (uint)b);
}

class InputEventHandler {
public:
    virtual void key(int vkCode, bool pressed) {}
    virtual void mouseMove(int2 pos, KeyMod mod) {}
    virtual void mouseButton(uint button, int2 pos, KeyMod mod, MouseClick click) {}
    virtual void mouseWheel(int delta, KeyMod mod) {}
    virtual void render(const FrameResource& frame) {}
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
    wstring fontsDirectory   = L"./";
    Adapter adapter = Adapter::HARDWARE;
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
    ComPtr<ID3D11InfoQueue> infoQueue;

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