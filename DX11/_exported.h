#pragma once

#include "_defines.h"

void throwOnDXError(HRESULT hr, const char* msg = nullptr);

#include "types.h"
#include "swapchain.h"
#include "buffer.h"
#include "sampler.h"
#include "textures.h"
#include "shaders.h"
#include "font.h"
#include "dx11.h"
#include "quad.h"
#include "text.h"
#include "shader_printf.h"

