
#ifndef MPVI2_HPP_
#define MPVI2_HPP_

#include <atomic>
#include <condition_variable>
#include <list>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include "serial_port.hpp"
#include "obd2/can_msg.hpp"

class Mpvi2 {
 public:
  static const std::string kSerialNumber;
  static const uint8_t kEofByte;

  Mpvi2();
  Mpvi2(std::shared_ptr<SerialPort> serial);
  ~Mpvi2();

  uint32_t get_device_id();
  uint16_t get_part_number();
  uint32_t get_power_on_time();
  uint32_t get_vehicle_connected_time();
  bool get_hardware_version(uint16_t &major, uint16_t &minor, uint16_t &subminor);
  bool send_can(const CanMsg &msg);

  template< class Rep, class Period >
  bool get_next_can_msg(CanMsg &msg, const std::chrono::duration<Rep, Period>& rel_time);

  bool get_next_can_msg(CanMsg &msg);
  void kill();

 private:

  static void remove_zeros(const std::vector<uint8_t> &in, std::vector<uint8_t> &out);
  static void restore_zeros(const std::vector<uint8_t> &in, std::vector<uint8_t> &out);
  static uint16_t calculate_crc(const std::vector<uint8_t> &in);

  void read_and_decode();
  bool write(const std::vector<uint8_t> &data);
  bool read(std::vector<uint8_t> &data, const uint8_t num_bytes);
  uint32_t send_command(const std::vector<uint8_t> &command,
      const uint32_t response_size, std::vector<uint8_t> &response);

  uint32_t read_device_id();
  uint16_t read_part_number();
  bool read_hardware_version(uint16_t &major, uint16_t &minor, uint16_t &subminor);

  void decode(const std::vector<uint8_t> &command);
  void decode_can(const std::vector<uint8_t> &command);

  std::atomic<bool> keep_running_{true};
  std::atomic<bool> started_{false};
  std::shared_ptr<SerialPort> serial_;
  std::list<CanMsg> can_msgs_;
  std::mutex can_msgs_mutex_;
  std::condition_variable can_msgs_cv_;

  std::thread decode_thread_;
  std::mutex tx_mutex_;

  uint32_t device_id_;
  uint16_t part_number_;

  uint16_t hardware_version_major_;
  uint16_t hardware_version_minor_;
  uint16_t hardware_version_subminor_;
};

template< class Rep, class Period >
bool Mpvi2::get_next_can_msg(CanMsg &msg, const std::chrono::duration<Rep, Period>& rel_time) {
  std::unique_lock<std::mutex> lk(can_msgs_mutex_);
  if(can_msgs_.size() < 1) {
    can_msgs_cv_.wait_for(lk, rel_time, [this]{return can_msgs_.size() > 0;});
  }
  if(can_msgs_.size() < 1) {
    return false;
  }
  msg = can_msgs_.front();
  can_msgs_.pop_front();
  return true;
}


#endif
