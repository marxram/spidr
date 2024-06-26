#ifndef Inverter_h
#define Inverter_h

#include "Arduino.h"
#include "SerialCaptureLines.h"

enum ParseStatus {
    PARSE_OK,
    PARSE_ERROR,
    PARSE_HTML_ISSUES,
    PARSE_PARSING_ISSUES
};



class Inverter {
public:
  
  struct DataPoint {
    unsigned long timestamp; // Stores the time when the data was recorded
    float value;             // Stores the corresponding value
};

  Inverter(SerialCaptureLines& serialCapture);
  ParseStatus updateData(const String& html);
  static const int bufferSize = 192;  // Buffer size 16h @ 5-minute intervals

  String getInverterSerial() const;
  String getWebdataMsvn() const;
  String getWebdataSsvn() const;
  String getWebdataPvType() const;
  String getWebdataRateP() const;
  float getInverterPowerNow_W() const;
  float getInverterEnergyToday_kWh() const;
  float getInverterEnergyTotal_kWh() const;
  String getWebdataAlarm() const;
  String getWebdataUtime() const;
  String getLoggerModuleID() const;
  String getLoggerSoftwareVersion() const;
  String getLoggerWifiMode() const;
  String getLoggerApSsid() const;
  String getLoggerApIp() const;
  String getLoggerApMac() const;
  String getLoggerStaSsid() const;
  String getLoggerStaRssi() const;
  String getLoggerStaIp() const;
  String getLoggerStaMac() const;
  String getRemoteServerStatusA() const;
  String getRemoteServerStatusB() const;
  String getRemoteServerStatusC() const;
  unsigned long getLastUpdateTimestamp() const;
  unsigned long getLastSuccessfullTimestamp() const;
  bool wasReadSuccessfull() const;
  void printVariables() const;
  void setInactiveValues();
  bool isInverterActive();
  const DataPoint* getPowerData() const; // Returns a pointer to the power data array
  int getPowerDataSize() const; // Returns the current size or count of effective data points
  void generateTestData();
  void initializeDataBuffer(float powerValue);
  void resetEnergyTodayCounter();


private:
  // Member variables
  SerialCaptureLines& serialCapture;
  String webdata_sn;
  String webdata_msvn;
  String webdata_ssvn;
  String webdata_pv_type;
  String webdata_rate_p;
  float webdata_now_p;
  float webdata_today_e;
  float webdata_total_e;
  String webdata_alarm;
  String webdata_utime;
  String cover_mid;
  String cover_ver;
  String cover_wmode;
  String cover_ap_ssid;
  String cover_ap_ip;
  String cover_ap_mac;
  String cover_sta_ssid;
  String cover_sta_rssi;
  String cover_sta_ip;
  String cover_sta_mac;
  String status_a;
  String status_b;
  String status_c;
  unsigned long lastUpdateTimestamp;
  unsigned long lastSuccessfullTimestamp;
  bool lastReadSuccess;
  ParseStatus  extractVariables(const String& html);
  String extractValue(const String& html, const String& variableName) const;
  float extractFloatValue(const String& html, const String& variableName) const;
  String extractAndValidateString(const String& html, const String& key, int& countParseSuccess);
  bool inverterActive;
  void addDataPoint(DataPoint buffer[], int &index, float value);
  void updateDataPoints(float power, float energyToday, float energyTotal);
  DataPoint powerData[bufferSize];
  DataPoint energyTodayData[bufferSize];
  DataPoint energyTotalData[bufferSize];
  int powerIndex = 0;
  int energyTodayIndex = 0;
  int energyTotalIndex = 0;
};

#endif
