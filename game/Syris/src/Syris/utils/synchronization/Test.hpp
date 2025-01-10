#pragma once
#include "Syris/utils/containers/MutexGuard.hpp"
#include "Syris/scripts/DynamicLibraryLoader.hpp"

namespace Syris{
    MutexGuard<DynamicLibraryLoader> dll;
}