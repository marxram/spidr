#include "Arduino.h"
#include "Inverter.h"
#include "SerialCaptureLines.h"


float defaultValue = -100000.0;


// Constructor
Inverter::Inverter(SerialCaptureLines& serialCapture): serialCapture(serialCapture)  {
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
    serialCapture.println("Inverter Variables:");
    serialCapture.print("Webdata SN: "); serialCapture.println(webdata_sn);
    serialCapture.print("Webdata MSVN: "); serialCapture.println(webdata_msvn);
    serialCapture.print("Webdata SSVN: "); serialCapture.println(webdata_ssvn);
    serialCapture.print("Webdata PV Type: "); serialCapture.println(webdata_pv_type);
    serialCapture.print("Webdata Rate P: "); serialCapture.println(webdata_rate_p);
    serialCapture.print("Webdata Now P: "); serialCapture.println(webdata_now_p);
    serialCapture.print("Webdata Today E: "); serialCapture.println(webdata_today_e);
    serialCapture.print("Webdata Total E: "); serialCapture.println(webdata_total_e);
    serialCapture.print("Webdata Alarm: "); serialCapture.println(webdata_alarm);
    serialCapture.print("Webdata Utime: "); serialCapture.println(webdata_utime);
    serialCapture.print("Cover MID: "); serialCapture.println(cover_mid);
    serialCapture.print("Cover VER: "); serialCapture.println(cover_ver);
    serialCapture.print("Cover WMODE: "); serialCapture.println(cover_wmode);
    serialCapture.print("Cover AP SSID: "); serialCapture.println(cover_ap_ssid);
    serialCapture.print("Cover AP IP: "); serialCapture.println(cover_ap_ip);
    serialCapture.print("Cover AP MAC: "); serialCapture.println(cover_ap_mac);
    serialCapture.print("Cover STA SSID: "); serialCapture.println(cover_sta_ssid);
    serialCapture.print("Cover STA RSSI: "); serialCapture.println(cover_sta_rssi);
    serialCapture.print("Cover STA IP: "); serialCapture.println(cover_sta_ip);
    serialCapture.print("Cover STA MAC: "); serialCapture.println(cover_sta_mac);
    serialCapture.print("Status A: "); serialCapture.println(status_a);
    serialCapture.print("Status B: "); serialCapture.println(status_b);
    serialCapture.print("Status C: "); serialCapture.println(status_c);
    serialCapture.print("Last Update Timestamp: "); serialCapture.println(lastUpdateTimestamp);
}

