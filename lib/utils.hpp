#ifndef UTILS_HPP_
#define UTILS_HPP_

#include <memory>

#include "logger.hpp"

struct Utils {
  std::shared_ptr<Logger> logger_;
};

class UtilManager {
 public:

  static void set_logger(std::shared_ptr<Logger> logger);
  static std::shared_ptr<Logger> logger();

 private:
  static Utils utils_;
};

#endif
