#ifndef OBD2_PIDS_HPP_
#define OBD2_PIDS_HPP_

#include <array>
#include <bitset>
#include <cstdint>
#include <cstring>
#include <limits>
#include <vector>

#include "can_msg.hpp"

namespace Obd2 {
static constexpr const uint16_t kObd2RequestResponseAddressOffset = 8;
static constexpr const uint16_t kEcuBroadcastAddress = 0x07DF;
static constexpr const uint16_t kEcuStartAddress = 0x07E0;
static constexpr const uint16_t kEcuResponseAddress = kEcuStartAddress + kObd2RequestResponseAddressOffset;
static constexpr const uint8_t kServiceResponseOffset = 0x40;

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
  kNumServices
};

enum Service01Pid : uint8_t {
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

enum Service09Pid : uint8_t {
  kPid09Supported01To20 = 0x0,
  kPidVinMessageCount,
  kPidVin,
  kPidCalibrationIdMsgCount,
  kPidCalibrationId,
  kPidCvnMsgCount,
  kPidCvn,
  kPidasdfasfasf,
  kPidasfasdfasfg,
  kPidEcuNameCount,
  kPidEcuName,
  kPidadsfadsfasfsaf,
  kNumService09Pids
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
inline std::string service_01_to_string(const uint8_t pid) {
  static const std::array<std::string, kNumService01Pids> names{
    "Supported01To20",
    "Number of Emission Related DTC",
    "kPidFreezeDtc",
    "Fuel System #1 Status",
    "Calculated Engine Load",
    "Engine Coolant Temp",
    "Short Term Fuel Trim Bank 1",
    "Long Term Fuel Trim Bank 1",
    "Short Term Fuel Trim Bank 2",
    "Long Term Fuel Trim Bank 2",
    "Fuel Pressure",
    "Intake Manifold Absolute Pressure",
    "Engine Rpm",
    "Vehicle Speed",
    "Timing Advance",
    "Intake Air Temp",
    "Mass AirFlow",
    "Throttle Position",
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


struct Obd2Msg {
  static constexpr const uint8_t kMaxObd2DataBytes = 4;
  static constexpr const uint8_t kNumBytesOffset = 0;
  static constexpr const uint8_t kServiceOffset = 1;
  static constexpr const uint8_t kPidOffset = 2;
  static constexpr const uint8_t kDataOffset = 3;

  uint32_t can_id;
  uint8_t num_bytes;
  uint8_t service;
  uint8_t pid = std::numeric_limits<uint8_t>::max();
  std::array<uint8_t, kMaxObd2DataBytes> data;

  Obd2Msg(const uint8_t pid_in) : pid(pid_in){};
  Obd2Msg() {};

  bool encode_value(const double &value);

  bool operator==(const Obd2Msg &other) const;

  std::string to_string() const;

  static bool parse_response(const CanMsg &in, Obd2Msg &out);

  CanMsg to_can();


  std::string pid_name() {
    switch(service) {
      case kService01CurrentData:
        return service_01_to_string(pid);
      default:
        return "";
    }
  }
};

CanMsg create_obd2_query(const uint16_t address, const Services mode, const uint8_t pid );

std::bitset<32> supported_pids(const Obd2Msg &msg);



double decode_current_data_pid(const Obd2Msg &msg);

double decode_value(const Obd2Msg &msg);

} // namespace Obd2

#endif
