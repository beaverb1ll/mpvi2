#ifndef CHANNEL_HPP_
#define CHANNEL_HPP_

#include <limits>
#include <list>
#include <string>

#include "sample.hpp"
#include "obd2/obd2.hpp"

class Channel {
 public:
  Channel() = default;

  void update_value(const double new_value);

  double get_value() const;
  double get_min() const;
  double get_max() const;
  uint64_t get_num_samples() const;

 private:
  std::list<Sample> samples_;
  double min_value_ = std::numeric_limits<double>::max();
  double max_value_ = std::numeric_limits<double>::min();
};
#endif
