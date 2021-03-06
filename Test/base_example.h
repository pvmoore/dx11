#pragma once

class BaseExample : public InputEventHandler {
protected:
	DX11 dx11;
	InitParams params = {};
public:
	virtual void init(HINSTANCE hInstance, int cmdShow) {
		params.shadersDirectory = L"../Resources/shaders/";
        params.fontsDirectory   = L"../Resources/fonts/";
		dx11.init(hInstance, params, this);
		setup();
		dx11.showWindow(cmdShow);
	}
	virtual void shutdown() {};
	virtual void setup() = 0;
	void run() { dx11.run(); shutdown(); }
};