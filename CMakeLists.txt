cmake_minimum_required(VERSION 3.5)

project(mpvi2-driver LANGUAGES CXX)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

include(libftdi)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(PkgConfig REQUIRED)
find_package(Threads REQUIRED)
find_package(GTest REQUIRED)

find_package(Qt5 COMPONENTS Widgets)

pkg_check_modules(GMOCK REQUIRED gmock)
pkg_check_modules(GMOCK_MAIN REQUIRED gmock_main)

include_directories(
${PROJECT_SOURCE_DIR}/src
${PROJECT_SOURCE_DIR}/lib
)

add_subdirectory(lib)
add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(tools)
