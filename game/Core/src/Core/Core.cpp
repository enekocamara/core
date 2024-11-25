#include "Core/Core.hpp"
#include <time.h>
#include <hpx/iostream.hpp>
#include <hpx/hpx_start.hpp>
#include <hpx/threading/thread.hpp>
#include <gtest/gtest.h>
constexpr bool HPX = false;
int hpx_main(int argc, char* argv[])
{ 
    Syris::Statistics statistics;
    AppInit init{
        .statistics = statistics
    };
    std::unique_ptr<Application> client_app = get_client_app(init);
    //try {
        client_app->run();
   // }
   // catch (std::exception& e) {
   //     std::cerr << "Failed to run app: " << e.what() << '\n';
   // }
    return hpx::local::finalize();
}

void no_hpx(){
    Syris::Statistics statistics;
    AppInit init{
        .statistics = statistics
    };
    std::unique_ptr<Application> client_app = get_client_app(init);
    client_app->run();
}

int main(int argc, char* argv[])
{
    if constexpr (!HPX){
        no_hpx();
    }else{
        hpx::start(argc, argv);
        return hpx::stop();
    }
}
