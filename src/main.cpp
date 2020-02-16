#include <chrono>
#include <cstdio>
#include <memory>
#include <thread>

#include "ftdi_serial_port.hpp"
#include "mpvi2.hpp"

#include "obd2.hpp"


int main(int argc, char *argv[]) {

  auto serial = std::make_shared<FtdiSerialPort>(Mpvi2::kSerialNumber);
  Mpvi2 mpvi2(serial);

  printf("Device ID: %d\n", mpvi2.get_device_id());
  printf("Part Num: %d\n", mpvi2.get_part_number());
  printf("P on: %d (s)\n", mpvi2.get_power_on_time());
  printf("Connected: %d (s)\n", mpvi2.get_vehicle_connected_time());

  uint16_t  major, minor, subminor;
  mpvi2.get_hardware_version(major, minor, subminor);
  printf("Hardware Version: %d.%d.%d\n", major, minor, subminor);


  mpvi2.send_can({0x07DF, {0x02, Obd2::kModeCurrentData, Obd2::kPidEngineRpm, 0, 0, 0, 0, 0}});
  Mpvi2::CanMsg can_msg;
  while(true) {
    if(mpvi2.get_next_can_msg(can_msg)) {
      // do something with can msg
      for(const auto &byte : can_msg.data) {
        printf("0x%02X ", byte);
      }
      printf("%lf\n", (((can_msg.data[3]*256.0) + can_msg.data[4])/4.0));
    } else {
      printf("No valid can msg\n");
    }
  }

  return 0;
}
