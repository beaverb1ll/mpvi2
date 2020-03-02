#include "string_utils.hpp"

#include <algorithm>

namespace StringUtils {

std::string to_lower(std::string s) {
  std::transform(s.begin(), s.end(), s.begin(),
                 [](unsigned char c){ return std::tolower(c); }
                );
  return s;
}

};
