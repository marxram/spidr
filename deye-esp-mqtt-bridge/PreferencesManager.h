#ifndef PreferencesManager_h
#define PreferencesManager_h

#include <Preferences.h>

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


private:
    Preferences preferences;
    void checkAndInitDefaults();
};

#endif
