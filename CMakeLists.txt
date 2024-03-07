cmake_minimum_required(VERSION 3.10)
project(ksetspp LANGUAGES CXX VERSION 0.0.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_compile_definitions(DEBUG)
add_compile_options(-g)
add_executable(
    main
    src/main.cpp
)
add_library(
    ksets
    src/ksets/activationhistory.cpp
    src/ksets/k0.cpp
    src/ksets/k1.cpp
    src/ksets/k2.cpp
    src/ksets/k2layer.cpp
    src/ksets/k3.cpp
    src/ksets/k0collection.cpp
)
target_link_libraries(main ksets)
target_include_directories(ksets PUBLIC ./include)

include(GNUInstallDirs)
install(
    DIRECTORY include/ksets DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
install(
    TARGETS ksets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)