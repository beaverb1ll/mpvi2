cmake_minimum_required(VERSION 3.5)

project(mpvi2-driver LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(test1
  main.cpp
  mpvi2.cpp
  ftdi_serial_port.cpp
)

target_link_libraries(test1 PRIVATE /home/user/Downloads/libftd2xx-x86_64-1.4.8/release/build/libftd2xx.so.1.4.8)
