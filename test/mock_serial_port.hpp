
#ifndef MOCK_SERIAL_PORT_HPP
#define MOCK_SERIAL_PORT_HPP

#include <gmock/gmock.h>

#include "serial_port.hpp"

class MockSerialPort : public SerialPort {
 public:
  MOCK_METHOD2(read, bool(std::vector<uint8_t> &data, const uint8_t num_bytes));
  MOCK_METHOD3(read, bool(std::vector<uint8_t> &data, const uint8_t num_bytes, const std::chrono::milliseconds &duration));
  MOCK_METHOD1(write, bool(const std::vector<uint8_t> &msg));

  MOCK_METHOD1(set_baud_rate, bool(const uint32_t rate));
  MOCK_METHOD1(set_flow_control, bool(const DataFlowControl flow_control));
  MOCK_METHOD3(set_framing, bool(const DataBits data, const DataParity parity, DataStopBits stop_bits));
  MOCK_METHOD0(flush, bool());
  MOCK_METHOD0(reset, bool());
  MOCK_METHOD0(get_num_rx_bytes, uint32_t());
  MOCK_METHOD1(wait_for_rx, bool(const std::chrono::milliseconds&));
  MOCK_METHOD1(wait_for_bytes, bool(const uint32_t num_bytes));
};

#endif

