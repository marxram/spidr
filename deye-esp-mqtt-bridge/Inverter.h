#ifndef Inverter_h
#define Inverter_h

#include "Arduino.h"

class Inverter {
public:
  Inverter();
  void updateData(const String& html);
  String getWebdataSn() const;
  String getWebdataMsvn() const;
  String getWebdataSsvn() const;
  String getWebdataPvType() const;
  String getWebdataRateP() const;
  float getWebdataNowP() const;
  float getWebdataTodayE() const;
  float getWebdataTotalE() const;
  String getWebdataAlarm() const;
  String getWebdataUtime() const;
  String getCoverMid() const;
  String getCoverVer() const;
  String getCoverWmode() const;
  String getCoverApSsid() const;
  String getCoverApIp() const;
  String getCoverApMac() const;
  String getCoverStaSsid() const;
  String getCoverStaRssi() const;
  String getCoverStaIp() const;
  String getCoverStaMac() const;
  String getStatusA() const;
  String getStatusB() const;
  String getStatusC() const;
  unsigned long getLastUpdateTimestamp() const;

private:
  // Member variables
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
};

#endif
