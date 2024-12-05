#pragma once
#include "Breakpoint.h"

#define TODO(...) BREAK_POINT( \
    std::format("TODO!: {}", __VA_ARGS__))

// Specialize the case where no arguments are passed
#define TODO() BREAK_POINT("TODO!")