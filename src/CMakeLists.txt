
add_library(serial_port mpvi2.cpp ftdi_serial_port.cpp)

add_executable(main1
  main.cpp
)
include_directories(SYSTEM
  ${FTDI_INCLUDE_DIR}
)


target_link_libraries(main1 PRIVATE pthread ${FTDI_LIBRARIES} serial_port)
