#ifndef FTDI_SERIAL_PORT_HPP_
#define FTDI_SERIAL_PORT_HPP_

#include <string>

#include "serial_port.hpp"

#include "/home/user/Downloads/libftd2xx-x86_64-1.4.8/release/ftd2xx.h"

class FtdiSerialPort : public SerialPort {

 public:

  FtdiSerialPort(const std::string &serial_num);

  bool read(std::vector<uint8_t> &data, const uint8_t num_bytes) override;
  bool write(const std::vector<uint8_t> &msg) override;

  bool set_baud_rate(const uint32_t rate) override;
  bool set_flow_control(const DataFlowControl flow_control) override;
  bool set_framing(const DataBits data, const DataParity parity, DataStopBits stop_bits) override;
  bool flush() override;
  bool reset() override;

 private:
  FT_HANDLE ftHandle_{nullptr};

};

#endif
