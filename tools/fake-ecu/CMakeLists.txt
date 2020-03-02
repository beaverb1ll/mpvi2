
add_executable(fake-ecu
  main.cpp
  socket_can.cpp
)


target_link_libraries(fake-ecu PRIVATE pthread obd2 utils)
