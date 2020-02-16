enable_testing()

set(TEST_NAME test_can_send)

add_executable(${TEST_NAME}
  ${TEST_NAME}.cpp
)

include_directories(${CMAKE_SOURCE_DIR}/src)
target_link_libraries(${TEST_NAME} PRIVATE ${CMAKE_THREAD_LIBS_INIT} ${GTEST_BOTH_LIBRARIES} ${GMOCK_LIBRARIES} serial_port)
add_test(${TEST_NAME} ${TEST_NAME})
