#ifndef CHANNEL_MANAGER_HPP
#define CHANNEL_MANAGER_HPP

#include <deque>
#include <functional>

#include "channel.hpp"
#include "channels.hpp"

class ChannelManager {
 public:
  enum MsgInterval {
    kMsgInterval10ms = 0,
    kMsgInterval20ms,
    kMsgInterval50ms,
    kMsgInterval100ms,
    kMsgInterval200ms,
    kMsgInterval500ms,
    kMsgInterval1000ms,
    kMsgInterval2000ms,
    kMsgInterval5000ms,
    kMsgInterval10000ms,
    kMsgIntervalOff,
    kNumMsgInterval
  };
  static std::string to_string(const MsgInterval interval);

  ChannelManager() = default;

  void add_channel(const uint32_t ecu_id, const Obd2::Service01Pid pid, const int index = 0);
  std::string name_for_index(const int index);
  MsgInterval rate_for_index(const int index);
  double value_for_index(const int index);
  int num_rows() const {return channels_monitored_.size();}
  void set_rate_for_index(const int index, const ChannelManager::MsgInterval rate);

  void process_rate(const MsgInterval interval, std::function<void(const uint32_t, const Obd2::Service01Pid)> f);

  void update_value(const Obd2::Obd2Msg &msg);

  std::unordered_map<uint32_t, Channels>& get_channels();
  Channels& get_channels(const uint32_t ecu_id);

 private:
  struct ChannelSettings {
    bool enabled;
    uint32_t ecu_id;
    Obd2::Service01Pid pid;
    MsgInterval rate;
    std::reference_wrapper<Channel> channel;

    bool operator==(const ChannelSettings &other) {
      return ecu_id == other.ecu_id && pid == other.pid;
    }
  };
  // keep settings and order in which channels are displayed
  std::deque<ChannelSettings> channels_monitored_;

  // holds all channels data
  std::unordered_map<uint32_t, Channels> channels_;

  // keep simple way to lookup channel based upon rate
  std::unordered_map<MsgInterval, std::list<std::reference_wrapper<ChannelSettings>>> rates_;
};

#endif
