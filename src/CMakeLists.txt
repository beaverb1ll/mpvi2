
add_library(serial_port mpvi2.cpp ftdi_serial_port.cpp)

add_executable(main1
  main.cpp
)


target_link_libraries(main1 PRIVATE pthread ftd2xx.so serial_port)
