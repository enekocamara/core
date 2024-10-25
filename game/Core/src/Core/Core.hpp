#pragma once
#include "Syris/statistics/Statistics.hpp"

class Application{
    public:
        Application() {};
        virtual ~Application() {};
        virtual void run() = 0;
};

struct AppInit{
    Syris::Statistics& statistics;
};

extern Application* get_client_app(AppInit& app_init);

/*
int main(int argc, char* argv[])
{
    return hpx::local::init(hpx_main, argc, argv);
}*/

