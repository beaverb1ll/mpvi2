#include <cstdio>

#include "mpvi2.hpp"

int main(int argc, char *argv[]) {
  Mpvi2 mpvi2("/dev/ttyUSB0");

  mpvi2.get_device_id();


  return 0;
}
