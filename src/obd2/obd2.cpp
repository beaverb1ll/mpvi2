#include "obd2/obd2.hpp"

namespace Obd2 {

bool Obd2Msg::operator==(const Obd2Msg &other) const {
  if(this->can_id != other.can_id ||
      this->num_bytes != other.num_bytes ||
      this->service != other.service ||
      this->pid != other.pid ||
      this->data != other.data) {
    return false;
  }
  return true;
}

std::string Obd2Msg::to_string() const {
  // xxx#ss#pp[num]yy yy yy yy
  std::string out;
  out.reserve(35);
  char temp[25];
  temp[24] = '\0';

  snprintf(temp, 24, "%03X#%02X#%02X", can_id, service, pid);
  out+=temp;
  snprintf(temp, 24, " [%d] ", num_bytes);
  out+=temp;
  for(const auto &a : data) {
    snprintf(temp, 24, "%02X ", a);
    out+=temp;
  }
  return out;
}

bool Obd2Msg::parse_response(const CanMsg &in, Obd2Msg &out) {
    if(in.length < 4) {
      printf("Length too short\n");
      return false;
    }

    const uint8_t num_bytes = in.data[kNumBytesOffset];
    out.can_id = in.id;
    out.service = in.data[kServiceOffset];

    if(num_bytes < 2) {
      out.num_bytes = 0;
      out.pid = 0;
      memset(out.data.data(), 0, kMaxObd2DataBytes);
      return true;
    }

    out.num_bytes = num_bytes-2; // don't include service and pid
    out.pid = in.data[kPidOffset];

    if(out.num_bytes > kMaxObd2DataBytes ||
       out.num_bytes + kDataOffset > in.data.size()) { // ensure num_bytes was actually provided
      printf("num_bytes error\n");
      return false;
    }
    for(int i = 0; i < out.num_bytes; i++) {
      out.data[i] = in.data[kDataOffset + i];
    }
    // Fill in non used bytes with 0x0
    for(int i = out.num_bytes; i < kMaxObd2DataBytes; i++) {
      out.data[i] = 0x00;
    }
    return true;
  }

CanMsg Obd2Msg::to_can() {
  CanMsg out;
  out.id = can_id;
  out.length = 8;
  out.data[kNumBytesOffset] = num_bytes+2;
  out.data[kServiceOffset] = service;
  out.data[kPidOffset] = pid;
  for(int i = 0; i < num_bytes; i++) {
    out.data[kDataOffset+i] = data[i];
  }
  for(int i = num_bytes; i < kMaxObd2DataBytes+1; i++) {
    out.data[kDataOffset+i] = 0x00;
  }
  return out;
}

bool Obd2Msg::encode_value(const double &value) {
  memset(data.data(), 0, sizeof(data));

  switch(pid) {
    case kPidEngineLoad:
      data[0] = value * 2.55;
      num_bytes = 1;
      return true;

    case kPidEngineCoolantTemp:
      data[0] = value + 40.0;
      num_bytes = 1;
      return true;

    case kPidShortTermFuelPercentBank1:
      // fallthrough
    case kPidLongTermFuelPercentBank1:
      // fallthrough
    case kPidShortTermFuelPercentBank2:
      // fallthrough
    case kPidLongTermFuelPercentBank2:
      data[0] = (value + 100) * 1.28;
      num_bytes = 1;
      return true;

    case kPidFuelPressure:
      data[0] = value / 3.0;
      num_bytes = 1;
      return true;

    case kPidIntakeManifoldPressureAbs:
      data[0] = value;
      num_bytes = 1;
      return true;

    case kPidEngineRpm: {
        uint16_t temp = value*4;
        data[0] = (temp >> 8);
        data[1] = (temp & 0xFF);
      }
      num_bytes = 2;
      return true;

    case kPidVehicleSpeed:
      num_bytes = 1;
      data[0] = value;
      return true;

    case kPidTimingAdvance:
      data[0] = (value + 64) *2.0;
      num_bytes = 1;
      return true;

    case kPidIntakeAirTemp:
      data[0] = value + 40.0;
      num_bytes = 1;
      return true;

    case kPidMafAirFlow: {
        uint16_t temp = value * 100.0;
        data[0] = temp >> 8;
        data[1] = temp & 0xFF;
      }
      num_bytes = 2;
      return true;

    case kPidThrottlePosition:
      data[0] = (value/100) * 255;
      num_bytes = 1;
      return true;

    case kPidCommandedSecondaryAir:
      return false;

    case kPidOxygenSensorPresent:
      return false;

    case kPidOxygenSensor1:
      // fallthrough
    case kPidOxygenSensor2:
      // fallthrough
    case kPidOxygenSensor3:
      // fallthrough
    case kPidOxygenSensor4:
      // fallthrough
    case kPidOxygenSensor5:
      // fallthrough
    case kPidOxygenSensor6:
      // fallthrough
    case kPidOxygenSensor7:
      // fallthrough
    case kPidOxygenSensor8:
      return false;

    case kPidAmbientAirTemp:
      data[0] = value + 40.0;
      num_bytes = 1;
      return true;

    default:
      printf("No Obd pid decode\n");
      break;
  }
  return false;
}

double decode_current_data_pid(const Obd2Msg &msg) {
  switch(msg.pid) {

    case kPidEngineLoad:
      return msg.data[0] / 2.55;

    case kPidEngineCoolantTemp:
      return msg.data[0] - 40.0;

    case kPidShortTermFuelPercentBank1:
      // fallthrough
    case kPidLongTermFuelPercentBank1:
      // fallthrough
    case kPidShortTermFuelPercentBank2:
      // fallthrough
    case kPidLongTermFuelPercentBank2:
      return (msg.data[0] / 1.28) - 100;

    case kPidFuelPressure:
      return msg.data[0] * 3.0;

    case kPidIntakeManifoldPressureAbs:
      return msg.data[0];

    case kPidEngineRpm:
      return ((msg.data[0] << 8) | msg.data[1])/4.0;

    case kPidVehicleSpeed:
      return msg.data[0];

    case kPidTimingAdvance:
      return (msg.data[0]/2.0) - 64;

    case kPidIntakeAirTemp:
      return msg.data[0] - 40.0;

    case kPidMafAirFlow:
      return ((msg.data[0] << 8) | msg.data[1])/100.0;

    case kPidThrottlePosition:
      return (msg.data[0]/255) * 100.0;

    case kPidCommandedSecondaryAir:
      return 0.0;

    case kPidOxygenSensorPresent:
      return 0.0;

    case kPidOxygenSensor1:
      // fallthrough
    case kPidOxygenSensor2:
      // fallthrough
    case kPidOxygenSensor3:
      // fallthrough
    case kPidOxygenSensor4:
      // fallthrough
    case kPidOxygenSensor5:
      // fallthrough
    case kPidOxygenSensor6:
      // fallthrough
    case kPidOxygenSensor7:
      // fallthrough
    case kPidOxygenSensor8:
      return 0.0;

    case kPidAmbientAirTemp:
      return msg.data[0] - 40.0;

    default:
      printf("No Obd pid decode\n");
      break;
  }
  return 0;
}

CanMsg create_obd2_query(const uint16_t address, const Services mode, const uint8_t pid ) {
  return {address, 8, {2, mode, pid, 0, 0, 0, 0, 0}};
}

std::bitset<32> supported_pids(const Obd2Msg &msg) {
  std::bitset<32> valid;
  for(int i = 0; i < 4; i++) {
    for(int j = 0; j < 8; j++) {
      const auto &byte = msg.data[i];
      const uint8_t mask = (1 << (7-j));
      const auto offset = (i*8) + j;
      valid[offset] = byte & mask;
    }
  }
  return valid;
}

double decode_value(const Obd2Msg &msg) {
  switch(msg.service) {
    case kService01CurrentData + kServiceResponseOffset:
      return decode_current_data_pid(msg);
      break;
    default:
      printf("No Obd mode decode\n");
      break;
  }
  return 0;
}


};
