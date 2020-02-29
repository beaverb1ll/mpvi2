#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <vector>

#include "can_msg.hpp"
#include "mpvi2.hpp"
#include "mock_serial_port.hpp"

using ::testing::Return;
using ::testing::Invoke;
using ::testing::NiceMock;
using ::testing::SetArgReferee;
using ::testing::DoAll;
using ::testing::InSequence;
using ::testing::_;

class CanReceiveTest : public ::testing::Test {
 protected:
  void SetUp() override {
    serial_ = std::make_shared<NiceMock<MockSerialPort>>();
    ON_CALL(*serial_, reset()).WillByDefault(Return(true));
    ON_CALL(*serial_, flush()).WillByDefault(Return(true));
    ON_CALL(*serial_, set_baud_rate(_)).WillByDefault(Return(true));
    ON_CALL(*serial_, set_framing(_, _, _)).WillByDefault(Return(true));
    ON_CALL(*serial_, set_flow_control(_)).WillByDefault(Return(true));

  }

  void TearDown() override {
    mpvi2_ = nullptr;
    serial_ = nullptr;
  }

  std::shared_ptr<MockSerialPort> serial_;
  std::shared_ptr<Mpvi2> mpvi2_;
};


TEST_F(CanReceiveTest, data1) {

  const std::vector<uint8_t> rx_data{0x02, 0x1C, 0x05, 0x07, 0xE8, 0x06, 0x41, 0x08, 0xBF, 0xFF, 0xB9, 0x93, 0xAA, 0x17, 0x63, 0x00};
  const CanMsg truth{0x07E8, 8, {0x06, 0x41, 0x00, 0xBF, 0xFF, 0xB9, 0x93, 0xAA}};

  EXPECT_CALL(*serial_, get_num_rx_bytes())
    .WillOnce(Return(16))
    .WillRepeatedly(Return(0));

  EXPECT_CALL(*serial_, read(_, 16, _))
    .WillOnce(DoAll(SetArgReferee<0>(rx_data), Return(true)));
  EXPECT_CALL(*serial_, read(_, 0, _))
    .WillRepeatedly(Return(false));

  mpvi2_ = std::make_shared<Mpvi2>(serial_);

  CanMsg can_msg;
  ASSERT_TRUE(mpvi2_->get_next_can_msg(can_msg));

  ASSERT_EQ(can_msg.id, truth.id);

}


