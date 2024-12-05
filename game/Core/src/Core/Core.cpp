#include "Core/Core.hpp"
#include <time.h>

#include <gtest/gtest.h>

int main(int argc, char* argv[])
{
    Syris::Statistics statistics;
    Syris::AppInit init{
        .statistics = statistics
    };
    std::unique_ptr<Syris::Application> client_app = get_client_app(init);
    client_app->run();
    return 0;
}
