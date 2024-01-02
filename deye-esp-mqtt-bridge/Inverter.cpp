#include "Arduino.h"
#include "Inverter.h"

// Constructor
Inverter::Inverter() {
    lastUpdateTimestamp = 0;
    // Initialize all variables
    webdata_sn = "";
    webdata_msvn = "";
    webdata_ssvn = "";
    webdata_pv_type = "";
    webdata_rate_p = "";
    webdata_now_p = 0.0f;
    webdata_today_e = 0.0f;
    webdata_total_e = 0.0f;
    webdata_alarm = "";
    webdata_utime = "";
    cover_mid = "";
    cover_ver = "";
    cover_wmode = "";
    cover_ap_ssid = "";
    cover_ap_ip = "";
    cover_ap_mac = "";
    cover_sta_ssid = "";
    cover_sta_rssi = "";
    cover_sta_ip = "";
    cover_sta_mac = "";
    status_a = "";
    status_b = "";
    status_c = "";
    lastUpdateTimestamp = 0;
}


void Inverter::printVariables() const {
    Serial.println("Inverter Variables:");
    Serial.print("Webdata SN: "); Serial.println(webdata_sn);
    Serial.print("Webdata MSVN: "); Serial.println(webdata_msvn);
    Serial.print("Webdata SSVN: "); Serial.println(webdata_ssvn);
    Serial.print("Webdata PV Type: "); Serial.println(webdata_pv_type);
    Serial.print("Webdata Rate P: "); Serial.println(webdata_rate_p);
    Serial.print("Webdata Now P: "); Serial.println(webdata_now_p);
    Serial.print("Webdata Today E: "); Serial.println(webdata_today_e);
    Serial.print("Webdata Total E: "); Serial.println(webdata_total_e);
    Serial.print("Webdata Alarm: "); Serial.println(webdata_alarm);
    Serial.print("Webdata Utime: "); Serial.println(webdata_utime);
    Serial.print("Cover MID: "); Serial.println(cover_mid);
    Serial.print("Cover VER: "); Serial.println(cover_ver);
    Serial.print("Cover WMODE: "); Serial.println(cover_wmode);
    Serial.print("Cover AP SSID: "); Serial.println(cover_ap_ssid);
    Serial.print("Cover AP IP: "); Serial.println(cover_ap_ip);
    Serial.print("Cover AP MAC: "); Serial.println(cover_ap_mac);
    Serial.print("Cover STA SSID: "); Serial.println(cover_sta_ssid);
    Serial.print("Cover STA RSSI: "); Serial.println(cover_sta_rssi);
    Serial.print("Cover STA IP: "); Serial.println(cover_sta_ip);
    Serial.print("Cover STA MAC: "); Serial.println(cover_sta_mac);
    Serial.print("Status A: "); Serial.println(status_a);
    Serial.print("Status B: "); Serial.println(status_b);
    Serial.print("Status C: "); Serial.println(status_c);
    Serial.print("Last Update Timestamp: "); Serial.println(lastUpdateTimestamp);
}

void Inverter::updateData(const String& html) {
    Serial.println("Start updating data");
    extractVariables(html);
    Serial.println("Data updated");
    lastUpdateTimestamp = millis();
}

String Inverter::getInverterSerial() const {
    return webdata_sn;
}

String Inverter::getWebdataMsvn() const {
    return webdata_msvn;
}

String Inverter::getWebdataSsvn() const {
    return webdata_ssvn;
}

String Inverter::getWebdataPvType() const {
    return webdata_pv_type;
}

String Inverter::getWebdataRateP() const {
    return webdata_rate_p;
}

float Inverter::getInverterPowerNow_W() const {
    return webdata_now_p;
}

float Inverter::getInverterEnergyToday_kWh() const {
    return webdata_today_e;
}

float Inverter::getInverterEnergyTotal_kWh() const {
    return webdata_total_e;
}

String Inverter::getWebdataAlarm() const {
    return webdata_alarm;
}

String Inverter::getWebdataUtime() const {
    return webdata_utime;
}

String Inverter::getLoggerModuleID() const {
    return cover_mid;
}

String Inverter::getLoggerSoftwareVersion() const {
    return cover_ver;
}

String Inverter::getLoggerWifiMode() const {
    return cover_wmode;
}

String Inverter::getLoggerApSsid() const {
    return cover_ap_ssid;
}

String Inverter::getLoggerApIp() const {
    return cover_ap_ip;
}

String Inverter::getLoggerApMac() const {
    return cover_ap_mac;
}

String Inverter::getLoggerStaSsid() const {
    return cover_sta_ssid;
}

String Inverter::getLoggerStaRssi() const {
    return cover_sta_rssi;
}

String Inverter::getLoggerStaIp() const {
    return cover_sta_ip;
}

String Inverter::getLoggerStaMac() const {
    return cover_sta_mac;
}

String Inverter::getRemoteServerStatusA() const {
    return status_a;
}

String Inverter::getRemoteServerStatusB() const {
    return status_b;
}

String Inverter::getRemoteServerStatusC() const {
    return status_c;
}

unsigned long Inverter::getLastUpdateTimestamp() const {
    return lastUpdateTimestamp;
}

void Inverter::extractVariables(const String& html) {
    webdata_sn = extractValue(html, "var webdata_sn =");
    webdata_msvn = extractValue(html, "var webdata_msvn =");
    webdata_ssvn = extractValue(html, "var webdata_ssvn =");
    webdata_pv_type = extractValue(html, "var webdata_pv_type =");
    webdata_rate_p = extractValue(html, "var webdata_rate_p =");
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

String Inverter::extractValue(const String& html, const String& variableName) const {
    int startIndex = html.indexOf(variableName);
    if (startIndex != -1) {
        startIndex += variableName.length();
        int endIndex = html.indexOf(";", startIndex);
        if (endIndex != -1) {
            String value = html.substring(startIndex, endIndex);
            value.replace("\"", ""); // Remove double quotation marks
            value.trim();
            return value;
        }
    }
    return "";
}

float Inverter::extractFloatValue(const String& html, const String& variableName) const {
    String valueStr = extractValue(html, variableName);
    if (valueStr.length() > 0) {
        return valueStr.toFloat();
    }
    return 0.0f;
}

