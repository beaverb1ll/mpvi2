
#ifndef CAN_MSG_HPP
#define CAN_MSG_HPP

#include <array>
#include <cstdint>

struct CanMsg {
  uint32_t id;
  uint8_t length;
  std::array<uint8_t, 8> data;

  void print() const {
    printf("%02X :: ", id);
    for(const auto &a : data) {
      printf("%02X ", a);
    }
    printf("\n");
  }
};

#endif

