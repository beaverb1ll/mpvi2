#ifndef FTDI_SERIAL_PORT_HPP_
#define FTDI_SERIAL_PORT_HPP_

#include <memory>
#include <string>

#include "logger.hpp"
#include "serial_port.hpp"

#include "ftd2xx.h"

class FtdiSerialPort : public SerialPort {

 public:

  FtdiSerialPort(const std::string &serial_num);
  ~FtdiSerialPort();

  bool read(std::vector<uint8_t> &data, const uint8_t num_bytes) override;
  bool read(std::vector<uint8_t> &data, const uint8_t num_bytes, const std::chrono::milliseconds &duration) override;
  bool write(const std::vector<uint8_t> &msg) override;

  bool set_baud_rate(const uint32_t rate) override;
  bool set_flow_control(const DataFlowControl flow_control) override;
  bool set_framing(const DataBits data, const DataParity parity, DataStopBits stop_bits) override;
  bool flush() override;
  bool reset() override;

  uint32_t get_num_rx_bytes() override;
  bool wait_for_bytes(const uint32_t num_bytes) override;
  bool wait_for_rx(const std::chrono::milliseconds &timeout = std::chrono::milliseconds::max()) override;

  void set_logger(std::shared_ptr<Logger> logger);

 private:
  FT_HANDLE ftHandle_{nullptr};
  EVENT_HANDLE eh_;
  std::shared_ptr<Logger> logger_ = std::make_shared<Logger>();
};

#endif
