#include "Arduino.h"
//#include "Inverter.h"

class Inverter {
public:
  Inverter() {}

  // Function to update variables from HTML content
  void updateData(const String& html) {
    // Parse HTML content and extract variables
    extractVariables(html);

    // Update the timestamp
    lastUpdateTimestamp = millis();
  }

////////////////////////////////////////////////////////////////////////////////////////////
    // Getters 

    String getWebdataSn() const {
    return webdata_sn;
    }

    String getWebdataMsvn() const {
    return webdata_msvn;
    }

    String getWebdataSsvn() const {
    return webdata_ssvn;
    }

    String getWebdataPvType() const {
    return webdata_pv_type;
    }

    String getWebdataRateP() const {
    return webdata_rate_p;
    }

    float getWebdataNowP() const {
    return webdata_now_p;
    }

    float getWebdataTodayE() const {
    return webdata_today_e;
    }

    float getWebdataTotalE() const {
    return webdata_total_e;
    }

    String getWebdataAlarm() const {
    return webdata_alarm;
    }

    String getWebdataUtime() const {
    return webdata_utime;
    }

    String getCoverMid() const {
    return cover_mid;
    }

    String getCoverVer() const {
    return cover_ver;
    }

    String getCoverWmode() const {
    return cover_wmode;
    }

    String getCoverApSsid() const {
    return cover_ap_ssid;
    }

    String getCoverApIp() const {
    return cover_ap_ip;
    }

    String getCoverApMac() const {
    return cover_ap_mac;
    }

    String getCoverStaSsid() const {
    return cover_sta_ssid;
    }

    String getCoverStaRssi() const {
    return cover_sta_rssi;
    }

    String getCoverStaIp() const {
    return cover_sta_ip;
    }

    String getCoverStaMac() const {
    return cover_sta_mac;
    }

    String getStatusA() const {
    return status_a;
    }

    String getStatusB() const {
    return status_b;
    }

    String getStatusC() const {
    return status_c;
    }

    unsigned long getLastUpdateTimestamp() const {
    return lastUpdateTimestamp;
    }


private:
  // Variables
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

  // Timestamp for the last update
  unsigned long lastUpdateTimestamp = 0;

  // Function to extract variables from HTML content
  void extractVariables(const String& html) {
    // Use String functions to extract values from HTML content
    webdata_sn = extractValue(html, "var webdata_sn =");
    webdata_msvn = extractValue(html, "var webdata_msvn =");
    webdata_ssvn = extractValue(html, "var webdata_ssvn =");
    webdata_pv_type = extractValue(html, "var webdata_pv_type =");
    webdata_rate_p = extractValue(html, "var webdata_rate_p =");

    // Parse and update the float variables
    webdata_now_p = extractFloatValue(html, "var webdata_now_p =");
    webdata_today_e = extractFloatValue(html, "var webdata_today_e =");
    webdata_total_e = extractFloatValue(html, "var webdata_total_e =");

    webdata_alarm = extractValue(html, "var webdata_alarm =");
    webdata_utime = extractValue(html, "var webdata_utime =");
    cover_mid = extractValue(html, "var cover_mid =");
    cover_ver = extractValue(html, "var cover_ver =");
    cover_wmode = extractValue(html, "var cover_wmode =");
    cover_ap_ssid = extractValue(html, "var cover_ap_ssid =");
    cover_ap_ip = extractValue(html, "var cover_ap_ip =");
    cover_ap_mac = extractValue(html, "var cover_ap_mac =");
    cover_sta_ssid = extractValue(html, "var cover_sta_ssid =");
    cover_sta_rssi = extractValue(html, "var cover_sta_rssi =");
    cover_sta_ip = extractValue(html, "var cover_sta_ip =");
    cover_sta_mac = extractValue(html, "var cover_sta_mac =");
    status_a = extractValue(html, "var status_a =");
    status_b = extractValue(html, "var status_b =");
    status_c = extractValue(html, "var status_c =");
  }

  // Helper function to extract values from HTML content
  String extractValue(const String& html, const String& variableName) const {
    int startIndex = html.indexOf(variableName);
    if (startIndex != -1) {
      startIndex += variableName.length();
      int endIndex = html.indexOf(";", startIndex);
      if (endIndex != -1) {
        String value = html.substring(startIndex, endIndex);
        // Trim leading and trailing whitespaces
        value.trim();
        return value;
      }
    }
    return "";
  }

  // Helper function to extract float values from HTML content
  float extractFloatValue(const String& html, const String& variableName) const {
    String valueStr = extractValue(html, variableName);
    if (valueStr.length() > 0) {
      return valueStr.toFloat();
    }
    return 0.0f; // Default value if extraction fails
  }
    
    

};