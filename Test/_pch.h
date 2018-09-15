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
/// Windows Header Files:
#include <windows.h>

/// C RunTime Header Files
#include <cstdlib>
#include <stdio.h>
#include <cstdarg>
#include <cstring>
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

/// Core
#include "../../Core/Core/core.h"
#ifdef _DEBUG
#pragma comment(lib, "../../Core/x64/Debug/Core.lib")
#else 
#pragma comment(lib, "../../Core/x64/Release/Core.lib")
#endif

/// Maths
#include "../../Maths/Maths/maths.h"
#ifdef _DEBUG
#pragma comment(lib, "../../Maths/x64/Debug/Maths.lib")
#else 
#pragma comment(lib, "../../Maths/x64/Release/Maths.lib")
#endif