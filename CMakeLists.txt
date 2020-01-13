cmake_minimum_required(VERSION 3.5)

project(mpvi2-driver LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(test1
  main.cpp
  mpvi2.cpp
  serial_port.cpp
)

# target_link_libraries(TestQt PRIVATE Qt5::Widgets)
