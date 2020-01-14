
#ifndef MPVI2_HPP_
#define MPVI2_HPP_

#include <memory>
#include <string>
#include <vector>

#include "serial_port.hpp"

class Mpvi2 {
 public:
  static const std::string kSerialNumber;

  Mpvi2(std::shared_ptr<SerialPort> serial);
  uint32_t get_device_id();
  uint16_t get_part_number();

 private:

  bool write(std::vector<uint8_t> &data);
  bool read(std::vector<uint8_t> &data, const uint8_t num_bytes);

  std::shared_ptr<SerialPort> serial_;
};

#endif
