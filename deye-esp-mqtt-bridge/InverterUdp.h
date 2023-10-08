#ifndef InverterUdp_h
#define InverterUdp_h

#include "Arduino.h"
// AT Commands
#include <WiFiUdp.h>

class InverterUdp {
public:
  InverterUdp();
  bool inverter_connect(String udpSrv, int remPort, int locPort, int timeOut);
  String inverter_readtime();
  bool inverter_settime();
  bool inverter_close();
  bool isconnected();


private:
  String readModbus(String address, String length);
  const char* ModbusCrc(const char* dataStr);
  void Modbus(const uint8_t* data, uint8_t dataLength, uint8_t* output);
  void hexStringToBytes(const char* hexString, uint8_t* byteData, uint8_t byteDataLength);
  void send_message(String message);
  String getResponse();
  String noResponse;
  bool connected;
  // Member variables
  String webdata_sn;
  //udp Settings and variables
  char buffer[256];
  WiFiUDP udp;
  int udpTimeout_s;
  String udpLogin;
  String udpServer;
  int remotePort;
  int localPort;
};

#endif
