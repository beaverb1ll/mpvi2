#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <vector>

#include "obd2/can_msg.hpp"
#include "mpvi2.hpp"
#include "mock_serial_port.hpp"

using ::testing::Return;
using ::testing::NiceMock;
using ::testing::_;

class CanSendTest : public ::testing::Test {
 protected:
  void SetUp() override {
    serial_ = std::make_shared<NiceMock<MockSerialPort>>();
    ON_CALL(*serial_, reset()).WillByDefault(Return(true));
    ON_CALL(*serial_, flush()).WillByDefault(Return(true));
    ON_CALL(*serial_, set_baud_rate(_)).WillByDefault(Return(true));
    ON_CALL(*serial_, set_framing(_, _, _)).WillByDefault(Return(true));
    ON_CALL(*serial_, set_flow_control(_)).WillByDefault(Return(true));
    ON_CALL(*serial_, wait_for_rx(_)).WillByDefault(Return(true));
    ON_CALL(*serial_, get_num_rx_bytes()).WillByDefault(Return(0));

    mpvi2_ = std::make_shared<Mpvi2>(serial_);

  }

  void TearDown() override {}

  void data_test_can(const CanMsg &msg, const std::vector<uint8_t> &out) {
    EXPECT_CALL(*serial_, write(out));

    mpvi2_->send_can(msg);
  }

  std::shared_ptr<MockSerialPort> serial_;
  std::shared_ptr<Mpvi2> mpvi2_;
};


TEST_F(CanSendTest, data1) {

  data_test_can({
      0x7DF, 8,
      {0x02, 0x1, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
      {0x02, 0x1C, 0x01, 0x05, 0x07, 0xDF, 0x02, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x03, 0x93, 0x0E, 0x00});
}

TEST_F(CanSendTest, data2) {

  data_test_can({
      0x7DF, 8,
      {0x02, 0x01, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00}},
      {0x02, 0x1C, 0x01, 0x06, 0x07, 0xDF, 0x02, 0x01, 0x20, 0x01, 0x01, 0x01, 0x01, 0x03, 0xA6, 0x06, 0x00});
}

TEST_F(CanSendTest, data3) {

  data_test_can({
      0x7DF, 8,
      {0x02, 0x01, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00}},
      {0x02, 0x1C, 0x01, 0x06, 0x07, 0xDF, 0x02, 0x01, 0x40, 0x01, 0x01, 0x01, 0x01, 0x03, 0xF9, 0x1E, 0x00});
}

