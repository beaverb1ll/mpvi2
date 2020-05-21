#ifndef CHANNELS_HPP_
#define CHANNELS_HPP_

#include <unordered_map>

#include "channel.hpp"
#include "obd2/obd2.hpp"

class Channels {
 public:

   bool set_supported_pids(const Obd2::Obd2Msg &in_msg, CanMsg &out_msg);
  std::vector<Obd2::Service01Pid> get_supported_pids() const;
  bool is_pid_supported(const Obd2::Service01Pid pid);
  bool is_complete();

  void update_value(const Obd2::Service01Pid pid, const double value);
  Channel& get_channel(const Obd2::Service01Pid pid);

 private:
  std::bitset<0xE0> supported_obd2_pids_;

  std::unordered_map<Obd2::Service01Pid, Channel> channels_;

};

#endif
