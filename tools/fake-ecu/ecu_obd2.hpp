#ifndef ECU_OBD2_HPP_
#define ECU_OBD2_HPP_

#include <bitset>
#include <functional>
#include <string>
#include <unordered_map>

#include "obd2/can_msg.hpp"
#include "obd2/obd2.hpp"
#include "obd2/iso_tp.hpp"

#include "logger.hpp"
#include "utils.hpp"

// OBD2 is a command/request style interface
// this means that an outgoing OBD2 packet is only
// generated upon receipt of an incoming OBD2 packet

class EcuObd2 {

 public:
  using send_func_t = std::function<void(const CanMsg &out)>;

  EcuObd2(const uint16_t can_id);

  void set_send_function(const send_func_t &func);
  void set_vin(const std::string &vin);
  void set_pid_supported(const Obd2::Services service, const uint8_t pid, const bool enabled=true);
  void set_pid_value(const Obd2::Services service, const uint8_t pid, const double value);

  bool process_rx_packet(const CanMsg &msg);

  uint32_t get_read_count(const Obd2::Services service, const uint8_t pid);

 private:
  void send_packet(const CanMsg &out);
  bool process_obd(const CanMsg &msg);
  bool process_tp(const CanMsg &in_msg);
  bool process_service01(const Obd2::Obd2Msg &requset, CanMsg &out);
  bool process_service09(const Obd2::Obd2Msg &request, CanMsg &response);
  void send_response(const Obd2::Obd2Msg &request);
  bool encode_service_01_supported_pid(Obd2::Obd2Msg &out);

  uint16_t ecu_can_id_;
  uint16_t ecu_response_id_;
  std::bitset<Obd2::kNumService01Pids> service01_supported_pids_;
  std::unordered_map<Obd2::Service01Pid, double> service01_vals_;
  std::array<std::unordered_map<uint8_t, uint32_t>, Obd2::kNumServices> read_counters_;
  std::string vin_number_;
  send_func_t send_packet_func_;

  IsoTp::IsoTpTransmitMsg current_isotp_;
};

#endif
