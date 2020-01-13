
#include "serial_port.hpp"


#include <cstdio>
#include <stdexcept>

#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */

#include <asm/termbits.h>
#include <sys/ioctl.h>


SerialPort::SerialPort(const std::string &filename) {
  fd_ = open_port(filename);
}

SerialPort::~SerialPort() {
  if(fd_ > 0) {
    close(fd_);
  }
}

int SerialPort::open_port(const std::string &filename) {
  const int fd = open(filename.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
  if (fd == -1) {
   /*
    * Could not open the port.
    */
    throw std::runtime_error("unable to open serial: " + filename);
  }

  fcntl(fd, F_SETFL, FNDELAY);
  return (fd);
}

std::vector<uint8_t> SerialPort::read(const uint8_t buff_size) {
  std::vector<uint8_t> buff{buff_size, 0};
  const auto n = ::read(fd_, buff.data(), buff_size);
  return buff;
}

bool SerialPort::write(const uint8_t msg) {
  const auto n = ::write(fd_, &msg, 1);
  return (n==1);
}

bool SerialPort::write(const std::string &msg) {
  const auto n = ::write(fd_, msg.c_str(), msg.length());
  return (n==msg.length());
}

bool SerialPort::write(const std::vector<uint8_t> &msg) {
  const auto n = ::write(fd_, msg.data(), msg.size());
  return (n==msg.size());
}

void SerialPort::set_baud_rate(const uint32_t rate) {
  struct termios2 tio;

  ioctl(fd_, TCGETS2, &tio);
  tio.c_cflag &= ~CBAUD;
  tio.c_cflag |= BOTHER;
  tio.c_ispeed = rate;
  tio.c_ospeed = rate;

  tio.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // raw mode
  tio.c_cflag &= ~PARENB;
  tio.c_cflag &= ~CSTOPB;  // 1 stop bit
  tio.c_cflag &= ~CSIZE;
  tio.c_cflag |= CS8;   // 8 bits
  tio.c_cflag |= CRTSCTS;
  ioctl(fd_, TCSETS2, &tio);




}
