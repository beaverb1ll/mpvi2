
#include "ftdi_serial_port.hpp"

#include <stdexcept>

FtdiSerialPort::FtdiSerialPort(const std::string &serial_num) {
  ftHandle_ = FT_W32_CreateFile(serial_num.c_str(), 0xC0000000, /*GENERIC_READ|GENERIC_WRITE*/
                                  0,0, 3,/*OPEN_EXISTING*/
                                  0x40000081, /*FILE_ATTRIBUTE_NORMAL | FT_OPEN_BY_SERIAL_NUMBER*/
                                  0);
  if(ftHandle_ == (FT_HANDLE)INVALID_HANDLE_VALUE) {
    throw std::runtime_error("Unable to find FTDI device");
  }
  FTTIMEOUTS ftTS;
  ftTS.ReadIntervalTimeout = 0;
  ftTS.ReadTotalTimeoutMultiplier = 0;
  ftTS.ReadTotalTimeoutConstant = 100;
  ftTS.WriteTotalTimeoutMultiplier = 0;
  ftTS.WriteTotalTimeoutConstant = 200;
  if (!FT_W32_SetCommTimeouts(ftHandle_, &ftTS)) {
    printf("unable to set comm timeouts\n");
  }
  FT_SetLatencyTimer(ftHandle_, 2);
}

bool FtdiSerialPort::read(std::vector<uint8_t> &data, const uint8_t num_bytes) {
  DWORD dwRead;
  data.resize(num_bytes);
  if (!FT_W32_ReadFile(ftHandle_, data.data(), num_bytes, &dwRead, nullptr)) {
    return false;
  }
  return (num_bytes == dwRead);
}

bool FtdiSerialPort::write(const std::vector<uint8_t> &msg) {
  DWORD dwWritten;
  std::vector<uint8_t> copy{msg};
  if(!FT_W32_WriteFile(ftHandle_, copy.data(), copy.size(), &dwWritten, nullptr)) {
    return false;
  }
  return (dwWritten == msg.size());
}

bool FtdiSerialPort::set_baud_rate(const uint32_t rate) {
  const FT_STATUS status = FT_SetBaudRate(ftHandle_, rate);
  return status == FT_OK;
}

bool FtdiSerialPort::set_flow_control(const DataFlowControl flow_control) {
  USHORT ft_fc;
  switch(flow_control) {
    case kDataFlowControlNone:
    ft_fc = FT_FLOW_NONE;
    break;

    case kDataFlowControlRtsCts:
    ft_fc = FT_FLOW_RTS_CTS;
    break;

    case kDataFlowControlDtrDsr:
    ft_fc = FT_FLOW_DTR_DSR;
    break;

    case kDataFlowControlXOnXOff:
    ft_fc = FT_FLOW_XON_XOFF;
    break;

    default:
    return false;
    break;
  }
  const FT_STATUS status = FT_SetFlowControl(ftHandle_, ft_fc, 0, 0);
  return status == FT_OK;
}

bool FtdiSerialPort::set_framing(const DataBits data, const DataParity parity, DataStopBits stop_bits) {
  char fti_length, fti_stop, fti_parity;
  switch(data) {
    case kDataBits7:
    fti_length = FT_BITS_7;
    break;

    case kDataBits8:
    fti_length = FT_BITS_8;
    break;

    default:
    return false;
    break;
  }

  switch(parity) {
    case kDataParityNone:
    fti_parity = FT_PARITY_NONE;
    break;

    case kDataParityOdd:
    fti_parity = FT_PARITY_ODD;
    break;

    case kDataParityEven:
    fti_parity = FT_PARITY_EVEN;
    break;

    case kDataParityMark:
    fti_parity = FT_PARITY_MARK;
    break;

    case kDataParitySpace:
    fti_parity = FT_PARITY_SPACE;
    break;

    default:
    return false;
    break;
  }

  switch(stop_bits) {
    case kDataStopBits1:
    fti_stop = FT_STOP_BITS_1;
    break;

    case kDataStopBits2:
    fti_stop = FT_STOP_BITS_2;
    break;

    default:
    return false;
    break;
  }
  const FT_STATUS status = FT_SetDataCharacteristics(ftHandle_, fti_length, fti_stop, fti_parity);
  return status == FT_OK;
}

bool FtdiSerialPort::flush() {
  return FT_W32_PurgeComm(ftHandle_, PURGE_TXCLEAR | PURGE_RXCLEAR);
}

bool FtdiSerialPort::reset() {

  const FT_STATUS status = FT_ResetDevice(ftHandle_);
  return status == FT_OK;
}
