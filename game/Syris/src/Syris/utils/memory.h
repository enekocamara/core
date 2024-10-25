#pragma once

#include <span>
#include <cstddef>
namespace Syris{
    //non owning memory only
    struct MemSpan{
        uint64_t size;
        void *data;
    };
}