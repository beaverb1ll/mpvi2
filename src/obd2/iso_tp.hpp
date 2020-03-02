#ifndef ISO_TP_HPP_
#define ISO_TP_HPP_

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstring>

#include "can_msg.hpp"
#include "obd2.hpp"

namespace IsoTp {

constexpr const uint8_t kArbitrationIdOffset = 0x8;

//  Protocol Control Information
enum ProtocolInformation {
  kPciSingleFrame = 0x0,
  kPciFirstFrame = 0x1,
  kPciConsecutiveFrame = 0x2,
  kPciFlowControlFrame = 0x3
};

// PCI flow control IDs
enum FlowControl {
  kFlowControlContinue = 0x0,
  kFlowControlWait = 0x1,
  kFlowControlOverflow = 0x2
};

struct IsoTpData {
  uint32_t can_id;
  uint8_t service;
  uint8_t pid;
  uint8_t length;
  std::array<uint8_t, 256> data; // somewhat arbitrary, actual spec is greater
};

class IsoTpReceiveMsg {
 public:
   enum RxState {
    kRxUninitialized = 0,
    kRxSendingFlowControl,
    kRxReceiving,
    kRxComplete,
    kNumRxStates
  };
  IsoTpReceiveMsg(const IsoTpData &data) : data_(data) {}

  const IsoTpData& get_data() {return data_;}

  CanMsg get_flow_control() {
    CanMsg out;
    out.id = data_.can_id;
    out.length = 8;
    memset(out.data.data(), 0, sizeof(out.data));
    out.data[0] = (kPciFlowControlFrame << 0x4);
    state_ = kRxReceiving;
    return out;
  }
  void process_rx_frame(const CanMsg &rx) {
    switch(state_) {
      case kRxUninitialized:
        if(rx.data[0] == 0x10 &&
            rx.data[2] == data_.service + Obd2::kServiceResponseOffset &&
            rx.data[3] == data_.pid) {
          data_.length = rx.data[1] - 3;
          data_.data[0] = rx.data[5];
          data_.data[1] = rx.data[6];
          data_.data[2] = rx.data[7];
          num_bytes_received_ += 3;
          state_ = kRxSendingFlowControl;
          printf("Got valid response\n");
        }
        break;
      case kRxSendingFlowControl:
        break;
      case kRxReceiving:
        if(rx.data[0] == ((0x2 << 4) | index_)) {
          printf("got msg: %d\n", index_);
          index_++;
          std::copy_n(rx.data.begin()+1, 7, data_.data.begin() + num_bytes_received_);
          num_bytes_received_ += 7;
          if(num_bytes_received_ >= data_.length) {
            state_ = kRxComplete;
          }
        }
        break;
      case kRxComplete:
        break;
      default:
        break;
    }
  }
  RxState get_state() {return state_;};

 private:
  IsoTpData data_;
  uint8_t index_ = 1;
  uint8_t num_bytes_received_ = 0;
  RxState state_ = kRxUninitialized;
};

class IsoTpTransmitMsg {
 public:
  enum TxState {
    kTxUninitialized,
    kTxInitialized,
    kTxAwaitingFlowControl,
    kTxSending,
    kTxComplete,
    kNumTxStates
  };

  IsoTpTransmitMsg() : state_(kTxComplete) {}
  IsoTpTransmitMsg(const IsoTpData &data) : state_(kTxInitialized), data_(data) {}

  TxState get_state() {return state_;}

  bool is_runnable() {
    return (state_ != kTxComplete &&
       state_ != kTxUninitialized);
  }

  bool process_rx_frame(const CanMsg &rx) {
    if(state_ == kTxAwaitingFlowControl &&
        rx.data[0] == 0x30) {
      state_ = kTxSending;
      return true;
    }
    return false;
  }

  bool get_next_tx_frame(CanMsg &out) {
    switch(state_) {
      case kTxUninitialized:
        break;
      case kTxInitialized:
        out.id = data_.can_id;
        out.length = 8;
        out.data[0] = 0x10;
        out.data[1] = data_.length + 3;
        out.data[2] = data_.service + Obd2::kServiceResponseOffset;
        out.data[3] = data_.pid;
        out.data[4] = 0x01;  // why is this a 1?
        out.data[5] = data_.data[0];
        out.data[6] = data_.data[1];
        out.data[7] = data_.data[2];
        num_bytes_sent_ += 3;
        state_ = kTxAwaitingFlowControl;
        return true;
        break;
      case kTxAwaitingFlowControl:
        break;
      case kTxSending:
        out.id = data_.can_id;
        out.length = 8;
        out.data[0] = (0x2 << 4) | index_;
        std::copy_n(data_.data.begin()+num_bytes_sent_, 7, out.data.begin()+1);
        num_bytes_sent_ += 7;
        printf("sending %d\n", index_);
        index_++;
        if(num_bytes_sent_ >= data_.length) {
          state_ = kTxComplete;
          printf("tx complete\n");
        }
        return true;
        break;
      case kTxComplete:
        break;
      default:
        break;
    }
    return false;
  }

 private:
   TxState state_ = kTxUninitialized;
   IsoTpData data_;
   uint8_t index_ = 1;

  uint8_t num_bytes_sent_ = 0;

};

};
#endif
