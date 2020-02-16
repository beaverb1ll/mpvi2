#ifndef OBD2_PIDS_HPP_
#define OBD2_PIDS_HPP_

namespace Obd2 {
enum Modes {
  kModeCurrentData = 0x1,
  kModeFreezeFrameData,
  kModeStoredDtc,
  kModeClearDtc,
  kModeTestResultsNonCan,
  kModeTestResults,
  kModePendingDtc,
  kModeControlOperations,
  kModeRequestInfo,
  kModePermanentDtc,
  kNumModes
};

enum Mode01Pid {
  kPidsSupported01To20 = 0x0,
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
};

} // namespace Obd2

#endif
