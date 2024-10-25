#include "Core/Core.hpp"
#include <time.h>
#include <hpx/iostream.hpp>
#include <hpx/hpx_start.hpp>
#include <hpx/threading/thread.hpp>

std::atomic<bool> shutdown_flag{false};
constexpr bool HPX = true;
int hpx_main(int argc, char* argv[])
{ 
    // ...Execute other code here...
    Syris::Statistics statistics;
    AppInit init{
        .statistics = statistics
    };
    Application* client_app = get_client_app(init);
    client_app->run();
    delete client_app;
    shutdown_flag.store(true);
    // Wait for hpx::finalize being called.
    return hpx::local::finalize();
}

int main(int argc, char* argv[])
{
    
    if constexpr (!HPX){
        Syris::Statistics statistics;
        AppInit init{
            .statistics = statistics};
        Application* client_app = get_client_app(init);
        client_app->run();
        delete client_app;
    }else{
    // Initialize HPX, run hpx_main.
        hpx::start(argc, argv);
        return hpx::stop();
    }
}
/*void hpx_main() {
}*/
