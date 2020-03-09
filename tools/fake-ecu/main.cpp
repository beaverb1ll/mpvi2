#include <atomic>
#include <chrono>
#include <csignal>
#include <cstdio>

#include "ecu_obd2.hpp"
#include "obd2/obd2.hpp"
#include "obd2/iso_tp.hpp"
#include "socket_can.hpp"

#include "arg_parse.hpp"
#include "console_logger.hpp"
#include "utils.hpp"

std::atomic<bool> keep_running{true};
void signal_handler(int signal) {
  keep_running = false;
}

const uint16_t ecu_address = Obd2::kEcuStartAddress;
const std::string vin_num = "1GCNCNEH5JZABCDEF";

int main(int argc, char *argv[]) {

  ArgParse args;
  args.register_option("device,d", "SocketCAN device", "can0");
  args.register_option("log-level", "Console log level", "info");
  args.parse(argc, argv);

  const auto log_level = args.get_option("log-level");
  printf("Log Level: %s\n", log_level.c_str());

  UtilManager::set_logger(std::make_shared<ConsoleLogger>(log_level));
  SocketCan can(args.get_option("device"));
  EcuObd2 ecu(ecu_address);
  ecu.set_send_function([&](const CanMsg &msg){
    LOG_DEBUG(*UtilManager::logger(), "Tx: %s", msg.to_string().c_str());
    can.write(msg);
  });
  ecu.set_vin(vin_num);
  ecu.set_pid_supported(Obd2::kService01CurrentData, Obd2::kPidEngineRpm);
  ecu.set_pid_supported(Obd2::kService01CurrentData, Obd2::kPidVehicleSpeed);
  ecu.set_pid_supported(Obd2::kService01CurrentData, Obd2::kPidAmbientAirTemp);

  std::signal(SIGINT, signal_handler);

  CanMsg msg;
  while(keep_running) {
    if(!can.read(msg, std::chrono::milliseconds(2000))) {
      LOG_TRACE(*UtilManager::logger(), "read failed");
      continue;
    }

    LOG_DEBUG(*UtilManager::logger(), "Rx: %s", msg.to_string().c_str());
    if(!ecu.process_rx_packet(msg)) {
      LOG_DEBUG(*UtilManager::logger(), "ECU uninteresting packet: %s", msg.to_string().c_str());
    }
  }
  return 0;
}
