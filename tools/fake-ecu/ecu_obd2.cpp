#include "ecu_obd2.hpp"

EcuObd2::EcuObd2(const uint16_t can_id) :
ecu_can_id_(can_id), ecu_response_id_(can_id + Obd2::kObd2RequestResponseAddressOffset) {
  set_pid_supported(Obd2::kService01CurrentData, Obd2::kPidSupported01To20);
  set_pid_supported(Obd2::kService01CurrentData, Obd2::kPidSupported21To40);
  set_pid_supported(Obd2::kService01CurrentData, Obd2::kPidSupported41To60);
  set_pid_supported(Obd2::kService01CurrentData, Obd2::kPidSupported61To80);
}

void EcuObd2::set_send_function(const send_func_t &func) {
  send_packet_func_ = func;
}

void EcuObd2::send_packet(const CanMsg &out) {
  if(send_packet_func_) {
    send_packet_func_(out);
  }
}

void EcuObd2::set_vin(const std::string &vin) {
  vin_number_ = vin;
  vin_number_.resize(17); // only allow 17 digits in VIN
}

void EcuObd2::set_pid_supported(const Obd2::Services service, const uint8_t pid) {
  if(service == Obd2::kService01CurrentData) {
    service01_supported_pids_[pid] = true;
  }
}

bool EcuObd2::process_rx_packet(const CanMsg &msg) {

  if(msg.id != ecu_can_id_ && msg.id != Obd2::kEcuBroadcastAddress) {
    return false;
  }

  bool ret_val = false;
  if(process_tp(msg)) {
    ret_val = true;
    LOG_TRACE(*UtilManager::logger(), "Got a TP msg");
  } else if(process_obd(msg)) {
    ret_val = true;
  }
  return ret_val;
}

bool EcuObd2::process_obd(const CanMsg &msg) {
  bool ret_val = false;
  Obd2::Obd2Msg obd;
  if(Obd2::Obd2Msg::parse_response(msg, obd)) {
    LOG_TRACE(*UtilManager::logger(), "%s", obd.to_string().c_str());
    send_response(obd);
    ret_val = true;
  } else {
    LOG_ERROR(*UtilManager::logger(), "parse failed");
  }
  return ret_val;
}

bool EcuObd2::process_tp(const CanMsg &in_msg) {
  bool ret_val = false;
  if(current_isotp_.is_runnable() && current_isotp_.process_rx_frame(in_msg)) {
    ret_val = true;
    CanMsg out;
    while(current_isotp_.get_state() == IsoTp::IsoTpTransmitMsg::kTxSending) {
      if(current_isotp_.get_next_tx_frame(out)) {
        LOG_TRACE(*UtilManager::logger(), "%s", out.to_string().c_str());
        send_packet(out);
      } else {
        LOG_ERROR(*UtilManager::logger(), "ISO TP tx failed");
      }
    }
  }
  return ret_val;
}

void EcuObd2::send_response(const Obd2::Obd2Msg &request) {
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
  send_packet(response);
}

bool EcuObd2::process_service01(const Obd2::Obd2Msg &request, CanMsg &out) {
  LOG_TRACE(*UtilManager::logger(), "process_service01");
  Obd2::Obd2Msg response;
  response.pid = request.pid;
  response.can_id = ecu_response_id_;
  response.service = request.service + Obd2::kServiceResponseOffset;
  bool valid = false;
  switch(request.pid) {
    case Obd2::kPidSupported01To20:
    // fallthrough
    case Obd2::kPidSupported21To40:
    // fallthrough
    case Obd2::kPidSupported41To60:
    // fallthrough
    case Obd2::kPidSupported61To80:
    valid =  encode_service_01_supported_pid(response);
    break;

    default:
    valid = response.encode_value(request.pid);
    break;
  }
  if(!valid) {
    LOG_WARN(*UtilManager::logger(), "service01 encode failed");
    return false;
  }
  out = response.to_can();
  return true;
}

bool EcuObd2::encode_service_01_supported_pid(Obd2::Obd2Msg &out) {
  if(out.pid != Obd2::kPidSupported01To20 &&
    out.pid !=  Obd2::kPidSupported21To40 &&
    out.pid !=  Obd2::kPidSupported41To60 &&
    out.pid !=  Obd2::kPidSupported61To80) {
    return false;
  }

  memset(out.data.data(), 0, sizeof(out.data));
  out.num_bytes = 4;
  const auto base_offset = out.pid + 1;
  for(int i = 0; i < 0x20; i++) {
    const int index = i/8;
    const int shift = 7 - (i%8);
    const bool val = service01_supported_pids_[base_offset+i];
    out.data[index] |= ( val << (shift) );
  }
  return true;
}

bool EcuObd2::process_service09(const Obd2::Obd2Msg &request, CanMsg &response) {
  static IsoTp::IsoTpData vinNum{
    .can_id = ecu_response_id_,
    .service = Obd2::kService09RequestInfo,
    .pid = Obd2::kPidVin,
    .length = 0,
    .data = {}};

  LOG_TRACE(*UtilManager::logger(), "process_service09");
  Obd2::Obd2Msg out;
  switch(request.pid) {
    case Obd2::kPid09Supported01To20:
      out.can_id = ecu_response_id_;
      out.service = request.service + Obd2::kServiceResponseOffset;
      out.pid = Obd2::kPid09Supported01To20;
      out.num_bytes = 4;
      out.data[0] = 0xC0;
      out.data[1] = 0x00;
      out.data[2] = 0x00;
      out.data[3] = 0x00;
      response = out.to_can();
      return true;
    break;

    case Obd2::kPidVin:
    vinNum.length = vin_number_.length();
    std::memcpy(vinNum.data.data(), vin_number_.c_str(), vin_number_.length());
    current_isotp_ = vinNum;
    return current_isotp_.get_next_tx_frame(response);
  }
  return false;
}

