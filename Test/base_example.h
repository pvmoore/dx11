#pragma once

class BaseExample : public InputEventHandler {
protected:
	DX11 dx11;
	InitParams params = {};
public:
	virtual void init(HINSTANCE hInstance, int cmdShow) {
		params.shadersDirectory = L"../Resources/shaders/";
		dx11.init(hInstance, params, this);
		setup();
		dx11.showWindow(cmdShow);
	}
	virtual void shutdown() {};
	virtual void setup() = 0;
	void run() { dx11.run(); shutdown(); }
	virtual void key(int vkCode, bool pressed) {}
	virtual void mouseMove(POINT pos, KeyMod mod) {}
	virtual void mouseButton(int button, POINT pos, KeyMod mod, MouseClick click) {}
	virtual void mouseWheel(int delta, KeyMod mod) {}
};