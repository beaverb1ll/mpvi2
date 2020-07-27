##fake-ecu
  Provides a test bed. only responds with hardcoded values compiled into main

##fake-ecu-gui
  Provides a means of easily emulating an OBD-II ECU. Can easily enable/disable various PIDs and respond with specified values



### Notes
sudo ip link set can0 type can bitrate 500000
sudo ip link set up dev can0
