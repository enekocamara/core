#pragma once

#include <glm/glm.hpp>
#include "Syris/include/OpenGL.h"
#include "Syris/renderer/FrameBuffer.hpp"
#include "Syris/renderer/ViewPort.hpp"
#include <stack>
#include "Syris/jobs/ThreadPool.hpp"

namespace Syris{
    class Renderer{
    public:
        static void push_framebuffer(FrameBuffer* framebuffer);
        static FrameBuffer* pop_framebuffer();
        static FrameBuffer* get_current_framebuffer();
        static void push_viewport(glm::uvec2 viewport);
        static ViewPort pop_viewport();
        static ViewPort get_current_viewport();
        Renderer(const Renderer &ref) = delete;
    private:
        Renderer(){};
        static Renderer &get_renderer();
        std::stack<FrameBuffer*> m_framebuffers;
        std::stack<ViewPort> m_viewports;
    };
}