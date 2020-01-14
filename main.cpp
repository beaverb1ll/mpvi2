#include <cstdio>
#include <memory>

#include "ftdi_serial_port.hpp"
#include "mpvi2.hpp"

int main(int argc, char *argv[]) {

  auto serial = std::make_shared<FtdiSerialPort>(Mpvi2::kSerialNumber);
  Mpvi2 mpvi2(serial);

  mpvi2.get_device_id();
  mpvi2.get_part_number();

  return 0;
}
