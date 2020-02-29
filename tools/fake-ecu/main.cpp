#include <atomic>
#include <chrono>
#include <csignal>
#include <cstdio>

#include "socket_can.hpp"
#include "obd2.hpp"

void send_response(SocketCan &can, const Obd2::Obd2Msg &msg);

std::atomic<bool> keep_running{true};
void signal_handler(int signal) {
  keep_running = false;
}

int main(int argc, char *argv[]) {
  std::signal(SIGINT, signal_handler);

  if(argc != 2) {
    printf("%s <can interface>\n", argv[0]);
    return -1;
  }
  SocketCan can(argv[1]);

  CanMsg msg;
  while(keep_running) {
    if(!can.read(msg, std::chrono::milliseconds(2000))) {
      printf("read failed\n");
      continue;
    }
    msg.print();
    Obd2::Obd2Msg obd;
    if(Obd2::Obd2Msg::parse_response(msg, obd)) {
      send_response(can, obd);
    }
  }
  return 0;
}

void send_response(SocketCan &can, const Obd2::Obd2Msg &msg) {
  Obd2::Obd2Msg out(msg.pid);
  out.can_id = 0x7E8;
  out.service = msg.service + Obd2::kServiceResponseOffset;
  if(!out.encode_value(msg.pid)) {
    return;
  }
  if(!can.write(out.get_can())) {
    printf("write failed\n");
  }
}
