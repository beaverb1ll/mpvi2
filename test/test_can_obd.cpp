#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <vector>

#include "obd2/can_msg.hpp"
#include "obd2/obd2.hpp"

using ::testing::Return;
using ::testing::Invoke;
using ::testing::NiceMock;
using ::testing::SetArgReferee;
using ::testing::DoAll;
using ::testing::InSequence;
using ::testing::_;

class CanObdTest : public ::testing::Test {
 protected:
  void SetUp() override {
  }

  void TearDown() override {
  }

};


TEST_F(CanObdTest, data1) {

  Obd2::Obd2Msg truth;
  truth.can_id = Obd2::kEcuResponseAddress;
  truth.num_bytes = 0x2;
  truth.service = 0x41;
  truth.pid = 0x0C;
  truth.data = {0x1D, 0x51, 0, 0};

  const CanMsg in{Obd2::kEcuResponseAddress, 8, {0x04, 0x41, 0x0C, 0x1D, 0x51, 0xAA, 0xAA, 0xAA}};

  Obd2::Obd2Msg out;
  const auto ret_val = Obd2::Obd2Msg::parse_response(in, out);
  ASSERT_EQ(ret_val, true);

  ASSERT_EQ(out, truth);
}

TEST_F(CanObdTest, supported_pids1) {
  Obd2::Obd2Msg in;
  in.can_id = Obd2::kEcuResponseAddress;
  in.num_bytes = 0x4;
  in.service = 0x41;
  in.pid = 0x0;
  in.data = {0xBE, 0x1F, 0xA8, 0x13};
  const std::bitset<32> truth{0b11001000000101011111100001111101};

  const auto out = Obd2::supported_pids(in);
  ASSERT_EQ(out, truth);
}

TEST_F(CanObdTest, pids) {
  ASSERT_EQ(0x46, Obd2::kPidAmbientAirTemp);
  ASSERT_EQ(0x70, Obd2::kPidBoostPressureCntrl);
}

