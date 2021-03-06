#pragma once

#ifndef _ALLOW_RTCc_IN_STL
#define _ALLOW_RTCc_IN_STL
#endif

/// Target Windows 7 and above          
#define WINVER		 _WIN32_WINNT_WIN7
#define _WIN32_WINNT _WIN32_WINNT_WIN7
#include <SDKDDKVer.h>

#define NOMINMAX	
#define NOMENUS
#define NODRAWTEXT
#define NOCTLMGR
#define NOCLIPBOARD
#define NODRAWTEXT
#define NOTEXTMETRIC
//#define NOGDI
#define NOBITMAP
#define NOMCX	
#define NOSERVICE	
#define NOHELP	
#define WIN32_LEAN_AND_MEAN 
#include <windows.h>

/// C RunTime Header Files
#include <cstdlib>
#include <cstdio>
#include <cassert>

/// DirectX stuff
#include <d3d11.h>	
#include <dxgi1_6.h>
#include <D3Dcompiler.h>
#include <wrl.h>

#ifdef _DEBUG
#include <dxgidebug.h>
#endif

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

/// std namespace files
#include <vector>
#include <chrono>
#include <string>
#include <unordered_map>
#include <exception>
#include <memory>
#include <random>

using std::shared_ptr;
using std::unique_ptr;
using std::unordered_map;
using std::string;
using std::wstring;
using std::vector;
using std::chrono::high_resolution_clock;

using Microsoft::WRL::ComPtr;

#include "../External/DDSTextureLoader.h"
#include "../External/stb_image.h"
#include "../../Core/Core/core.h"
#include "../../Maths/Maths/maths.h"

#include "_defines.h"
