cmake_minimum_required(VERSION 3.10)
project(ksetspp LANGUAGES CXX VERSION 0.0.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_compile_definitions(DEBUG)
add_executable(
    main
    src/main.cpp
)
add_library(
    ksets
    src/activationhistory.cpp
    src/k0.cpp
    src/k1.cpp
    src/k2.cpp
    src/k3.cpp
)
target_link_libraries(main ksets)
target_include_directories(ksets PUBLIC ./include)
install(
    DIRECTORY include/ksets
    DESTINATION include
)
