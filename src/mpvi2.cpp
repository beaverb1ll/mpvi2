#include "mpvi2.hpp"

#include <algorithm>
#include <stdexcept>

#define CRCPP_USE_CPP11
#include "crc.hpp"
#undef CRCPP_USE_CPP11

#include "ftdi_serial_port.hpp"

const std::string Mpvi2::kSerialNumber{"MPVI00020"};
const uint8_t Mpvi2::kEofByte = 0x0;

Mpvi2::Mpvi2() : Mpvi2(std::make_shared<FtdiSerialPort>(kSerialNumber)) {
}

Mpvi2::Mpvi2(std::shared_ptr<SerialPort> serial) : serial_(serial) {

  if (!serial_->reset()) {
    printf("Unable to reset\n");
  }

  if(!serial_->flush()) {
    printf("Unable to flush\n");
  }

  if (!serial_->set_baud_rate(3000000)) {
    printf("Unable to set baud\n");
  }

  if(!serial_->set_framing(SerialPort::kDataBits8, SerialPort::kDataParityNone, SerialPort::kDataStopBits1)) {
    printf("Unable to set data framing\n");
  }

  if (!serial_->set_flow_control(SerialPort::kDataFlowControlRtsCts)) {
    printf("Unable to set flow control\n");
  }

  decode_thread_ = std::thread([this](){
    pthread_setname_np(pthread_self(), "decode_thread");
    started_ = true;
    read_and_decode();
  });
  while(!started_) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

//  device_id_ = read_device_id();
//  part_number_ = read_part_number();
//  read_hardware_version(hardware_version_major_, hardware_version_minor_, hardware_version_subminor_);
//
  serial_->write({0x0});
//  serial_->write({0x02, 0x01, 0x02, 0x01, 0x04, 0x10, 0x52, 0xE7, 0x00});
}

Mpvi2::~Mpvi2() {
  kill();
  if(decode_thread_.joinable()) {
    decode_thread_.join();
  }
}

void Mpvi2::kill() {
  keep_running_ = false;
  can_msgs_cv_.notify_all();
}

void Mpvi2::read_and_decode() {
  std::vector<uint8_t> data;
  std::vector<uint8_t> command;
  std::vector<uint8_t> temp;
  while(keep_running_) {

    serial_->wait_for_rx(std::chrono::milliseconds(2000));
    const auto rx_bytes = serial_->get_num_rx_bytes();
    if(rx_bytes < 1) {
      continue;
    }
    if(!serial_->read(temp, rx_bytes)) {
      printf("read failed\n");
      continue;
    }

    for(int i = 0; i < rx_bytes; i++) {
      data.insert(data.end(), temp[i]);
      if(data.back() == kEofByte) {
        restore_zeros(data, command);
        data.clear();
        decode(command);
      }
    }
  }
}

void Mpvi2::decode(const std::vector<uint8_t> &command) {
  const uint16_t type = (command[0] << 8) | command[1];
  switch(type) {
    case 0x01: // internal
      printf("Got internal response\n");
      break;
    case 0x1C: // CAN
      decode_can(command);
      break;

    default:
      return;
  }
}

void Mpvi2::decode_can(const std::vector<uint8_t> &command) {
  CanMsg msg;
  msg.length = 8;
  msg.id = (command[3] << 8) | command[4];
  std::copy(command.begin() + 5, command.end() - 3, msg.data.begin());
  std::unique_lock<std::mutex> lk(can_msgs_mutex_);
  can_msgs_.push_back(msg);
  lk.unlock();
  can_msgs_cv_.notify_one();
}

bool Mpvi2::write(const std::vector<uint8_t> &data) {
  std::lock_guard<std::mutex> lk(tx_mutex_);
  return serial_->write(data);
}

bool Mpvi2::read(std::vector<uint8_t> &data, const uint8_t num_bytes) {
  return serial_->read(data, num_bytes);
}

void Mpvi2::remove_zeros(const std::vector<uint8_t> &in, std::vector<uint8_t> &out) {
  out = in;
  int num_valid = 1;
  out[in.size()-1] = 0x0;
  bool is_within_zero_block = false;
  for(int i = in.size()-2; i >= 0; i--) {
    if(in[i] == 0x00) {
      if(is_within_zero_block) {
        out[i] = 0x01;
      } else {
        out[i] = num_valid;
        num_valid = 1;
        is_within_zero_block = true;
      }
    } else {
      is_within_zero_block = false;
      num_valid++;
    }
  }
}

void Mpvi2::restore_zeros(const std::vector<uint8_t> &in, std::vector<uint8_t> &out) {
  out = in;
  for(int i = 0; i < in.size(); i++) {
    if(in[i] == kEofByte) {
      continue;
    }
    out[i] = 0x0;
    if(in[i] != 0x1) {
      i+=(in[i]-1);
    }
  }
}

