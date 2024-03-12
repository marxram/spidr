#include "Arduino.h"
#include "Inverter.h"

float defaultValue = -100000.0;

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
    lastSuccessfullTimestamp = 0;
    lastReadSuccess = false; // Initialize as false
    inverterActive = true;
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

ParseStatus Inverter::updateData(const String& html) {
    Serial.println("Start updating data");
    ParseStatus result = extractVariables(html);
    inverterActive = true;
    return result;
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

void Inverter::setInactiveValues() {
    webdata_now_p = 0.0f;
    lastReadSuccess = false; // Initialize as false
    inverterActive = false;
}

bool Inverter::isInverterActive() {
    return inverterActive;
}

unsigned long Inverter::getLastUpdateTimestamp() const {
    return lastUpdateTimestamp;
}
unsigned long Inverter::getLastSuccessfullTimestamp() const {
    return lastSuccessfullTimestamp ;
}
bool Inverter::wasReadSuccessfull() const {
    return lastReadSuccess;
}

ParseStatus Inverter::extractVariables(const String& html) {
    Serial.println("Starting extractVariables");
    if (html.isEmpty()) {
        Serial.println("HTML is empty");
        return PARSE_HTML_ISSUES;
    }

    int countParseErrors = 0;
    int countParseSuccess = 0;

    // Define temporary variables for parsing
    Serial.println("Extracting float values...");
    float temp_webdata_now_p = extractFloatValue(html, "var webdata_now_p =");
    float temp_webdata_today_e = extractFloatValue(html, "var webdata_today_e =");
    float temp_webdata_total_e = extractFloatValue(html, "var webdata_total_e =");
    Serial.printf("Extracted float values: now_p=%f, today_e=%f, total_e=%f\n", temp_webdata_now_p, temp_webdata_today_e, temp_webdata_total_e);

    // Validate important float values
    if (temp_webdata_total_e <= 0.0) {
        Serial.println("webdata_total_e is <= 0.0, indicating parsing issues");
        return PARSE_PARSING_ISSUES;    
    } // Global consistency check
    
    if (temp_webdata_now_p != -100000.0) temp_webdata_now_p > 0.0 ? countParseSuccess++ : countParseErrors++;
    if (temp_webdata_today_e != -100000.0) temp_webdata_today_e >= 0.0 ? countParseSuccess++ : countParseErrors++;

    Serial.println("Extracting and validating string values...");
    // Extract and validate string values (Medium importance)
    String temp_webdata_sn = extractAndValidateString(html, "var webdata_sn =", countParseSuccess );
    Serial.printf("Extracted webdata_sn: %s\n", temp_webdata_sn.c_str());

    String temp_webdata_msvn = extractAndValidateString(html, "var webdata_msvn =", countParseSuccess);
    String temp_webdata_ssvn = extractAndValidateString(html, "var webdata_ssvn =", countParseSuccess);
    String temp_webdata_pv_type = extractAndValidateString(html, "var webdata_pv_type =", countParseSuccess);
    String temp_webdata_rate_p = extractAndValidateString(html, "var webdata_rate_p =", countParseSuccess);
    String temp_webdata_alarm = extractAndValidateString(html, "var webdata_alarm =", countParseSuccess);
    String temp_webdata_utime = extractAndValidateString(html, "var webdata_utime =", countParseSuccess);

    // Less important values, success counting only
    String temp_cover_mid = extractAndValidateString(html, "var cover_mid =", countParseSuccess);
    String temp_cover_ver = extractAndValidateString(html, "var cover_ver =", countParseSuccess);
    String temp_cover_wmode = extractAndValidateString(html, "var cover_wmode =", countParseSuccess);
    String temp_cover_ap_ssid = extractAndValidateString(html, "var cover_ap_ssid =", countParseSuccess);
    String temp_cover_ap_ip = extractAndValidateString(html, "var cover_ap_ip =", countParseSuccess);
    String temp_cover_ap_mac = extractAndValidateString(html, "var cover_ap_mac =", countParseSuccess);
    String temp_cover_sta_ssid = extractAndValidateString(html, "var cover_sta_ssid =", countParseSuccess);
    String temp_cover_sta_rssi = extractAndValidateString(html, "var cover_sta_rssi =", countParseSuccess);
    String temp_cover_sta_ip = extractAndValidateString(html, "var cover_sta_ip =", countParseSuccess);
    String temp_cover_sta_mac = extractAndValidateString(html, "var cover_sta_mac =", countParseSuccess);
    String temp_status_a = extractAndValidateString(html, "var status_a =", countParseSuccess);
    String temp_status_b = extractAndValidateString(html, "var status_b =", countParseSuccess);
    String temp_status_c = extractAndValidateString(html, "var status_c =", countParseSuccess);

    Serial.printf("Parse successes: %d, Parse errors: %d\n", countParseSuccess, countParseErrors);

    // Only update class members if all important checks passed
    if (countParseErrors == 0) {
        Serial.println("Updating class member variables with extracted values...");
        webdata_now_p = temp_webdata_now_p;
        webdata_today_e = temp_webdata_today_e;
        webdata_total_e = temp_webdata_total_e;
        webdata_sn = temp_webdata_sn;
        webdata_msvn = temp_webdata_msvn;
        webdata_ssvn = temp_webdata_ssvn;
        webdata_pv_type = temp_webdata_pv_type;
        webdata_rate_p = temp_webdata_rate_p;
        webdata_alarm = temp_webdata_alarm;
        webdata_utime = temp_webdata_utime;
        cover_mid = temp_cover_mid;
        cover_ver = temp_cover_ver;
        cover_wmode = temp_cover_wmode;
        cover_ap_ssid = temp_cover_ap_ssid;
        cover_ap_ip = temp_cover_ap_ip;
        cover_ap_mac = temp_cover_ap_mac;
        cover_sta_ssid = temp_cover_sta_ssid;
        cover_sta_rssi = temp_cover_sta_rssi;
        cover_sta_ip = temp_cover_sta_ip;
        cover_sta_mac = temp_cover_sta_mac;
        status_a = temp_status_a;
        status_b = temp_status_b;
        status_c = temp_status_c;

        lastSuccessfullTimestamp = millis();
        lastUpdateTimestamp = lastSuccessfullTimestamp;
        lastReadSuccess = true;
        Serial.println("Parsing OK");
        return PARSE_OK;
    } else {
        lastUpdateTimestamp = millis();
        lastReadSuccess = false;
        Serial.println("Parsing issues detected");
        return PARSE_PARSING_ISSUES;
    }
    
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
    return defaultValue;
}

String Inverter::extractAndValidateString(const String& html, const String& key, int& countParseSuccess) {
    String value = extractValue(html, key);
    if (!value.isEmpty()) {
        countParseSuccess++;
    }
    return value;
}
