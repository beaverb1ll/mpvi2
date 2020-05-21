#ifndef SAMPLE_HPP_
#define SAMPLE_HPP_

#include <limits>
#include <chrono>
#include <string>

struct Sample {
  Sample(const double &a_value,
      const std::chrono::high_resolution_clock::time_point &a_time) :
      value(a_value), timestamp(a_time) {}

  double value;
  std::chrono::high_resolution_clock::time_point timestamp;
};
#endif
