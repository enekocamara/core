#pragma once
#include "Syris/statistics/Statistics.hpp"

namespace Syris{
class Application{
    public:
        Application() {};
        virtual ~Application() {};
        virtual void run() = 0;
};

struct AppInit{
    Syris::Statistics& statistics;
};
}