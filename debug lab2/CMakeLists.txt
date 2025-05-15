cmake_minimum_required(VERSION 3.16)
project("SmartServing" C CXX)

option(ENABLE_SANITIZERS "Enable address and undefined santizers" ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED true)

if (ENABLE_SANITIZERS)
    add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
    link_libraries(-fsanitize=address,undefined)
endif()

add_subdirectory(libs)
add_subdirectory(src)