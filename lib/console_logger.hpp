#ifndef CONSOLE_LOGGER_HPP_
#define CONSOLE_LOGGER_HPP_

#include "logger.hpp"

class ConsoleLogger : public Logger {
 public:
  ConsoleLogger();
  ConsoleLogger(const std::string &log_level);
  ConsoleLogger(const Logger::LogLevel &level);

private:
  void log_raw(const std::string &msg) override;

};

#endif
