#include "mpvi2.hpp"

#include <stdexcept>

const std::string Mpvi2::kSerialNumber{"MPVI00020"};

Mpvi2::Mpvi2(std::shared_ptr<SerialPort> serial) : serial_(serial) {

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

  if (!serial_->reset()) {
    printf("Unable to reset\n");
  }

  if(!serial_->flush()) {
    printf("Unable to flush\n");
  }

  // FTTIMEOUTS ftTS;
  // ftTS.ReadIntervalTimeout = 0;
  // ftTS.ReadTotalTimeoutMultiplier = 0;
  // ftTS.ReadTotalTimeoutConstant = 100;
  // ftTS.WriteTotalTimeoutMultiplier = 0;
  // ftTS.WriteTotalTimeoutConstant = 200;
  // if (!FT_W32_SetCommTimeouts(ftHandle_, &ftTS)) {
  //   printf("unable to set comm timeouts\n");
  // }

  // ftStatus = FT_SetLatencyTimer(ftHandle_, 2);
  // if (ftStatus != FT_OK) {
  //   printf("unable to set latency timer\n");
  // }

  if (!serial_->set_baud_rate(3000000)) {
    printf("unable to set baud\n");
  }

  if(!serial_->set_framing(SerialPort::kDataBits8, SerialPort::kDataParityNone, SerialPort::kDataStopBits1)) {
    printf("Unable to set data framing\n");
  }

  if (!serial_->set_flow_control(SerialPort::kDataFlowControlRtsCts)) {
    printf("Unable to set flow control\n");
  }

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
}

bool Mpvi2::write(std::vector<uint8_t> &data) {
  return serial_->write(data);
}

bool Mpvi2::read(std::vector<uint8_t> &data, const uint8_t num_bytes) {
  return serial_->read(data, num_bytes);
}

uint32_t Mpvi2::get_device_id() {
  std::vector<uint8_t> command{0x02, 0x01, 0x02, 0x01, 0x04, 0x14, 0x12, 0x63, 0x00};
  write(command);

  uint32_t dev_id = 0;
  if(read(command, 14)) {
    dev_id = (command[7] << 24) | (command[8] << 16) | (command[9] << 8) | (command[10]);
    printf("Device ID: %d\n", dev_id);
  }
  return dev_id;
}

uint16_t Mpvi2::get_part_number() {
  std::vector<uint8_t> command{0x02, 0x01, 0x02, 0x01, 0x04, 0x15, 0x02, 0x42, 0x00};
  write(command);

  uint16_t part_num = 0;
  if(read(command, 14)) {
    uint32_t part_num = (command[9] << 8) | (command[10]);
    printf("Part Num: %d\n", part_num);
  }
  return part_num;
}
