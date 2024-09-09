#pragma once

class Application{
    public:
        Application() {};
        virtual ~Application() {};
        virtual void run() = 0;
};

extern Application* get_client_app();

/*
int main(int argc, char* argv[])
{
    return hpx::local::init(hpx_main, argc, argv);
}*/

