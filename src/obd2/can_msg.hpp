#ifndef CAN_MSG_HPP
#define CAN_MSG_HPP

#include <array>
#include <cstdint>
#include <string>

struct CanMsg {
  uint32_t id = 0;
  uint8_t length = 0;
  std::array<uint8_t, 8> data;

  std::string to_string() const {
    // xxx#yy yy yy yy yy yy yy yy
    std::string out;
    out.reserve(35);
    char temp[10];
    temp[9] = '\0';

    snprintf(temp, 9, "%03X", id);
    out+=temp;
    out+="#";
    for(const auto &a : data) {
      snprintf(temp, 9, "%02X ", a);
      out+=temp;
    }
    return out;
  }
};

#endif
