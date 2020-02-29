#ifndef OBD2_PIDS_HPP_
#define OBD2_PIDS_HPP_

#include <array>
#include <bitset>
#include <cstdint>
#include <vector>

#include "can_msg.hpp"

namespace Obd2 {
constexpr const uint16_t kObd2RequestResponseAddressOffset = 8;
constexpr const uint16_t kBroadcastAddress = 0x07DF;
constexpr const uint16_t kEcuRequestAddress = 0x07E0;
constexpr const uint16_t kEcuResponseAddress = kEcuRequestAddress + kObd2RequestResponseAddressOffset;
constexpr const uint8_t kServiceResponseOffset = 0x40;

enum Services : uint8_t {
  kService01CurrentData = 0x1,
  kService02FreezeFrameData,
  kService03StoredDtc,
  kService04ClearDtc,
  kService05TestResultsNonCan,
  kService06TestResults,
  kService07PendingDtc,
  kService08ControlOperations,
  kService09RequestInfo,
  kService0APermanentDtc,
};

enum Service01Pid : uint8_t{
  kPidSupported01To20 = 0x0,
  kPidMonitorStatus,
  kPidFreezeDtc,
  kPidFuelSystemStatus,
  kPidEngineLoad,
  kPidEngineCoolantTemp,
  kPidShortTermFuelPercentBank1,
  kPidLongTermFuelPercentBank1,
  kPidShortTermFuelPercentBank2,
  kPidLongTermFuelPercentBank2,
  kPidFuelPressure,
  kPidIntakeManifoldPressureAbs,
  kPidEngineRpm,
  kPidVehicleSpeed,
  kPidTimingAdvance,
  kPidIntakeAirTemp,
  kPidMafAirFlow,
  kPidThrottlePosition,
  kPidCommandedSecondaryAir,
  kPidOxygenSensorPresent,
  kPidOxygenSensor1,
  kPidOxygenSensor2,
  kPidOxygenSensor3,
  kPidOxygenSensor4,
  kPidOxygenSensor5,
  kPidOxygenSensor6,
  kPidOxygenSensor7,
  kPidOxygenSensor8,
  kPidObdStandard,
  kPidOxygenSensorPresent4Banks,
  kPidAuxInputStatus,
  kPidEngineRunTime,
  kPidSupported21To40,
  kPidDistanceWithMil,
  kPidFuelRailPressure,
  kPidFuelRailGaugePressure,
  kPidOxygenSensor1B,
  kPidOxygenSensor2B,
  kPidOxygenSensor3B,
  kPidOxygenSensor4B,
  kPidOxygenSensor5B,
  kPidOxygenSensor6B,
  kPidOxygenSensor7B,
  kPidOxygenSensor8B,
  kPidCommandedEgr,
  kPidEgrError,
  kPidCommandedEvapPurge,
  kPidFuelTankLevel,
  kPidWarmUpsSinceCodeClear,
  kPidDistanceSinceCodeClear,
  kPidEvapSysVaporPressure,
  kPidAbsoluteBaroPressure,
  kPidOxygenSensor1C,
  kPidOxygenSensor2C,
  kPidOxygenSensor3C,
  kPidOxygenSensor4C,
  kPidOxygenSensor5C,
  kPidOxygenSensor6C,
  kPidOxygenSensor7C,
  kPidOxygenSensor8C,
  kPidCatTempBank1Sens1,
  kPidCatTempBank2Sens1,
  kPidCatTempBank1Sens2,
  kPidCatTempBank2Sens2,
  kPidSupported41To60,
  kPidMonStatusDriveCycle,
  kPidControlModuleVoltage,
  kPidAbsLoadValue,
  kPidFuelAirCmdEquiv,
  kPidRelThrottlePos,
  kPidAmbientAirTemp,
  kPidAbsThrottlePosB,
  kPidAbsThrottlePosC,
  kPidAbsThrottlePosD,
  kPidAbsThrottlePosE,
  kPidAbsThrottlePosF,
  kPidCmdThrottleAct,
  kPidTimeRunMilOn,
  kPidTimeSinceCodeClear,
  kPidMaxValuesMultiple,
  kPidMaxAirFlowMAF,
  kPidFuelType,
  kPidEthanolFuelPct,
  kPidAbsEvapSysVapPres,
  kPidEvapSysVapPres,
  kPidasdf,
  kPidasdfg,
  kPidasdfgh,
  kPidadsfghj,
  kPidFuelRailAbsPres,
  kPidRelAccelPedalPos,
  kPidHybrBatPackRemLife,
  kPidEngineOilTemp,
  kPidFuelInjectionTiming,
  kPidEngineFuelRate,
  kPidEmissionReq,
  kPidSupported61To80,
  kPidDemandEngTorqPct,
  kPidActualEngTorqPct,
  kPidEngineRefTorq,
  kPidEngPctTorq,
  kPidAuxInputOutput,
  kPidMafSensor,
  kPidEngineCoolantTempB,
  kPidIntakeAirTempB,
  kPidCmdEgrEgrError,
  kPidCmdDieselIntAir,
  kPidExhaustGasTemp,
  kPidCmdThrottleActRel,
  kPidFuelPresContrSys,
  kPidInjPresContrSys,
  kPidTurboComprPressure,
  kPidBoostPressureCntrl,
  kNumService01Pids
};

inline std::string to_string(const Services service) {
  static const std::array<std::string, kService0APermanentDtc> names{
  "Current",
  "Freeze Frame",
  "Stored DTCs",
  "Clear DTCs",
  "",
  "",
  "Pending DTCs",
  "Vehicle Control",
  "Vehicle Information",
  "Permanent DTCs"
  };
  const int offset = service - 1;
  return names[offset];
}
;

struct Obd2Msg {
  uint32_t can_id;
  uint8_t num_bytes;
  uint8_t service;
  uint8_t pid;
  std::array<uint8_t, 4> data;

