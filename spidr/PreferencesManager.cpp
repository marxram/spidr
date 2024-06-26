// Include the header file
#include "PreferencesManager.h"
#include "config.h"

#ifdef USE_SECRETS_FILE_AS_DEFAULT
    #include "arduino_secrets.h"
#endif


void PreferencesManager::begin() {
    preferences.begin("my-app", false); // Use a unique namespace for your app
    checkAndInitDefaults();
}

void PreferencesManager::end() {
    preferences.end();
}

// Initialize with default values if it's the first boot
void PreferencesManager::checkAndInitDefaults() {
    if (preferences.getBool("firstBoot", true)) {
    #ifdef USE_SECRETS_FILE_AS_DEFAULT
        // Set default values for all settings here
        preferences.putString("homeSSID", SECRET_WIFI_HOME_SSID);
        preferences.putString("homeKey", SECRET_WIFI_HOME_KEY);
        preferences.putString("inverterSSID", SECRET_WIFI_INVERTER_SSID);
        preferences.putString("inverterKey", SECRET_WIFI_INVERTER_KEY);
        preferences.putString("relaisSSID", SECRET_WIFI_RELAIS_SSID);
        preferences.putString("relaisKey", SECRET_WIFI_RELAIS_KEY);
        preferences.putString("mqttBrokerHost", SECRET_MQTT_BROKER_HOST);
        preferences.putInt("mqttBrokerPort", SECRET_MQTT_BROKER_PORT);
        preferences.putString("mqttBrokerUser", SECRET_MQTT_BROKER_USER);
        preferences.putString("mqttBrokerPwd", SECRET_MQTT_BROKER_PWD);
        preferences.putString("mqttBrokerMainTopic", SECRET_MQTT_BROKER_MAINTOPIC);
        preferences.putString("inverterWebUser", SECRET_INVERTER_WEBACCESS_USER);
        preferences.putString("inverterWebPwd", SECRET_INVERTER_WEBACCESS_PWD);
        preferences.putString("relaisWebUser", SECRET_RELAIS_WEBACCESS_USER);
        preferences.putString("relaisWebPwd", SECRET_RELAIS_WEBACCESS_PWD);
    #else
        // Set default values for all settings here
        preferences.putString("homeSSID", "YOUR-HOME-WIFI");
        preferences.putString("homeKey", "YOUR-WIFI-PASSKEY");
        preferences.putString("inverterSSID", "AP_12345678");
        preferences.putString("inverterKey", "12345678");
        preferences.putString("relaisSSID", "EM_12345678");
        preferences.putString("relaisKey", "check-sticker");
        preferences.putString("mqttBrokerHost", "BROKER-IP-or-Hostname");
        preferences.putInt("mqttBrokerPort", 1883);
        preferences.putString("mqttBrokerUser", "Client-USER");
        preferences.putString("mqttBrokerPwd", "Client-PWD");
        preferences.putString("mqttBrokerMainTopic", "stats/energy/deye/");
        preferences.putString("inverterWebUser", "admin");
        preferences.putString("inverterWebPwd", "admin");
        preferences.putString("relaisWebUser", "check-Sticker");
        preferences.putString("relaisWebPwd", "check-sticker");

    #endif

        preferences.putBool("firstBoot", false); // Mark first boot as complete
    }
}

// // Getters
String PreferencesManager::getHomeSSID() { 
    return preferences.getString("homeSSID"); 
}
String PreferencesManager::getHomeKey() { 
    return preferences.getString("homeKey"); 
}
String PreferencesManager::getInverterSSID() { 
    return preferences.getString("inverterSSID"); 
}
String PreferencesManager::getInverterKey() { 
    return preferences.getString("inverterKey"); 
}
String PreferencesManager::getRelaisSSID() { 
    return preferences.getString("relaisSSID"); 
}
String PreferencesManager::getRelaisKey() { 
    return preferences.getString("relaisKey"); 
}

String PreferencesManager::getMqttBrokerHost() {
    return preferences.getString("mqttBrokerHost");
}

int PreferencesManager::getMqttBrokerPort() {
    return preferences.getInt("mqttBrokerPort", 1883); // Default port
}

String PreferencesManager::getMqttBrokerUser() {
    return preferences.getString("mqttBrokerUser");
}

String PreferencesManager::getMqttBrokerPwd() {
    return preferences.getString("mqttBrokerPwd");
}

String PreferencesManager::getMqttBrokerMainTopic() {
    return preferences.getString("mqttTopic");
}

void PreferencesManager::setMqttBrokerHost(const String& value) {
    preferences.putString("mqttBrokerHost", value);
}

void PreferencesManager::setMqttBrokerPort(int value) {
    preferences.putInt("mqttBrokerPort", value);
}

void PreferencesManager::setMqttBrokerUser(const String& value) {
    preferences.putString("mqttBrokerUser", value);
}

void PreferencesManager::setMqttBrokerPwd(const String& value) {
    preferences.putString("mqttBrokerPwd", value);
}

void PreferencesManager::setMqttBrokerMainTopic(const String& value) {
    preferences.putString("mqttTopic", value);
}

// Web Access Credentials
String PreferencesManager::getInverterWebUser() {
    return preferences.getString("inverterWebUser");
}

String PreferencesManager::getInverterWebPwd() {
    return preferences.getString("inverterWebPwd");
}

String PreferencesManager::getRelaisWebUser() {
    return preferences.getString("relaisWebUser");
}

String PreferencesManager::getRelaisWebPwd() {
    return preferences.getString("relaisWebPwd");
}




