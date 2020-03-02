#include "utils.hpp"

Utils UtilManager::utils_;

void UtilManager::set_logger(std::shared_ptr<Logger> logger) {
  utils_.logger_ = logger;
}

std::shared_ptr<Logger> UtilManager::logger() {
  return utils_.logger_;
}
