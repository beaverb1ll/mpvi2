#include <atomic>
#include <chrono>
#include <csignal>
#include <cstdio>
#include <memory>
#include <thread>

#include "ftdi_serial_port.hpp"
#include "mpvi2.hpp"

#include "obd2/can_msg.hpp"
#include "obd2/obd2.hpp"
#include "obd2/iso_tp.hpp"

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

    mpvi2->send_can(create_obd2_query(Obd2::kEcuStartAddress,
          Obd2::kService09RequestInfo, Obd2::kPidVin));
    std::this_thread::sleep_for(std::chrono::seconds(2));

//    while(keep_running_) {
//      mpvi2->send_can(create_obd2_query(Obd2::kEcuStartAddress,
//            Obd2::kService01CurrentData, Obd2::kPidEngineRpm));
//      mpvi2->send_can(create_obd2_query(Obd2::kEcuStartAddress  ,
//            Obd2::kService01CurrentData, Obd2::kPidAmbientAirTemp));
//      std::this_thread::sleep_for(std::chrono::milliseconds(100));
//    }
  });

  CanMsg can_msg;
  IsoTp::IsoTpReceiveMsg vinNum{{Obd2::kEcuStartAddress, Obd2::kService09RequestInfo, Obd2::kPidVin, 17, {}}};
  if(mpvi2->get_next_can_msg(can_msg, std::chrono::seconds(1))) {
    printf("%s\n", can_msg.to_string().c_str());
    vinNum.process_rx_frame(can_msg);
    if(vinNum.get_state() == IsoTp::IsoTpReceiveMsg::kRxSendingFlowControl) {
      printf("sending flow control\n");
      mpvi2->send_can(vinNum.get_flow_control());
    }
  }
  while(vinNum.get_state() != IsoTp::IsoTpReceiveMsg::kRxComplete) {
    if(mpvi2->get_next_can_msg(can_msg, std::chrono::seconds(1))) {
      printf("%s\n", can_msg.to_string().c_str());
      vinNum.process_rx_frame(can_msg);
    }
  }
  printf("VIN: ");
  for(int i = 0; i < vinNum.get_data().length; i++) {
    printf("%c", vinNum.get_data().data[i]);
  }
  printf("\n");
  keep_running_ = false;

  //while(keep_running_) {
  //  if(mpvi2->get_next_can_msg(can_msg, std::chrono::seconds(1))) {
  //    Obd2::Obd2Msg obd2_msg;
  //    if(Obd2::Obd2Msg::parse_response(can_msg, obd2_msg)) {

  //      obd2_msg.get_can().print();
  //      printf("%s: %lf\n", obd2_msg.pid_name().c_str(), Obd2::decode_value(obd2_msg));
  //    } else {
  //      printf("Obd parse failed\n");
  //      for(const auto &byte : can_msg.data) {
  //        printf("0x%02X ", byte);
  //      }
  //      printf("\n");
  //    }
  //  } else {
  //    printf("CAN msg timeout\n");
  //  }
  //}

  if(tx_thread.joinable()) {
    tx_thread.join();
  }

  return 0;
}
