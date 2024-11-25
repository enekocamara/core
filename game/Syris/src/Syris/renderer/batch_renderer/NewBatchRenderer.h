#pragma once

#include "OpenGLBatchRenderer.hpp"

namespace Syris{
    template <typename T>
    inline std::unique_ptr<BatchRenderer> new_batch_renderer(BatchRenderer::CreateInfo &info){
        return std::make_unique<OpenGLBatchRenderer<T>>(info);
    }
}