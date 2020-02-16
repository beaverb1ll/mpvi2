#ifndef LINUX_SERIAL_PORT_HPP_
#define LINUX_SERIAL_PORT_HPP_

#include "serial_port.hpp"

#include <string>
#include <vector>

class SerialPort : public SerialPort {

 public:
  SerialPort(const std::string &filename);
  ~SerialPort();

  std::vector<uint8_t> read(const uint8_t buff_size = 4);
  bool write(const uint8_t msg);
  bool write(const std::string &msg);
  bool write(const std::vector<uint8_t> &msg);

  void set_baud_rate(const uint32_t rate);

 private:
  int open_port(const std::string &filename);

  int fd_;
};

#endif
