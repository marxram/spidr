#ifndef PreferencesManager_h
#define PreferencesManager_h

#include <Preferences.h>
#include "SerialCaptureLines.h"

class PreferencesManager {
public:
    void begin();
    void end();

    // Methods to get preferences
    String getHomeSSID();
    String getHomeKey();
    String getInverterSSID();
    String getInverterKey();
    String getRelaisSSID();
    String getRelaisKey();
    
    
    // MQTT Preferences
    String getMqttBrokerHost();
    int getMqttBrokerPort();
    String getMqttBrokerUser();
    String getMqttBrokerPwd();
    String getMqttBrokerMainTopic();

    // Web Access Credentials
    String getInverterWebUser();
    String getInverterWebPwd();
    String getRelaisWebUser();
    String getRelaisWebPwd();

    // Configuration for NTP
    String getNtpServerA();
    String getNtpServerB();
    bool getIsNtpActive();

    int getNtpGmtOffset();
    int getNtpDstOffset();

    // Timing behavior
    int getTimingStayInHomeNetwork_MS();
    int getTimingStayInHomeNetworkFirstBoot_MS();
    int getTimingStayInApMode_MS();

    int getTimingWiFiConnectWaitDuration_S(); // WIFI_CONNECT_TIME_WINDOW_S
    int getTimingInverterOfflineTimeout_S(); // INVERTER_OFFLINE_TIMEOUT_SECONDS 6000 // 100 Minutes

   // Timing behavior
    int getTimingDisplayDurtationPower_MS();
    int getTimingDisplayDurtationEnergyToday_MS();
    int getTimingDisplayDurtationEnergyTotal_MS();
    int getTimingDisplayDurtationEnergyTime_MS();
    int getTimingDisplayDurtationEnergyGraph_MS();

    String getApSSID();
    String getApKey();


    // Setters

    // Configuration for NTP
    void setNtpServerA(const String& value);
    void setNtpServerB(const String& value);
    void setIsNtpActive(bool value);

    void setNtpGmtOffset(int value);
    void setNtpDstOffset(int value);

    // Timing behavior
    void setTimingStayInHomeNetwork_MS(int value);
    void setTimingStayInHomeNetworkFirstBoot_MS(int value);
    void setTimingStayInApMode_MS(int value);

    void setTimingWiFiConnectWaitDuration_S(int value); // WIFI_CONNECT_TIME_WINDOW_S
    void setTimingInverterOfflineTimeout_S(int value); // INVERTER_OFFLINE_TIMEOUT_SECONDS 6000 // 100 Minutes

   // Timing behavior
    void setTimingDisplayDurtationPower_MS(int value);
    void setTimingDisplayDurtationEnergyToday_MS(int value);
    void setTimingDisplayDurtationEnergyTotal_MS(int value);
    void setTimingDisplayDurtationEnergyTime_MS(int value);
    void setTimingDisplayDurtationEnergyGraph_MS(int value);



    // Methods to set preferences
    void setHomeSSID(const String& value);
    void setHomeKey(const String& value);
    void setInverterSSID(const String& value);
    void setInverterKey(const String& value);
    void setRelaisSSID(const String& value);
    void setRelaisKey(const String& value);

    void setMqttBrokerHost(const String& value);
    void setMqttBrokerPort(int value);
    void setMqttBrokerUser(const String& value);
    void setMqttBrokerPwd(const String& value);
    void setMqttBrokerMainTopic(const String& value);

    void setInverterWebUser(const String& value);
    void setInverterWebPwd(const String& value);
    void setRelaisWebUser(const String& value);
    void setRelaisWebPwd(const String& value);

    void setApSSID(const String& value);
    void setApKey(const String& value);


private:
    Preferences preferences;
    void checkAndInitDefaults();
};

#endif
