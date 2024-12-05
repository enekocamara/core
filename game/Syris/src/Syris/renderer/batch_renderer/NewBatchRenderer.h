#pragma once

#include "OpenGLBatchRenderer.hpp"

namespace Syris{
    template <typename... Types>
    inline std::unique_ptr<BatchRenderer> new_batch_renderer(BatchRenderer::CreateInfo &info){
        return std::make_unique<OpenGLBatchRenderer<Types...>>(info);
    }
}