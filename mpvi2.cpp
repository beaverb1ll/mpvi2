#include "mpvi2.hpp"

Mpvi2::Mpvi2(const std::string &filename) : serial_(filename) {

  serial_.set_baud_rate(3000000);
  serial_.write(0);
}

uint32_t Mpvi2::get_device_id() {
  const std::vector<uint8_t> command{0x02, 0x01, 0x02, 0x01, 0x04, 0x10, 0x52, 0xE7, 00};
 // const std::vector<uint8_t> command{0x02, 0x01, 0x02, 0x01, 0x04, 0x14, 0x12, 0x63, 0x00};
  serial_.write(command);
  const auto response = serial_.read();
  printf("response: %d\n", response.size());
  for(int i = 0; i < response.size(); i++) {
    printf("0x%02X ", response[i]);
  }
  return 0;
}
