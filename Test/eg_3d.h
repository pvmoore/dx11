#pragma once

class Example3D final : public BaseExample {
	Cube cube;
	Camera2D camera2d;
	Camera3D camera3d;
	ComPtr<ID3D11RasterizerState> rasterizerState;
	int2 oldMousePos = {};
	int2 mousePos = {};
	int mouseScroll = 0;
	bool keyLeft = false, keyRight = false, keyUp = false, keyDown = false, keyA = false, keyZ = false;
	Text text;
	float3 angle = {};
public:
	void init(HINSTANCE hInstance, int cmdShow) final override {
		params.title = L"DX11 3D Example";
		params.width = 1000;
		params.height = 600;
		params.windowMode = WindowMode::WINDOWED;
		params.vsync = false;
		BaseExample::init(hInstance, cmdShow);
	}
	void setup() final override {
		Log::format("Application setup");
		camera3d.init(dx11.windowSize(), {0, 0, 100}, {0, 1, 0}, {0, 0, 0});
		camera3d.fovNearFar(maths::toRadians(60), 10, 100000);
		Log::format("camera: %s", camera3d.toString().c_str());

		camera2d.init(dx11.windowSize());

		text.init(dx11, dx11.fonts.get(L"arial"), true, 256)
			.camera(camera2d)
			.setSize(128)
			.setColour({0.498039246f, 1.000000000f, 0.831372619f, 1.000000000f})
			.appendText("Hello there!", 170, 100);

		cube.init(dx11)
			.camera(camera3d)
			.scale(50)
			.move({0,0,0});

		/// Create the default rasteriser state
		D3D11_RASTERIZER_DESC drd = {};
		drd.FillMode = D3D11_FILL_SOLID;
		drd.CullMode = D3D11_CULL_BACK;
		drd.FrontCounterClockwise = FALSE;
		drd.DepthClipEnable = FALSE;
		drd.ScissorEnable = FALSE;
		drd.MultisampleEnable = FALSE;
		drd.AntialiasedLineEnable = FALSE;
		throwOnDXError(dx11.device->CreateRasterizerState(&drd, rasterizerState.GetAddressOf()));

		dx11.context->RSSetState(rasterizerState.Get());

		Log::format("Application setup finished");
	}
	void mouseMove(const int2 pos, const MouseDrag drag, KeyMod mod) final override {
		mousePos = pos;
	}
	void mouseWheel(int delta, KeyMod mod) final override {
		mouseScroll = delta;
	}
	void key(int vkCode, bool pressed) final override {
		keyLeft  = vkCode==VK_LEFT && pressed;
		keyRight = vkCode==VK_RIGHT && pressed;
		keyUp    = vkCode==VK_UP && pressed;
		keyDown  = vkCode==VK_DOWN && pressed;
		keyA     = vkCode=='A' && pressed;
		keyZ     = vkCode=='Z' && pressed;
	}
	void update(const FrameResource& frame) {
		bool cameraMoved = false;
		if(frame.number==0) mousePos = oldMousePos = dx11.mousePos();
		int2 mouseMovement = mousePos-oldMousePos;

		//if(mouseScroll>0) {
		//	camera3d.moveForward(frame.delta * 200);
		//	cameraMoved = true;
		//} else if(mouseScroll<0) {
		//	camera3d.moveForward(frame.delta * -200);
		//	cameraMoved = true;
		//}
		//mouseScroll = 0;

		if(keyA) {
			camera3d.moveForward(frame.delta * 100);
			cameraMoved = true;
		} else if(keyZ) {
			camera3d.moveForward(frame.delta * -100);
			cameraMoved = true;
		} else if(keyLeft) {
			camera3d.yaw(frame.delta * -1.0f);
			cameraMoved = true;
		} else if(keyRight) {
			camera3d.yaw(frame.delta * 1.0f);
			cameraMoved = true;
		} else if(keyUp) {
			camera3d.pitch(frame.delta * 1.0f);
			cameraMoved = true;
		} else if(keyDown) {
			camera3d.pitch(frame.delta * -1.0f);
			cameraMoved = true;
		}
		
		if(cameraMoved) {
			cube.camera(camera3d);
			Log::format("camera: %s -> %s", camera3d.position.toString().c_str(), camera3d.forward.toString().c_str());
		}
		angle += float3{1.0f, 0.5f, 0.1f}*50*frame.delta;
		cube.rotate(angle);
		cube.update(frame);
		text.update(frame);
	}
	void render(const FrameResource& frame) final override {
		auto context = frame.context;
		update(frame);

		context->OMSetRenderTargets(1, frame.renderTargetView.GetAddressOf(), nullptr);

		float clearColor[] = {0.2f, 0.0f, 0.2f, 0.0f};
		context->ClearRenderTargetView(frame.renderTargetView.Get(), clearColor);

		context->OMSetBlendState(nullptr, nullptr, 0xffffffff);

		cube.render(frame);
		text.render(frame);
	}
};
