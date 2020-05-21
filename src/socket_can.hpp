#ifndef SOCKET_CAN_HPP_
#define SOCKET_CAN_HPP_

#include <chrono>
#include <string>

#include "can_if.hpp"
#include "obd2/can_msg.hpp"

class SocketCan : public CanInterface {
 public:
  SocketCan(const std::string &device_name);
  ~SocketCan();

  bool write_can(const CanMsg &msg) override;
  bool read_can(CanMsg &msg, const std::chrono::milliseconds &timeout = std::chrono::milliseconds::max()) override;

 private:
  int socket_;

};

#endif
