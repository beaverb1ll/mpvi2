#include <atomic>
#include <chrono>
#include <csignal>
#include <cstdio>

#include "socket_can.hpp"
#include "obd2/obd2.hpp"
#include "obd2/iso_tp.hpp"

#include "utils.hpp"
#include "arg_parse.hpp"
#include "console_logger.hpp"

void send_response(SocketCan &can, const Obd2::Obd2Msg &msg);
bool process_tp(SocketCan &can, const CanMsg &msg);
void process_obd(SocketCan &can, const CanMsg &msg);

std::atomic<bool> keep_running{true};
void signal_handler(int signal) {
  keep_running = false;
}

IsoTp::IsoTpTransmitMsg current_isotp;

const uint16_t ecu_address = Obd2::kEcuStartAddress;
const uint16_t ecu_reply_address = ecu_address + Obd2::kObd2RequestResponseAddressOffset;

int main(int argc, char *argv[]) {

  ArgParse args;
  args.register_option("device,d", "SocketCAN device", "can0");
  args.register_option("log-level", "Console log level", "info");
  args.parse(argc, argv);

  UtilManager::set_logger(std::make_shared<ConsoleLogger>(args.get_option("log-level")));
  SocketCan can(args.get_option("device"));
  std::signal(SIGINT, signal_handler);

  CanMsg msg;
  while(keep_running) {
    if(!can.read(msg, std::chrono::milliseconds(2000))) {
      LOG_ERROR(*UtilManager::logger(), "read failed");
      continue;
    }
    LOG_TRACE(*UtilManager::logger(), "Rx: %s", msg.to_string().c_str());

    if(process_tp(can, msg)) {
      LOG_INFO(*UtilManager::logger(), "Got a TP msg");
    } else {
      process_obd(can, msg);
    }
  }
  return 0;
}

void process_obd(SocketCan &can, const CanMsg &msg) {
  Obd2::Obd2Msg obd;
  if(Obd2::Obd2Msg::parse_response(msg, obd)) {
    LOG_TRACE(*UtilManager::logger(), "%s", obd.to_string().c_str());
    send_response(can, obd);
  } else {
    LOG_ERROR(*UtilManager::logger(), "parse failed");
  }
}

bool process_tp(SocketCan &can, const CanMsg &in_msg) {
  bool ret_val = false;
  if(current_isotp.is_runnable()) {
    if(!current_isotp.process_rx_frame(in_msg)) {
      LOG_ERROR(*UtilManager::logger(), "Did't get flow control");
    } else {
      ret_val = true;
    }
    while(current_isotp.get_state() == IsoTp::IsoTpTransmitMsg::kTxSending) {
      CanMsg out;
      if(current_isotp.get_next_tx_frame(out)) {
        LOG_TRACE(*UtilManager::logger(), "%s", out.to_string().c_str());
        can.write(out);
      } else {
        LOG_ERROR(*UtilManager::logger(), "ISO TP tx failed");
      }
    }
  }
  return ret_val;
}

bool process_service01(const Obd2::Obd2Msg &requset, CanMsg &out) {
  LOG_TRACE(*UtilManager::logger(), "process_service01");
  Obd2::Obd2Msg response;
  response.pid = requset.pid;
  response.can_id = Obd2::kEcuResponseAddress;
  response.service = requset.service + Obd2::kServiceResponseOffset;
  if(!response.encode_value(requset.pid)) {
    LOG_ERROR(*UtilManager::logger(), "encode failed");
    return false;
  }
  out = response.get_can();
  return true;
}

bool process_service09(const Obd2::Obd2Msg &request, CanMsg &response) {
  static const IsoTp::IsoTpTransmitMsg vinNum{{
    .can_id = Obd2::kEcuResponseAddress,
    .service = Obd2::kService09RequestInfo,
    .pid = Obd2::kPidVin,
    .length = 17,
    .data = {0x31, 0x47, 0x43, 0x4e, 0x43, 0x4e, 0x45, 0x48, 0x35, 0x4a, 0x5a, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf}}};

  LOG_TRACE(*UtilManager::logger(), "process_service09");
  Obd2::Obd2Msg out;
  switch(request.pid) {
    case Obd2::kPid09Supported01To20:
      out.can_id = 0x7E8;
      out.service = 0x49;
      out.pid = Obd2::kPid09Supported01To20;
      out.num_bytes = 4;
      out.data[0] = 0xC0;
      out.data[1] = 0x00;
      out.data[2] = 0x00;
      out.data[3] = 0x00;
      response = out.get_can();
      return true;
    break;

    case Obd2::kPidVin:
    current_isotp = vinNum;
    return current_isotp.get_next_tx_frame(response);
  }
  return false;
}

void send_response(SocketCan &can, const Obd2::Obd2Msg &request) {
  CanMsg response;
  bool valid = false;
  switch(request.service) {
    case Obd2::kService01CurrentData:
      valid = process_service01(request, response);
      break;

    case Obd2::kService09RequestInfo:
      valid = process_service09(request, response);
      break;

    default:
      LOG_WARN(*UtilManager::logger(), "No response for service:0x%02X pid: 0x%02X", request.service, request.pid);
      return;
  }
  if(!valid) {
    LOG_WARN(*UtilManager::logger(), "No valid response: 0x%02X 0x%02X", request.service, request.pid);
    return;
  }
  LOG_TRACE(*UtilManager::logger(), "Tx: %s\n", response.to_string().c_str());
  if(!can.write(response)) {
    LOG_ERROR(*UtilManager::logger(), "write failed");
  }
}
