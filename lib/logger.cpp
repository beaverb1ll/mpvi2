#include "logger.hpp"

#include <array>
#include <chrono>
#include <ctime>
#include <sys/time.h>

#include "string_utils.hpp"

Logger::Logger() {
  set_mask(current_level_);
}

Logger::Logger(const std::string &level) {
  if(!set_loglevel(level)) {
    throw std::runtime_error("");
  }
}

Logger::Logger(const Logger::LogLevel &level) : current_level_(level) {
  set_mask(level);
}

bool Logger::set_loglevel(const std::string &level) {
  const auto log_level_lower = StringUtils::to_lower(level);
  Logger::LogLevel level_out;
  if(log_level_lower == "fatal") {
    level_out = Logger::kLogFatal;
  } else if(log_level_lower == "error") {
    level_out = Logger::kLogError;
  } else if(log_level_lower == "warn") {
    level_out = Logger::kLogWarn;
  } else if(log_level_lower == "info") {
    level_out = Logger::kLogInfo;
  } else if(log_level_lower == "debug") {
    level_out = Logger::kLogDebug;
  } else if(log_level_lower == "trace") {
    level_out = Logger::kLogTrace;
  } else {
    return false;
  }
  set_loglevel(level_out);
  return true;
}

void Logger::set_loglevel(const Logger::LogLevel &level) {
  current_level_ = level;
  set_mask(level);
}

Logger::LogLevel Logger::get_loglevel() {
  return current_level_;
}

void Logger::log(const Logger::LogLevel &level, const std::string &msg) {
  if(enable_mask_[level]) {

    timeval curTime;
    gettimeofday(&curTime, NULL);
    const int milli = curTime.tv_usec / 1000;
    char buffer [80];
    if(!std::strftime(buffer, 80, "%T", localtime(&curTime.tv_sec))) {
      buffer[0] = '0';
      buffer[1] = '\0';
    }
    printf("%s.%03d [%s] %s\n", buffer, milli, get_level_name(level).c_str(), msg.c_str());
  }
}

const std::string& Logger::get_level_name(const Logger::LogLevel &level) {
  static const std::array<std::string, Logger::kNumLogLevels+1> names{
    "", // OFF
    "FATAL",
    "ERROR",
    "WARN",
    "INFO",
    "DEBUG",
    "TRACE",
    ""
  };
  return names[level];
}

void Logger::set_mask(const Logger::LogLevel &level) {
  enable_mask_.reset();

  for(int i = 0; i <= level && i < kNumLogLevels; i++) {
    enable_mask_[i] = true;
  }
}