ParseStatus Inverter::updateData(const String& html) {
    serialCapture.println("Start updating data");
    ParseStatus result = extractVariables(html);
    inverterActive = true;
    serialCapture.println("Inverter Set Active");
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
    serialCapture.println("Inverter Set InActive");
    webdata_now_p = 0.0f;
    webdata_today_e = 0.0f;
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
    serialCapture.println("Starting extractVariables");
    if (html.isEmpty()) {
        serialCapture.println("HTML is empty");
        return PARSE_HTML_ISSUES;
    }

    int countParseErrors = 0;
    int countParseSuccess = 0;

    // Define temporary variables for parsing
    serialCapture.println("Extracting float values...");
    float temp_webdata_now_p = extractFloatValue(html, "webdata_now_p =");
    float temp_webdata_today_e = extractFloatValue(html, "webdata_today_e =");
    float temp_webdata_total_e = extractFloatValue(html, "webdata_total_e =");
    serialCapture.printf("Extracted float values: now_p=%f, today_e=%f, total_e=%f\n", temp_webdata_now_p, temp_webdata_today_e, temp_webdata_total_e);

    // Validate important float values
    if (temp_webdata_total_e <= 0.0) {
        serialCapture.println("webdata_total_e is <= 0.0, indicating parsing issues");
        return PARSE_PARSING_ISSUES;    
    } // Global consistency check
    

    serialCapture.println("Extracting and validating string values...");
    // Extract and validate string values (Medium importance)
    String temp_webdata_sn = extractAndValidateString(html, "webdata_sn =", countParseSuccess );
    serialCapture.printf("Extracted webdata_sn: %s\n", temp_webdata_sn.c_str());

    String temp_webdata_msvn = extractAndValidateString(html, "webdata_msvn =", countParseSuccess);
    String temp_webdata_ssvn = extractAndValidateString(html, "webdata_ssvn =", countParseSuccess);
    String temp_webdata_pv_type = extractAndValidateString(html, "webdata_pv_type =", countParseSuccess);
    String temp_webdata_rate_p = extractAndValidateString(html, "webdata_rate_p =", countParseSuccess);
    String temp_webdata_alarm = extractAndValidateString(html, "webdata_alarm =", countParseSuccess);
    String temp_webdata_utime = extractAndValidateString(html, "webdata_utime =", countParseSuccess);

    // Less important values, success counting only
    String temp_cover_mid = extractAndValidateString(html, "cover_mid =", countParseSuccess);
    String temp_cover_ver = extractAndValidateString(html, "cover_ver =", countParseSuccess);
    String temp_cover_wmode = extractAndValidateString(html, "cover_wmode =", countParseSuccess);
    String temp_cover_ap_ssid = extractAndValidateString(html, "cover_ap_ssid =", countParseSuccess);
    String temp_cover_ap_ip = extractAndValidateString(html, "cover_ap_ip =", countParseSuccess);
    String temp_cover_ap_mac = extractAndValidateString(html, "cover_ap_mac =", countParseSuccess);
    String temp_cover_sta_ssid = extractAndValidateString(html, "cover_sta_ssid =", countParseSuccess);
    String temp_cover_sta_rssi = extractAndValidateString(html, "cover_sta_rssi =", countParseSuccess);
    String temp_cover_sta_ip = extractAndValidateString(html, "cover_sta_ip =", countParseSuccess);
    String temp_cover_sta_mac = extractAndValidateString(html, "cover_sta_mac =", countParseSuccess);
    String temp_status_a = extractAndValidateString(html, "status_a =", countParseSuccess);
    String temp_status_b = extractAndValidateString(html, "status_b =", countParseSuccess);
    String temp_status_c = extractAndValidateString(html, "status_c =", countParseSuccess);

    serialCapture.printf("Parse successes: %d, Parse errors: %d\n", countParseSuccess, countParseErrors);

    // Only update class members if all important checks passed
    if (countParseErrors == 0) {
        serialCapture.println("Updating class member variables with extracted values...");
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
        serialCapture.println("Parsing OK");

        updateDataPoints(temp_webdata_now_p, temp_webdata_today_e, temp_webdata_total_e);

        return PARSE_OK;
    } else {
        lastUpdateTimestamp = millis();
        lastReadSuccess = false;
        serialCapture.println("Parsing issues detected");
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

void Inverter::addDataPoint(DataPoint buffer[], int &index, float value) {
    unsigned long currentTime = millis() / 1000; // Convert to seconds
    // Normalize to the nearest 5-minute mark
    unsigned long timeMark = (currentTime / 300) * 300; // 300 seconds = 5 minutes

    // Check if the current timeMark already exists in the buffer
    if (index > 0 && buffer[(index - 1) % bufferSize].timestamp == timeMark) {
        // Update the existing data point
        buffer[(index - 1) % bufferSize].value = value;
    } else {
        // Add a new data point
        buffer[index % bufferSize].timestamp = timeMark;
        buffer[index % bufferSize].value = value;
        index++;
    }
}

void Inverter::updateDataPoints(float power, float energyToday, float energyTotal) {
    addDataPoint(powerData, powerIndex, power);
    addDataPoint(energyTodayData, energyTodayIndex, energyToday);
    addDataPoint(energyTotalData, energyTotalIndex, energyTotal);
}

const Inverter::DataPoint* Inverter::getPowerData() const {
    return powerData; // Return a pointer to the power data array
}

int Inverter::getPowerDataSize() const {
    // Since powerIndex increments each time a new data point is added,
    // it effectively tracks the count of data points. However, if you're
    // using it also to overwrite old data in a circular buffer manner,
    // you might want to return bufferSize once it's fully populated.
    return min(powerIndex, bufferSize);
}

void Inverter::generateTestData() {
    // Clear existing data
    powerIndex = 0;

    // Generate test data for a 24-hour period, with data points every 5 minutes
    const int minutesPerDay = 24 * 60;
    const int interval = 5; // Data point interval in minutes
    const int dataPoints = minutesPerDay / interval;

    for (int i = 0; i < dataPoints; ++i) {
        // Simulate a power generation curve: low at night, peaks at midday
        int hour = (i * interval) / 60;
        float powerValue;

        // Example simplistic curve: quadratic, peaking at noon
        if (hour < 6 || hour > 18) { // Nighttime
            powerValue = 50; // Minimal power generation/consumption
        } else if (hour < 12) { // Morning ramp-up
            powerValue = 50 + (610.0f / 36.0f) * (hour - 6) * (hour - 6);
        } else { // Afternoon ramp-down
            powerValue = 50 + (610.0f / 36.0f) * (18 - hour) * (18 - hour);
        }

        // Fill the data point into the buffer
        unsigned long timestamp = (i * interval * 60); // Example timestamp, in seconds
        addDataPoint(powerData, powerIndex, powerValue);
        powerData[powerIndex % bufferSize].timestamp = timestamp; // Adjust timestamp directly for test data
        powerIndex++;
    }
}
