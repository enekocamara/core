#pragma once
#include "Syris/statistics/Statistics.hpp"
#include "Syris/app/Application.hpp"

extern std::unique_ptr<Syris::Application> get_client_app(Syris::AppInit& app_init);
