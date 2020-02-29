#ifndef SERIAL_PORT_HPP_
#define SERIAL_PORT_HPP_

#include <chrono>
#include <cstdint>
#include <vector>

class SerialPort {
 public:
  enum DataBits {
    kDataBits7 = 0,
    kDataBits8
  };
  enum DataParity {
    kDataParityNone = 0,
    kDataParityOdd,
    kDataParityEven,
    kDataParityMark,
    kDataParitySpace
  };
  enum DataStopBits {
    kDataStopBits1 = 0,
    kDataStopBits2
  };

  enum DataFlowControl {
    kDataFlowControlNone = 0,
    kDataFlowControlRtsCts,
    kDataFlowControlDtrDsr,
    kDataFlowControlXOnXOff
  };

  virtual ~SerialPort() = default;

  virtual bool read(std::vector<uint8_t> &data, const uint8_t num_bytes) = 0;
  virtual bool read(std::vector<uint8_t> &data, const uint8_t num_bytes, const std::chrono::milliseconds &duration) = 0;
  virtual bool write(const std::vector<uint8_t> &msg) = 0;

  virtual bool set_baud_rate(const uint32_t rate) = 0;
  virtual bool set_flow_control(const DataFlowControl flow_control) = 0;
  virtual bool set_framing(const DataBits data, const DataParity parity, DataStopBits stop_bits) = 0;
  virtual bool flush() = 0;
  virtual bool reset() = 0;
  virtual uint32_t get_num_rx_bytes() = 0;
  virtual bool wait_for_bytes(const uint32_t num_bytes) = 0;
  virtual bool wait_for_rx(const std::chrono::milliseconds &timeout = std::chrono::milliseconds::max()) = 0;

};

#endif
