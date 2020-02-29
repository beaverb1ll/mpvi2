#ifndef SOCKET_CAN_HPP_
#define SOCKET_CAN_HPP_

#include <chrono>
#include <string>

#include "can_msg.hpp"

class SocketCan {
 public:
  SocketCan(const std::string &device_name);
  ~SocketCan();

  bool write(const CanMsg &msg);
  bool read(CanMsg &msg, const std::chrono::milliseconds &timeout = std::chrono::milliseconds::max());

 private:
  int socket_;

};

#endif