bool Mpvi2::read_can(CanMsg &msg, const std::chrono::milliseconds &timeout) {
  std::unique_lock<std::mutex> lk(can_msgs_mutex_);
  if(can_msgs_.size() < 1) {
    can_msgs_cv_.wait_for(lk, timeout, [this]{return can_msgs_.size() > 0;});
  }
  if(can_msgs_.size() < 1) {
    return false;
  }
  msg = can_msgs_.front();
  can_msgs_.pop_front();
  return true;
}

uint32_t Mpvi2::send_command(const std::vector<uint8_t> &command,
  const uint32_t response_size, std::vector<uint8_t> &response) {
  std::vector<uint8_t> temp;
  remove_zeros(command, temp);
  serial_->flush();
  if(!write(temp)) {
    return false;
  }

  if(!serial_->wait_for_bytes(response_size)) {
    return false;
  }

  if(!read(temp, response_size)) {
    return false;
  }
  restore_zeros(temp, response);
  return true;
}

uint32_t Mpvi2::get_device_id() {
  return device_id_;
}

uint32_t Mpvi2::read_device_id() {
  const std::vector<uint8_t> command{0x00, 0x01, 0x00, 0x01, 0x00, 0x14, 0x12, 0x63, 0x00};
  std::vector<uint8_t> response;

  uint32_t dev_id = 0;
  if(send_command(command, 14, response)) {
    dev_id = (response[7] << 24) | (response[8] << 16) | (response[9] << 8) | (response[10]);
  }
  return dev_id;
}

uint16_t Mpvi2::get_part_number() {
  return part_number_;
}
uint16_t Mpvi2::read_part_number() {
  const std::vector<uint8_t> command{0x00, 0x01, 0x00, 0x01, 0x00, 0x15, 0x02, 0x42, 0x00};

  std::vector<uint8_t> response;
  uint16_t part_num = 0;

  if(send_command(command, 14, response)) {
    part_num = (response[9] << 8) | (response[10]);
  }
  return part_num;
}

uint32_t Mpvi2::get_power_on_time() {
  const std::vector<uint8_t> command{0x00, 0x01, 0x00, 0x01, 0x00, 0x16, 0x32, 0x21, 0x00};

  std::vector<uint8_t> response;
  uint32_t p_on_time = 0;
  //if(send_command(command, 14, response)) {
  //  p_on_time = (response[7] << 24) | (response[8] << 16) | (response[9] << 8) | (response[10]);
  //}
  return p_on_time;
}

uint32_t Mpvi2::get_vehicle_connected_time() {
  const std::vector<uint8_t> command{0x00, 0x01, 0x00, 0x01, 0x04, 0x00, 0x8C, 0x12, 0x00};

  std::vector<uint8_t> response;
  uint32_t v_conn_time = 0;
  //if(send_command(command, 14, response)) {
  //  v_conn_time = (response[7] << 24) | (response[8] << 16) | (response[9] << 8) | (response[10]);
  //}
  return v_conn_time;
}

bool Mpvi2::get_hardware_version(uint16_t &major, uint16_t &minor, uint16_t &subminor) {
  major = hardware_version_major_;
  minor = hardware_version_minor_;
  subminor = hardware_version_subminor_;
  return true;
}

bool Mpvi2::read_hardware_version(uint16_t &major, uint16_t &minor, uint16_t &subminor) {
  const std::vector<uint8_t> command{0x00, 0x01, 0x00, 0x01, 0x00, 0x23, 0x54, 0xD7, 0x00};

  std::vector<uint8_t> response;
  if(send_command(command, 14, response)) {
    major = response[7];
    minor = response[8];
    subminor = (response[9] << 8) | response[10];
    return true;
  }
  return false;
}

bool Mpvi2::write_can(const CanMsg &msg) {

  std::vector<uint8_t> input{0x00, 0x1C};
  const uint8_t *id_8 = reinterpret_cast<const uint8_t*>(&msg.id);
  input.push_back(id_8[3]);
  input.push_back(id_8[2]);
  input.push_back(id_8[1]);
  input.push_back(id_8[0]);

  for(int i = 0; i < msg.length; i++) {
    input.push_back(msg.data[i]);
  }

  const uint16_t chksum = calculate_crc(input);
  input.push_back((chksum >> 0x8) & 0xFF);
  input.push_back(chksum & 0xFF);

  input.push_back(kEofByte);

  std::vector<uint8_t> output;
  remove_zeros(input, output);

  return write(output);
}

uint16_t Mpvi2::calculate_crc(const std::vector<uint8_t> &in) {
  // ./reveng -w 16 -s 001C000007DF0201000000000000930E 001C000007DF0201200000000000A606 001C000007DF0201400000000000F91E 001C000007DF0201600000000000CC16
  // width=16  poly=0x1021  init=0x8122 check=0x919c
  static const CRC::Parameters<crcpp_uint16, 16> params{ 0x1021, 0x8122, 0x0, false, false };
  static const auto table = params.MakeTable();
  return CRC::Calculate(in.data(), in.size(), table);
}
