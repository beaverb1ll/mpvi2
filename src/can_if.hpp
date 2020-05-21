#ifndef CAN_IF_HPP_
#define CAN_IF_HPP_

#include <chrono>
#include <string>

#include "obd2/can_msg.hpp"

class CanInterface {
 public:

  virtual bool write_can(const CanMsg &msg) = 0;
  virtual bool read_can(CanMsg &msg, const std::chrono::milliseconds &timeout = std::chrono::milliseconds::max()) = 0;

};

#endif
