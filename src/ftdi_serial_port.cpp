
#include "ftdi_serial_port.hpp"

#include <chrono>
#include <stdexcept>
#include <thread>

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

  pthread_mutex_init(&eh_.eMutex, NULL);
  pthread_cond_init(&eh_.eCondVar, NULL);
}

FtdiSerialPort::~FtdiSerialPort() {
  if(ftHandle_) {
    FT_W32_CloseHandle(ftHandle_);
  }
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

uint32_t FtdiSerialPort::get_num_rx_bytes() {
  FTCOMSTAT newCS;
  DWORD dwErrors;
  if(!FT_W32_ClearCommError(ftHandle_, &dwErrors, (FTCOMSTAT *)&newCS)) {
    return 0;
  }
  return newCS.cbInQue;
}

bool FtdiSerialPort::wait_for_rx() {
  FT_SetEventNotification(ftHandle_, FT_EVENT_RXCHAR, (PVOID)&eh_);
  pthread_mutex_lock(&eh_.eMutex);
  pthread_cond_wait(&eh_.eCondVar, &eh_.eMutex);
  pthread_mutex_unlock(&eh_.eMutex);
  return true;
}

bool FtdiSerialPort::wait_for_bytes(const uint32_t num_bytes) {
  while(get_num_rx_bytes() < num_bytes) {
    wait_for_rx();
  }
  return true;
}



  // FT_STATUS ftStatus;
  // DWORD numDevs;
  // ftStatus = FT_ListDevices(&numDevs,NULL,FT_LIST_NUMBER_ONLY);
  // if (ftStatus == FT_OK) {
  //   printf("Got %d\n", numDevs);
  // // FT_ListDevices OK, number of devices connected is in numDevs
  // }
  // else {
  //   printf("Got none\n");
  // // FT_ListDevices failed
  // }


  // WORD devIndex = 0; // first device
  // char Buffer[64]; // more than enough room!
  // ftStatus =
  // FT_ListDevices((PVOID)devIndex,Buffer,FT_LIST_BY_INDEX|FT_OPEN_BY_SERIAL_NUMBER);
  // if (ftStatus == FT_OK) {
  // // FT_ListDevices OK, serial number is in Buffer
  //   printf("Serial: %s\n", Buffer);
  // }
  // else {
  //   printf("no serial\n");
  // // FT_ListDevices failed
  // }

  // // create the device information list
  // ftStatus = FT_CreateDeviceInfoList(&numDevs);
  // if (ftStatus == FT_OK) {
  // printf("Number of devices is %d\n",numDevs);
  // }
  // else {
  //   printf("no device info list\n");
  // // FT_CreateDeviceInfoList failed
  // }
  // if (numDevs > 0) {
  // // allocate storage for list based on numDevs
  // FT_DEVICE_LIST_INFO_NODE *devInfo =
  // (FT_DEVICE_LIST_INFO_NODE*)malloc(sizeof(FT_DEVICE_LIST_INFO_NODE)*numDevs);
  // // get the device information list
  // ftStatus = FT_GetDeviceInfoList(devInfo,&numDevs);
  // if (ftStatus == FT_OK) {
  // for (int i = 0; i < numDevs; i++) {
  // printf("Dev %d:\n",i);
  // printf(" Flags=0x%x\n",devInfo[i].Flags);
  // printf(" Type=0x%x\n",devInfo[i].Type);
  // printf(" ID=0x%x\n",devInfo[i].ID);
  // printf(" LocId=0x%x\n",devInfo[i].LocId);
  // printf(" SerialNumber=%s\n",devInfo[i].SerialNumber);
  // printf(" Description=%s\n",devInfo[i].Description);
  // printf(" ftHandle=0x%x\n",devInfo[i].ftHandle);
  // }
  // }
  // free(devInfo);
  // }


  // FT_SetEventNotification(ftHandle_, FT_EVENT_RXCHAR, )
  // std::vector<uint8_t> d1{0};
  // write(d1);
  // d1 = {0x02, 0x01, 0x02, 0x01, 0x04, 0x10, 0x52, 0xE7, 00};
  // write(d1);
  // d1.resize(63);
  // read(d1, 63);
  // for(int i = 0; i < 63; i++) {
  //   printf("0x%02X ", d1[i]);
  // }
