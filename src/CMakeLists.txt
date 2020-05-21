add_subdirectory(obd2)

add_library(socket_can socket_can.cpp)
add_library(mpvi2 ftdi_serial_port.cpp mpvi2.cpp channel.cpp channels.cpp channel_manager.cpp)

add_executable(main1
  main.cpp
)
include_directories(SYSTEM
  ${FTDI_INCLUDE_DIR}
)

target_link_libraries(main1 PRIVATE pthread ${FTDI_LIBRARIES} mpvi2 obd2 utils)
