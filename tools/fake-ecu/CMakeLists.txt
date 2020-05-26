
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)


add_library(fake-ecu-libs
  ecu_obd2.cpp
)
add_executable(fake-ecu
  main.cpp
)

add_executable(fake-ecu-gui
  main-gui.cpp
  mainwindow.cpp
)

target_link_libraries(fake-ecu PRIVATE pthread fake-ecu-libs socket_can obd2 utils)
target_link_libraries(fake-ecu-gui PRIVATE Qt5::Widgets pthread fake-ecu-libs socket_can obd2 utils)
