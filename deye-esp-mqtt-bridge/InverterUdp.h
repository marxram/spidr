#ifndef InverterUdp_h
#define InverterUdp_h

#include "Arduino.h"
#include "RTClib.h" // RTC Library
// AT Commands
#include <WiFiUdp.h>


#define TIME_NOT_INITIALIZTED_TOKEN "NO-TIME"

class InverterUdp {
public:
  InverterUdp();
  bool inverter_connect(String udpSrv, int remPort, int locPort, int timeOut);
  String inverter_readtime();
  bool inverter_close();
  bool isconnected();
  bool parseDateTime(String timestring);
  bool hexStringToDec(String hexString, int& output) ;
  String decToHex(int dec);
  String inverter_settime(unsigned long epochTime);
  bool isDefaultTimeIsSet();
  DateTime getInverterTime();
  bool isTimeSynchronized();


private:
  String readModbus(String address, String length);
  void Modbus(const uint8_t* data, uint8_t dataLength, uint8_t* output);
  void hexStringToBytes(const char* hexString, uint8_t* byteData, uint8_t byteDataLength);
  String byteToHexString(const uint8_t* byteArray, size_t length);
  void send_message(String message);
  String getResponse(bool deleteSeparatorChars);
  void removeByte(char* buffer, char byteToRemove, size_t bufferSize);
  long calculateUnixTimestamp(int year, int month, int day, int hour, int minute, int second);
  String writeModbus(String address, String  length, String payload, String payloadlength);
  bool defaultTimeIsSet = true;
  DateTime inverterTime;

  String noResponse;
  String RESP_TIME_UNSET;
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
  String modbusIntro;
  String modbusOutro;
  String modbusWriteToken;
  String modbusReadToken;
};

#endif
