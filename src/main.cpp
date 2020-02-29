#include <atomic>
#include <chrono>
#include <csignal>
#include <cstdio>
#include <memory>
#include <thread>

#include "ftdi_serial_port.hpp"
#include "mpvi2.hpp"

#include "can_msg.hpp"
#include "obd2.hpp"

std::atomic<bool> keep_running_{true};

void signal_handler(int signal) {
  keep_running_ = false;
}


int main(int argc, char *argv[]) {
  std::signal(SIGINT, signal_handler);

  std::unique_ptr<Mpvi2> mpvi2;
  try {
    mpvi2 = std::make_unique<Mpvi2>();
  } catch(const std::runtime_error &e) {
    printf("Unable to open MPVI2. Exiting...\n");
    return -1;
  }

  printf("Device ID: %d\n", mpvi2->get_device_id());
  printf("Part Num: %d\n", mpvi2->get_part_number());
  printf("P on: %d (s)\n", mpvi2->get_power_on_time());
  printf("Connected: %d (s)\n", mpvi2->get_vehicle_connected_time());

  uint16_t  major, minor, subminor;
  mpvi2->get_hardware_version(major, minor, subminor);
  printf("Hardware Version: %d.%d.%d\n", major, minor, subminor);

  std::thread tx_thread = std::thread([&](){
    pthread_setname_np(pthread_self(), "tx_thread");
    while(keep_running_) {
      mpvi2->send_can(create_obd2_query(Obd2::kEcuRequestAddress,
            Obd2::kService01CurrentData, Obd2::kPidEngineRpm));
      mpvi2->send_can(create_obd2_query(Obd2::kEcuRequestAddress,
            Obd2::kService01CurrentData, Obd2::kPidAmbientAirTemp));
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
  });

  CanMsg can_msg;
  while(keep_running_) {
    if(mpvi2->get_next_can_msg(can_msg, std::chrono::seconds(1))) {
      Obd2::Obd2Msg obd2_msg;
      if(Obd2::Obd2Msg::parse_response(can_msg, obd2_msg)) {

        obd2_msg.get_can().print();
        printf("%s: %lf\n", obd2_msg.pid_name().c_str(), Obd2::decode_value(obd2_msg));
      } else {
        printf("Obd parse failed\n");
        for(const auto &byte : can_msg.data) {
          printf("0x%02X ", byte);
        }
        printf("\n");
      }
    } else {
      printf("CAN msg timeout\n");
    }
  }

  if(tx_thread.joinable()) {
    tx_thread.join();
  }

  return 0;
}
