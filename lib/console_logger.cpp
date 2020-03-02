#include "console_logger.hpp"


ConsoleLogger::ConsoleLogger() : Logger() {}
ConsoleLogger::ConsoleLogger(const std::string &log_level) : Logger(log_level) {}
ConsoleLogger::ConsoleLogger(const Logger::LogLevel &level) : Logger(level) {}

void ConsoleLogger::log_raw(const std::string &msg) {
  printf("%s", msg.c_str());
}
