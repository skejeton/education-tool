#ifndef __APPLE__
#define SOKOL_IMPL

#ifdef _WIN32
# define SOKOL_D3D11
#elif defined(__EMSCRIPTEN__)
# define SOKOL_GLES2
#else
# define SOKOL_GLCORE33
#endif

#else
#define SOKOL_METAL
#endif

#ifdef RUN_TESTS
#define SOKOL_NO_ENTRY
#endif

#include "sokol/sokol_app.h"
#include "sokol/sokol_gfx.h"
#include "sokol/sokol_log.h"
#include "sokol/sokol_glue.h"

#include "imgui/imgui.h"
#define SOKOL_IMGUI_IMPL
#include "sokol/util/sokol_imgui.h"