  bool operator==(const Obd2Msg &other) const {
    if(this->can_id != other.can_id ||
        this->num_bytes != other.num_bytes ||
        this->service != other.service ||
        this->pid != other.pid ||
        this->data != other.data) {
      return false;
    }
    return true;
  }

  static bool parse_response(const CanMsg &in, Obd2Msg &out) {
    if(in.length < 4) {
      return false;
    }
    out.can_id = in.id;
    out.num_bytes = in.data[0]-2;
    out.service = in.data[1];
    out.pid = in.data[2];

    if(out.num_bytes > 4) {
      return false;
    }
    if(out.num_bytes + 3 > in.data.size()) {
      return false;
    }
    for(int i = 0; i < out.num_bytes; i++) {
      out.data[i] = in.data[i+3];
    }
    for(int i = out.num_bytes; i < 4; i++) {
      out.data[i] = 0x00;
    }
    return true;
  }

  CanMsg get_can() {
    CanMsg out;
    out.id = can_id;
    out.length = 8;
    out.data[0] = num_bytes+2;
    out.data[1] = service;
    out.data[2] = pid;
    for(int i = 0; i < num_bytes; i++) {
      out.data[3+i] = data[i];
    }
    for(int i = num_bytes; i < 4; i++) {
      out.data[3+i] = 0;
    }
    return out;
  }


  std::string pid_name() {
    static const std::array<std::string, kNumService01Pids> names{
    "kPidSupported01To20",
    "kPidMonitorStatus",
    "kPidFreezeDtc",
    "kPidFuelSystemStatus",
    "kPidEngineLoad",
    "kPidEngineCoolantTemp",
    "kPidShortTermFuelPercentBank1",
    "kPidLongTermFuelPercentBank1",
    "kPidShortTermFuelPercentBank2",
    "kPidLongTermFuelPercentBank2",
    "kPidFuelPressure",
    "kPidIntakeManifoldPressureAbs",
    "Engine Rpm",
    "Vehicle Speed",
    "kPidTimingAdvance",
    "kPidIntakeAirTemp",
    "kPidMafAirFlow",
    "kPidThrottlePosition",
    "kPidCommandedSecondaryAir",
    "kPidOxygenSensorPresent",
    "kPidOxygenSensor1",
    "kPidOxygenSensor2",
    "kPidOxygenSensor3",
    "kPidOxygenSensor4",
    "kPidOxygenSensor5",
    "kPidOxygenSensor6",
    "kPidOxygenSensor7",
    "kPidOxygenSensor8",
    "kPidObdStandard",
    "kPidOxygenSensorPresent4Banks",
    "kPidAuxInputStatus",
    "kPidEngineRunTime",
    "kPidSupported21To40",
    "kPidDistanceWithMil",
    "kPidFuelRailPressure",
    "kPidFuelRailGaugePressure",
    "kPidOxygenSensor1B",
    "kPidOxygenSensor2B",
    "kPidOxygenSensor3B",
    "kPidOxygenSensor4B",
    "kPidOxygenSensor5B",
    "kPidOxygenSensor6B",
    "kPidOxygenSensor7B",
    "kPidOxygenSensor8B",
    "kPidCommandedEgr",
    "kPidEgrError",
    "kPidCommandedEvapPurge",
    "kPidFuelTankLevel",
    "kPidWarmUpsSinceCodeClear",
    "kPidDistanceSinceCodeClear",
    "kPidEvapSysVaporPressure",
    "kPidAbsoluteBaroPressure",
    "kPidOxygenSensor1C",
    "kPidOxygenSensor2C",
    "kPidOxygenSensor3C",
    "kPidOxygenSensor4C",
    "kPidOxygenSensor5C",
    "kPidOxygenSensor6C",
    "kPidOxygenSensor7C",
    "kPidOxygenSensor8C",
    "kPidCatTempBank1Sens1",
    "kPidCatTempBank2Sens1",
    "kPidCatTempBank1Sens2",
    "kPidCatTempBank2Sens2",
    "kPidSupported41To60",
    "kPidMonStatusDriveCycle",
    "kPidControlModuleVoltage",
    "kPidAbsLoadValue",
    "kPidFuelAirCmdEquiv",
    "kPidRelThrottlePos",
    "Ambient Air Temperature",
    "kPidAbsThrottlePosB",
    "kPidAbsThrottlePosC",
    "kPidAbsThrottlePosD",
    "kPidAbsThrottlePosE",
    "kPidAbsThrottlePosF",
    "kPidCmdThrottleAct",
    "kPidTimeRunMilOn",
    "kPidTimeSinceCodeClear",
    "kPidMaxValuesMultiple",
    "kPidMaxAirFlowMAF",
    "kPidFuelType",
    "kPidEthanolFuelPct",
    "kPidAbsEvapSysVapPres",
    "kPidEvapSysVapPres",
    "kPidasdf",
    "kPidasdfg",
    "kPidasdfgh",
    "kPidadsfghj",
    "kPidFuelRailAbsPres",
    "kPidRelAccelPedalPos",
    "kPidHybrBatPackRemLife",
    "kPidEngineOilTemp",
    "kPidFuelInjectionTiming",
    "kPidEngineFuelRate",
    "kPidEmissionReq",
    "kPidSupported61To80",
    "kPidDemandEngTorqPct",
    "kPidActualEngTorqPct",
    "kPidEngineRefTorq",
    "kPidEngPctTorq",
    "kPidAuxInputOutput",
    "kPidMafSensor",
    "kPidEngineCoolantTempB",
    "kPidIntakeAirTempB",
    "kPidCmdEgrEgrError",
    "kPidCmdDieselIntAir",
    "kPidExhaustGasTemp",
    "kPidCmdThrottleActRel",
    "kPidFuelPresContrSys",
    "kPidInjPresContrSys",
    "kPidTurboComprPressure",
    "kPidBoostPressureCntrl",
    };
    return names[pid];
  }
};

struct Dtc {
  enum Type : uint8_t {
    kTypeStored = 0,
    kTypePending,
    kTypeCleared
  };
  Type type;
  char letter;
  uint16_t code;

