#include "channels.hpp"

bool Channels::set_supported_pids(const Obd2::Obd2Msg &in_msg, CanMsg &out_msg) {
  int offset = 0;
  switch(in_msg.pid) {
    case Obd2::kPidSupported01To20:
      offset = 1;
      break;
    case Obd2::kPidSupported21To40:
      offset = 0x21;
      break;
    case Obd2::kPidSupported41To60:
      offset = 0x41;
      break;
    case Obd2::kPidSupported61To80:
      offset = 0x61;
      break;
    default:
      return false;
  }
  const auto pids = Obd2::supported_pids(in_msg);

  for(int i = 0; i < pids.size(); i++) {
    supported_obd2_pids_[offset+i] = pids[i];
  }

  const auto more = pids[pids.size()-1];
  if(more && in_msg.pid < Obd2::kPidSupported41To60) {
    out_msg = Obd2::create_obd2_query(in_msg.can_id - Obd2::kObd2RequestResponseAddressOffset, Obd2::kService01CurrentData, in_msg.pid + 0x20);
    return true;
  }
  return false;
}
bool Channels::is_complete() {
  return false;
}

std::vector<Obd2::Service01Pid> Channels::get_supported_pids() const {
  std::vector<Obd2::Service01Pid> out;
  for(int i = 0; i < supported_obd2_pids_.size(); i++) {
    if(supported_obd2_pids_[i]) {
      out.push_back(static_cast<Obd2::Service01Pid>(i));
    }
  }
  return out;
}

void Channels::update_value(const Obd2::Service01Pid pid, const double value) {
  if(supported_obd2_pids_[pid]) {
    channels_[pid].update_value(value);
  }
}

Channel& Channels::get_channel(const Obd2::Service01Pid pid) {
  return channels_[pid];
}

bool Channels::is_pid_supported(const Obd2::Service01Pid pid) {
  return supported_obd2_pids_[pid];
}
