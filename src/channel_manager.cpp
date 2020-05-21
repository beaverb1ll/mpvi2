#include "channel_manager.hpp"

std::string ChannelManager::to_string(const MsgInterval interval) {
  switch(interval) {
    case kMsgInterval10ms:
      return "10ms (100Hz)";
    case kMsgInterval20ms:
      return "20ms (50Hz)";
    case kMsgInterval50ms:
      return "50ms (20Hz)";
    case kMsgInterval100ms:
      return "100ms (10Hz)";
    case kMsgInterval200ms:
      return "200ms (5Hz)";
    case kMsgInterval500ms:
      return "500ms (2Hz)";
    case kMsgInterval1000ms:
      return "1s (1Hz)";
    case kMsgInterval2000ms:
      return "2s (0.5Hz)";
    case kMsgInterval5000ms:
      return "5s (0.2Hz)";
    case kMsgInterval10000ms:
      return "10s (0.1Hz)";
    case kMsgIntervalOff:
      return "Off";
    default:
      break;
  }
  return "";
}

void ChannelManager::add_channel(const uint32_t ecu_id, const Obd2::Service01Pid pid, const int index) {
  constexpr const auto default_rate = ChannelManager::kMsgInterval1000ms;

  // dont allow duplicate ecu/pid items
  for(const auto &existing : channels_monitored_) {
    if(existing.ecu_id == ecu_id && existing.pid == pid) {
      return;
    }
  }

  const bool is_supported = channels_[ecu_id].is_pid_supported(pid);
  Channel &channel = channels_[ecu_id].get_channel(pid);

  const auto setting = channels_monitored_.insert(
      channels_monitored_.begin() + index,
      {is_supported, ecu_id, pid, default_rate, channel});
  rates_[default_rate].emplace_back(*setting);
}

std::string ChannelManager::name_for_index(const int index) {
  if(index >= channels_monitored_.size()) {
    return "";
  }
  return Obd2::service_01_to_string(channels_monitored_[index].pid);
}

ChannelManager::MsgInterval ChannelManager::rate_for_index(const int index) {
  if(index >= channels_monitored_.size()) {
    return ChannelManager::kMsgIntervalOff;
  }
  return channels_monitored_[index].rate;
}

double ChannelManager::value_for_index(const int index) {
  if(index >= channels_monitored_.size()) {
    return 0;
  }
  const auto &info = channels_monitored_[index];
  return channels_[info.ecu_id].get_channel(info.pid).get_value();
}

void ChannelManager::process_rate(const MsgInterval interval,
    std::function<void(const uint32_t, const Obd2::Service01Pid)> f) {
  const auto &setting_list = rates_[interval];
  for(const auto setting : setting_list) {
    f(setting.get().ecu_id, setting.get().pid);
  }
}

void ChannelManager::set_rate_for_index(const int index, const ChannelManager::MsgInterval rate) {
  if(index >= channels_monitored_.size()) {
    return;
  }
  const auto old_rate = channels_monitored_[index].rate;
  rates_[old_rate].remove_if([this, index](const std::reference_wrapper<ChannelSettings> &val){
      return val.get() == channels_monitored_[index];
  });
  rates_[rate].emplace_back(channels_monitored_[index]);
  channels_monitored_[index].rate = rate;
}

void ChannelManager::update_value(const Obd2::Obd2Msg &msg) {
  const double val = Obd2::decode_value(msg);
  const uint32_t ecu_id = msg.can_id - Obd2::kObd2RequestResponseAddressOffset;
  channels_[ecu_id].update_value(static_cast<Obd2::Service01Pid>(msg.pid), val);
}

std::unordered_map<uint32_t, Channels>& ChannelManager::get_channels() {
  return channels_;
}

Channels& ChannelManager::get_channels(const uint32_t ecu_id) {
  return channels_[ecu_id];
}