  Dtc(const Dtc::Type t, const uint16_t raw) {
    type = t;
    const uint8_t letterCode = raw >> 14;
    switch(letterCode) {
      case 0: letter = 'P'; break;
      case 1: letter = 'C'; break;
      case 2: letter = 'B'; break;
      case 3: letter = 'U'; break;
    }
    code = raw & 0x3FFF;
  }

  bool operator==(const Dtc &other) const {
    return type == other.type && letter == other.letter && code == other.code;
  }
};

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

//bool encode_data_pid(const uint8_t pid, const double value, std::array<uint8_t, 4> &data) {
//  switch(pid) {
//
//    case kPidEngineLoad:
//      data[0] = value * 2.55;
//      return true;
//
//    case kPidEngineCoolantTemp:
//      data[0] = value + 40.0;
//      return true;
//
//    case kPidShortTermFuelPercentBank1:
//      // fallthrough
//    case kPidLongTermFuelPercentBank1:
//      // fallthrough
//    case kPidShortTermFuelPercentBank2:
//      // fallthrough
//    case kPidLongTermFuelPercentBank2:
//       (msg.data[0] / 1.28) - 100;
//      return true;
//
//    case kPidFuelPressure:
//      return msg.data[0] * 3.0;
//
//    case kPidIntakeManifoldPressureAbs:
//      return msg.data[0];
//
//    case kPidEngineRpm:
//      return ((msg.data[0] << 8) | msg.data[1])/4.0;
//
//    case kPidVehicleSpeed:
//      return msg.data[0];
//
//    case kPidTimingAdvance:
//      return (msg.data[0]/2.0) - 64;
//
//    case kPidIntakeAirTemp:
//      return msg.data[0] - 40.0;
//
//    case kPidMafAirFlow:
//      return ((msg.data[0] << 8) | msg.data[1])/100.0;
//
//    case kPidThrottlePosition:
//      return (msg.data[0]/255) * 100.0;
//
//    case kPidCommandedSecondaryAir:
//      return 0.0;
//
//    case kPidOxygenSensorPresent:
//      return 0.0;
//
//    case kPidOxygenSensor1:
//      // fallthrough
//    case kPidOxygenSensor2:
//      // fallthrough
//    case kPidOxygenSensor3:
//      // fallthrough
//    case kPidOxygenSensor4:
//      // fallthrough
//    case kPidOxygenSensor5:
//      // fallthrough
//    case kPidOxygenSensor6:
//      // fallthrough
//    case kPidOxygenSensor7:
//      // fallthrough
//    case kPidOxygenSensor8:
//      return 0.0;
//
//    case kPidAmbientAirTemp:
//      return msg.data[0] - 40.0;
//
//
//
//
//    default:
//      printf("No Obd pid decode\n");
//      break;
//  }
//  return 0;
//}


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

} // namespace Obd2

#endif
