#pragma once

//imgui include headerfile for simplicity

#include <imgui.h>
#include "Syris/platform/OpenGl/imguiBackends/imgui_impl_glfw.h"
#include "Syris/platform/OpenGl/imguiBackends/imgui_impl_opengl3.h"
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif