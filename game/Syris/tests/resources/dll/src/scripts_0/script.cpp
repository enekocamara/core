#include <iostream>

//code from https://stackoverflow.com/questions/15249465/c-dynamically-load-arbitrary-function-from-dll-into-stdfunction
// Declare function prototypes with "extern C" to prevent name mangling.
// Declare functions using __declspec(dllexport) to signify the intent to export.
/*
extern "C" {
    __declspec(dllexport) void __stdcall print_name();
}
*/
#include "Syris/scripts/Export.h"
//#define EXPORT_FUNCTION extern "C" __declspec(dllexport)

EXPORT_FUNCTION void nothing(){
}

EXPORT_FUNCTION void param_val(int i){
}
EXPORT_FUNCTION int return_val(){
    return 420;
}
EXPORT_FUNCTION int add_1(int i){
    return i + 1;
}

EXPORT_FUNCTION void ref_add_1(int& i){
    i++;
}

namespace Dummy{
EXPORT_FUNCTION inline void namespaced_function(int& i){
    i++;
}
}

namespace Sandbox::ecs::Chicken{

EXPORT_FUNCTION inline void namespaced_chick(){

}
}