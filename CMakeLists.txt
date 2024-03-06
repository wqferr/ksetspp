cmake_minimum_required(VERSION 3.10)
project(ksetspp LANGUAGES CXX VERSION 0.0.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS -Iinclude)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_compile_options(-DDEBUG)
add_executable(
    main
    src/main.cpp
)
add_library(
    ksets
    SHARED
    src/ksets/activationhistory.cpp
    src/ksets/k0.cpp
    src/ksets/k1.cpp
    src/ksets/k2.cpp
    src/ksets/k3.cpp
)
target_link_libraries(main ksets)
target_include_directories(ksets PUBLIC ./include)