// // Setters
void PreferencesManager::setHomeSSID(const String& value) { 
    preferences.putString("homeSSID", value); 
}

void PreferencesManager::setHomeKey(const String& value) { 
    preferences.putString("homeKey", value); 
}

void PreferencesManager::setInverterSSID(const String& value) { 
    preferences.putString("inverterSSID", value); 
}

void PreferencesManager::setInverterKey(const String& value) { 
    preferences.putString("inverterKey", value); 
}

void PreferencesManager::setRelaisSSID(const String& value) { 
    preferences.putString("relaisSSID", value); 
}

void PreferencesManager::setRelaisKey(const String& value) { 
    preferences.putString("relaisKey", value); 
}

void PreferencesManager::setInverterWebUser(const String& value) {
    preferences.putString("inverterWebUser", value);
}

void PreferencesManager::setInverterWebPwd(const String& value) {
    preferences.putString("inverterWebPwd", value);
}

void PreferencesManager::setRelaisWebUser(const String& value) {
    preferences.putString("relaisWebUser", value);
}

void PreferencesManager::setRelaisWebPwd(const String& value) {
    preferences.putString("relaisWebPwd", value);
}


// New Configuration Values
// Configuration for NTP
// Configuration for NTP
String PreferencesManager::getNtpServerA() {
    return preferences.getString("ntpA");
}

String PreferencesManager::getNtpServerB() {
    return preferences.getString("ntpB");
}

bool PreferencesManager::getIsNtpActive() {
    return preferences.getBool("ntpActive");
}

int PreferencesManager::getNtpGmtOffset() {
    return preferences.getInt("ntpGmtOff");
}

int PreferencesManager::getNtpDstOffset() {
    return preferences.getInt("ntpDstOff");
}

// Timing behavior
int PreferencesManager::getTimingStayInHomeNetwork_MS() {
    return preferences.getInt("homeNetMS");
}

int PreferencesManager::getTimingStayInHomeNetworkFirstBoot_MS() {
    return preferences.getInt("homeFirstBootMS");
}

int PreferencesManager::getTimingStayInApMode_MS() {
    return preferences.getInt("apModeMS");
}

int PreferencesManager::getTimingWiFiConnectWaitDuration_S() {
    return preferences.getInt("wifiWaitS");
}

int PreferencesManager::getTimingInverterOfflineTimeout_S() {
    return preferences.getInt("invOffTimeoutS");
}

int PreferencesManager::getTimingDisplayDurtationPower_MS() {
    return preferences.getInt("dispPowerMS");
}

int PreferencesManager::getTimingDisplayDurtationEnergyToday_MS() {
    return preferences.getInt("dispEnergyTodayMS");
}

int PreferencesManager::getTimingDisplayDurtationEnergyTotal_MS() {
    return preferences.getInt("dispEnergyTotalMS");
}

int PreferencesManager::getTimingDisplayDurtationEnergyTime_MS() {
    return preferences.getInt("dispEnergyTimeMS");
}

int PreferencesManager::getTimingDisplayDurtationEnergyGraph_MS() {
    return preferences.getInt("dispEnergyGraphMS");
}

// Setters for NTP Configuration
void PreferencesManager::setNtpServerA(const String& value) {
    preferences.putString("ntpA", value);
}

void PreferencesManager::setNtpServerB(const String& value) {
    preferences.putString("ntpB", value);
}

void PreferencesManager::setIsNtpActive(bool value) {
    preferences.putBool("ntpActive", value);
}

void PreferencesManager::setNtpGmtOffset(int value) {
    preferences.putInt("ntpGmtOff", value);
}

void PreferencesManager::setNtpDstOffset(int value) {
    preferences.putInt("ntpDstOff", value);
}

// Setters for Timing Behavior
void PreferencesManager::setTimingStayInHomeNetwork_MS(int value) {
    preferences.putInt("homeNetMS", value);
}

void PreferencesManager::setTimingStayInHomeNetworkFirstBoot_MS(int value) {
    preferences.putInt("homeFirstBootMS", value);
}

void PreferencesManager::setTimingStayInApMode_MS(int value) {
    preferences.putInt("apModeMS", value);
}

void PreferencesManager::setTimingWiFiConnectWaitDuration_S(int value) {
    preferences.putInt("wifiWaitS", value);
}

void PreferencesManager::setTimingInverterOfflineTimeout_S(int value) {
    preferences.putInt("invOffTimeoutS", value);
}

void PreferencesManager::setTimingDisplayDurtationPower_MS(int value) {
    preferences.putInt("dispPowerMS", value);
}

void PreferencesManager::setTimingDisplayDurtationEnergyToday_MS(int value) {
    preferences.putInt("dispEnergyTodayMS", value);
}

void PreferencesManager::setTimingDisplayDurtationEnergyTotal_MS(int value) {
    preferences.putInt("dispEnergyTotalMS", value);
}

void PreferencesManager::setTimingDisplayDurtationEnergyTime_MS(int value) {
    preferences.putInt("dispEnergyTimeMS", value);
}

void PreferencesManager::setTimingDisplayDurtationEnergyGraph_MS(int value) {
    preferences.putInt("dispEnergyGraphMS", value);
}


void PreferencesManager::setApSSID(const String& value) { 
    preferences.putString("apSSID", value); 
}

void PreferencesManager::setApKey(const String& value) { 
    preferences.putString("apKey", value); 
}

String PreferencesManager::getApSSID() { 
    return preferences.getString("apSSID"); 
}
String PreferencesManager::getApKey() { 
    return preferences.getString("apKey"); 
}