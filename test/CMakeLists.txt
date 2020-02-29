enable_testing()
include_directories(${CMAKE_SOURCE_DIR}/src)

macro(add_test_executable TEST_NAME)
  add_executable(${TEST_NAME} ${TEST_NAME}.cpp)
  target_link_libraries(${TEST_NAME} PRIVATE ${REQUIRED_LIBS})
  add_test(${TEST_NAME} ${TEST_NAME})
endmacro()


set(REQUIRED_LIBS
  ${CMAKE_THREAD_LIBS_INIT}
  ${GTEST_BOTH_LIBRARIES}
  ${GMOCK_LIBRARIES}
  ${FTDI_LIBRARIES}
  serial_port
)

add_test_executable(test_can_send)
add_test_executable(test_can_receive)
add_test_executable(test_can_obd)

