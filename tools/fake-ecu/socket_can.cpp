#include "socket_can.hpp"

#include <cstring>
#include <stdexcept>

#include <unistd.h>
#include <poll.h>

#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include <linux/can.h>
#include <linux/can/raw.h>

SocketCan::SocketCan(const std::string &device_name) {
  /* open socket */
  if ((socket_ = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
    throw std::runtime_error("Unable to create CAN socket");
  }
  struct ifreq ifr;
  std::strncpy(ifr.ifr_name, device_name.c_str(), IFNAMSIZ - 1);
  ifr.ifr_name[IFNAMSIZ - 1] = '\0';
  ifr.ifr_ifindex = if_nametoindex(ifr.ifr_name);
  if (!ifr.ifr_ifindex) {
    throw std::runtime_error("Unable to find SocketCan device: " + device_name);
  }

  struct sockaddr_can addr;
  memset(&addr, 0, sizeof(addr));
  addr.can_family = AF_CAN;
  addr.can_ifindex = ifr.ifr_ifindex;

  if (bind(socket_, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    perror("bind");
    throw std::runtime_error("");
  }
}

SocketCan::~SocketCan() {
  if(socket_ > -1) {
  close(socket_);
  }
}

bool SocketCan::write(const CanMsg &msg) {
  struct can_frame frame;
  frame.can_id = msg.id;
  frame.can_dlc= msg.length;
  memcpy(frame.data, msg.data.data(), msg.length);

  if (::write(socket_, &frame, sizeof(frame)) != sizeof(frame)) {
    return false;
  }
  return true;
}

bool SocketCan::read(CanMsg &msg, const std::chrono::milliseconds &timeout) {
  struct pollfd fd;
  fd.fd = socket_;
  fd.events = POLLIN;
  if(poll(&fd, 1, timeout.count()) < 1) {
    return false;
  }
  struct can_frame frame;
  const int nbytes = recv(socket_, &frame, sizeof(frame), 0);
  if(nbytes != sizeof(frame)) {
    printf("nbytes mismatch. expected: %ld actual: %d\n", sizeof(frame), nbytes);
    return false;
  }
  msg.id = frame.can_id;
  msg.length = frame.can_dlc;
  memcpy(msg.data.data(), frame.data, frame.can_dlc);
  return true;
}
