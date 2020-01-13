
#ifndef MPVI2_HPP_
#define MPVI2_HPP_

#include <string>

#include "serial_port.hpp"

class Mpvi2 {
 public:
  Mpvi2(const std::string &filename);
  uint32_t get_device_id();

 private:
  SerialPort serial_;
};

#endif
