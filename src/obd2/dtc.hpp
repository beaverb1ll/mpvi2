#ifndef DTC_HPP_
#define DTC_HPP_

struct Dtc {
  enum Type : uint8_t {
    kTypeStored = 0,
    kTypePending,
    kTypeCleared
  };
  Type type;
  char letter;
  uint16_t code;

  Dtc(const Dtc::Type t, const uint16_t raw) {
    type = t;
    const uint8_t letterCode = raw >> 14;
    switch(letterCode) {
      case 0: letter = 'P'; break;
      case 1: letter = 'C'; break;
      case 2: letter = 'B'; break;
      case 3: letter = 'U'; break;
    }
    code = raw & 0x3FFF;
  }

  bool operator==(const Dtc &other) const {
    return type == other.type && letter == other.letter && code == other.code;
  }
};

#endif
