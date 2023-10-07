#ifndef InverterUdp_h
#define InverterUdp_h

#include "Arduino.h"
// AT Commands
#include <WiFiUdp.h>

class InverterUdp {
public:
  InverterUdp(String udpSrv);
  InverterUdp(String udpSrv, int remPort, int locPort);
  bool inverter_connect();
  String inverter_readtime();
  bool inverter_settime();
  bool inverter_close();
  bool isconnected();


private:
  bool connected;
  // Member variables
  String webdata_sn;
  //udp Settings and variables
  char buffer[64];
  String udpServer;
  unsigned int localPort;
  unsigned int remotePort;
  WiFiUDP udp;
  int udpTimeout_s;
  String udpLogin;
};

#endif
