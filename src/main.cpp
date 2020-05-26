#include <atomic>
#include <chrono>
#include <csignal>
#include <cstdio>
#include <memory>
#include <thread>

#include "arg_parse.hpp"
#include "console_logger.hpp"
#include "utils.hpp"

#include "ftdi_serial_port.hpp"
#include "mpvi2.hpp"

#include "obd2/can_msg.hpp"
#include "obd2/obd2.hpp"
#include "obd2/iso_tp.hpp"

#include "channels.hpp"
#include "channel_manager.hpp"

std::atomic<bool> keep_running_{true};
std::unique_ptr<Mpvi2> mpvi2;

void scan_for_obd_pids(Mpvi2 &mpvi, ChannelManager &cm);

void signal_handler(int signal) {
  keep_running_ = false;
}

int main(int argc, char *argv[]) {
  std::signal(SIGINT, signal_handler);

  ArgParse args;
  args.register_option("log-level", "Console log level", "info");
  args.parse(argc, argv);
  const auto log_level = args.get_option("log-level");
  printf("Log Level: %s\n", log_level.c_str());
  UtilManager::set_logger(std::make_shared<ConsoleLogger>(log_level));

  try {
    mpvi2 = std::make_unique<Mpvi2>(UtilManager::logger());
  } catch(const std::runtime_error &e) {
    LOG_FATAL(*UtilManager::logger(), "Unable to open MPVI2. Exiting...");
    return -1;
  }

  LOG_INFO(*UtilManager::logger(), "Device ID: %d", mpvi2->get_device_id());
  LOG_INFO(*UtilManager::logger(), "Part Num: %d", mpvi2->get_part_number());
  LOG_INFO(*UtilManager::logger(), "P on: %d (s)", mpvi2->get_power_on_time());
  LOG_INFO(*UtilManager::logger(), "Connected: %d (s)", mpvi2->get_vehicle_connected_time());

  uint16_t  major, minor, subminor;
  mpvi2->get_hardware_version(major, minor, subminor);
  LOG_INFO(*UtilManager::logger(), "Hardware Version: %d.%d.%d", major, minor, subminor);

  ChannelManager cm;

  scan_for_obd_pids(*mpvi2, cm);
  for(const auto &cm : cm.get_channels()) {
    const auto valid_pids = cm.second.get_supported_pids();
    const auto ecu_id = cm.first;
    std::string pid_string;
    for(const auto &pid : valid_pids) {
      pid_string += Obd2::service_01_to_string(pid) + ", ";
    }
    printf("0x%X: %s\n", ecu_id, pid_string.c_str());
  }

//  std::thread tx_thread = std::thread([&](){
//    pthread_setname_np(pthread_self(), "tx_thread");
//
//    mpvi2->send_can(create_obd2_query(Obd2::kEcuStartAddress,
//          Obd2::kService09RequestInfo, Obd2::kPidVin));
//    std::this_thread::sleep_for(std::chrono::seconds(2));
//
//    while(keep_running_) {
//      mpvi2->send_can(create_obd2_query(Obd2::kEcuStartAddress,
//            Obd2::kService01CurrentData, Obd2::kPidEngineRpm));
//      mpvi2->send_can(create_obd2_query(Obd2::kEcuStartAddress  ,
//            Obd2::kService01CurrentData, Obd2::kPidAmbientAirTemp));
//      std::this_thread::sleep_for(std::chrono::milliseconds(100));
//    }
//  });

//  CanMsg can_msg;
//  IsoTp::IsoTpReceiveMsg vinNum{{Obd2::kEcuStartAddress, Obd2::kService09RequestInfo, Obd2::kPidVin, 17, {}}};
//  if(mpvi2->get_next_can_msg(can_msg, std::chrono::seconds(1))) {
//    printf("%s\n", can_msg.to_string().c_str());
//    vinNum.process_rx_frame(can_msg);
//    if(vinNum.get_state() == IsoTp::IsoTpReceiveMsg::kRxSendingFlowControl) {
//      printf("sending flow control\n");
//      mpvi2->send_can(vinNum.get_flow_control());
//    }
//  }
//  while(vinNum.get_state() != IsoTp::IsoTpReceiveMsg::kRxComplete) {
//    if(mpvi2->get_next_can_msg(can_msg, std::chrono::seconds(1))) {
//      printf("%s\n", can_msg.to_string().c_str());
//      vinNum.process_rx_frame(can_msg);
//    }
//  }
//  printf("VIN: ");
//  for(int i = 0; i < vinNum.get_data().length; i++) {
//    printf("%c", vinNum.get_data().data[i]);
//  }
//  printf("\n");
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

  //if(tx_thread.joinable()) {
  //  tx_thread.join();
  //}

  return 0;
}

void scan_for_obd_pids(Mpvi2 &mpvi, ChannelManager &cm) {
  auto now = std::chrono::high_resolution_clock::now();
  auto start = now;

  LOG_TRACE(*UtilManager::logger(), "get_supported_obd_pids");
  mpvi.write_can(create_obd2_query(Obd2::kEcuBroadcastAddress, Obd2::kService01CurrentData, Obd2::kPidSupported01To20));
  CanMsg out;
  Obd2::Obd2Msg obd2;
  while(now - start < std::chrono::seconds(1)) {
    if(mpvi.read_can(out, std::chrono::seconds(1))) {
      LOG_TRACE(*UtilManager::logger(), "Got valid msg: %s", out.to_string().c_str());
      start = now;
      if(Obd2::Obd2Msg::parse_response(out, obd2)) {
        const auto ecu_id = obd2.can_id - Obd2::kObd2RequestResponseAddressOffset;
        if(cm.get_channels()[ecu_id].set_supported_pids(obd2, out)) {
          LOG_TRACE(*UtilManager::logger(), "did parse pid. response: %s", out.to_string().c_str());
          mpvi.write_can(out);
        }
      }
    }
    now = std::chrono::high_resolution_clock::now();
  }
}

