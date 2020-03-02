#ifndef LOGGER_HPP_
#define LOGGER_HPP_

#include <bitset>
#include <string>

class Logger {
 public:
  enum LogLevel {
    kLogOff = 0,
    kLogFatal,
    kLogError,
    kLogWarn,
    kLogInfo,
    kLogDebug,
    kLogTrace,
    kNumLogLevels
  };

  static constexpr const unsigned int kMaxMsgLength = 512;
  static const std::string& get_level_name(const LogLevel &level);

  Logger();
  Logger(const std::string &level);
  Logger(const LogLevel &level);

  void set_loglevel(const LogLevel &level);
  bool set_loglevel(const std::string &level);
  LogLevel get_loglevel();

  void log(const LogLevel &level, const std::string &msg);

 protected:
  virtual void log_raw(const std::string &msg) {};

 private:

  void set_mask(const LogLevel &level);

  LogLevel current_level_ = kLogInfo;
  std::bitset<kNumLogLevels> enable_mask_;
};

template <typename... Args>
void LOG(Logger &logger, const Logger::LogLevel &level, const Args ... args) {
  char temp[Logger::kMaxMsgLength];
  snprintf(temp, Logger::kMaxMsgLength, args...);
  logger.log(level, temp);
}

template <typename... Args>
void LOG_FATAL(Logger &logger, const Args ... args) {
  LOG(logger, Logger::kLogFatal, args...);
}

template <typename... Args>
void LOG_ERROR(Logger &logger, const Args ... args) {
  LOG(logger, Logger::kLogError, args...);
}

template <typename... Args>
void LOG_WARN(Logger &logger, const Args ... args) {
  LOG(logger, Logger::kLogWarn, args...);
}

template <typename... Args>
void LOG_INFO(Logger &logger, const Args ... args) {
  LOG(logger, Logger::kLogInfo, args...);
}

template <typename... Args>
void LOG_DEBUG(Logger &logger, const Args ... args) {
  LOG(logger, Logger::kLogDebug, args...);
}

template <typename... Args>
void LOG_TRACE(Logger &logger, const Args ... args) {
  LOG(logger, Logger::kLogTrace, args...);
}

#endif
