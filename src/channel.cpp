
#include "channel.hpp"

void Channel::update_value(const double new_value) {
  samples_.emplace_back(new_value, std::chrono::high_resolution_clock::now());
  if(new_value > max_value_) {
    max_value_ = new_value;
  } else if(new_value < min_value_) {
    min_value_ = new_value;
  }
}

double Channel::get_value() const {
  if(samples_.size() < 1) {
    return 0;
  }
  return samples_.back().value;
}

double Channel::get_max() const {
  return max_value_;
}

double Channel::get_min() const {
  return min_value_;
}

uint64_t Channel::get_num_samples() const {
  return samples_.size();
}

