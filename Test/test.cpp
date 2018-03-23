#include "_pch.h"

using namespace core;
using namespace maths;
#include "../DX11/_exported.h"

using namespace dx11;
#include "_internal.h"

int APIENTRY wWinMain(HINSTANCE hInstance,
					  HINSTANCE hPrevInstance,
					  LPWSTR    lpCmdLine,
					  int       nCmdShow) 
{
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_CHECK_ALWAYS_DF);
#endif

#define TEST 4

#if TEST==1
	Example2D app;
#elif TEST==2
	Example3D app;
#elif TEST==3
	ExampleCompute app;
#elif TEST==4
	ExampleComputeToTexture app;
#elif TEST==5
    ExampleShaderPrintf app;
#endif
	try{
		app.init(hInstance, nCmdShow);
		app.run();
	}catch(std::exception& e) {
		wstring msg = WString::toWString(e.what());
		MessageBox(nullptr, msg.c_str(), L"Error", MB_OK | MB_ICONEXCLAMATION);
		return -1;
	}
	return 0;
}