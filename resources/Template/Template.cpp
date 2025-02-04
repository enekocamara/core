#include "%PROJECT_NAME_FIRST_UPPER%App.hpp"
#include <iostream>
namespace %PROJECT_NAME%{
    %PROJECT_NAME_FIRST_UPPER%App::%PROJECT_NAME_FIRST_UPPER%App(){
        std::cout << "%PROJECT_NAME% app created\n";
    }
    %PROJECT_NAME_FIRST_UPPER%App::~%PROJECT_NAME_FIRST_UPPER%App(){
        std::cout << "%PROJECT_NAME% app destroyed\n";
    }

    void %PROJECT_NAME_FIRST_UPPER%App::run(){
        std::cout << "running...\n";
    }
}

std::unique_ptr<Syris::Application> get_client_app(Syris::AppInit& app_init){
    return std::make_unique<%PROJECT_NAME%::%PROJECT_NAME_FIRST_UPPER%App>();
}
