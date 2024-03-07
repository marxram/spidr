#include "MQTTManager.h"

MQTTManager::MQTTManager(const char* broker, uint16_t port, const char* user, const char* pwd, DisplayManager& displayManager, Inverter& inverter)
: _broker(broker), _port(port), _user(user), _pwd(pwd), _displayManager(displayManager), _inverter(inverter), mqttClient(espClient) {
    mqttClient.setServer(_broker, _port);
}

void MQTTManager::reconnect(ActionData& action) {
    while (!mqttClient.connected()) {
        Serial.println("Attempting MQTT connection...");
        if (mqttClient.connect("MQTTClient", _user, _pwd)) {
            Serial.println("connected");
            action.resultDetails = "Connected";
            _displayManager.displayAction(action); // Optional: update display on successful connection
        } else {
            Serial.print("failed, rc=");
            Serial.print(mqttClient.state());
            Serial.println(" try again in 5 seconds");
            delay(5000);
        }
    }
}

void MQTTManager::disconnect() {
    mqttClient.disconnect();
    Serial.println("MQTT Disconnected");
    //_displayManager.displayAction("MQTT Disconnection", "", "Disconnected");
}

void MQTTManager::publish(const char* topic, const char* payload) {
    mqttClient.publish(topic, payload);
}


void MQTTManager::publishAllData() {
    ActionData action;
    action.name = "MQTT Sync";
    action.details = "Publish data";
    action.params[0] = "Broker: " + String(_broker);
    action.params[1] = "Port: " + String(_port);
    action.result = "In Progress";
    
    _displayManager.displayAction(action); // Initial display update

    if (!mqttClient.connected()) {
        reconnect(action); // Pass action by reference if you want to update it within reconnect
    }

    if (mqttClient.connected()) {
        action.result = "Connected";
        action.resultDetails = "Publishing...";
        _displayManager.displayAction(action); // Update display after connection
        
        // Publishing all fields using the updated InverterData struct
        // Publish data directly from the Inverter instance
        publish("SolarInverterBridge/inverter/serial", _inverter.getInverterSerial().c_str());
        publish("SolarInverterBridge/inverter/power_W", String(_inverter.getInverterPowerNow_W()).c_str());
        publish("SolarInverterBridge/inverter/energy_today_kWh", String(_inverter.getInverterEnergyToday_kWh()).c_str());
        publish("SolarInverterBridge/inverter/energy_total_kWh", String(_inverter.getInverterEnergyTotal_kWh()).c_str());
        publish("SolarInverterBridge/status/alarm", _inverter.getWebdataAlarm().c_str());
        publish("SolarInverterBridge/status/utime", _inverter.getWebdataUtime().c_str());
        publish("SolarInverterBridge/logger/serial", _inverter.getLoggerModuleID().c_str());
        publish("SolarInverterBridge/logger/version", _inverter.getLoggerSoftwareVersion().c_str());
        publish("SolarInverterBridge/logger/wifi_mode", _inverter.getLoggerWifiMode().c_str());
        publish("SolarInverterBridge/logger/ApSsid", _inverter.getLoggerApSsid().c_str());
        publish("SolarInverterBridge/logger/ApIp", _inverter.getLoggerApIp().c_str());
        publish("SolarInverterBridge/logger/ApMac", _inverter.getLoggerApMac().c_str());
        publish("SolarInverterBridge/logger/StaSsid", _inverter.getLoggerStaSsid().c_str());
        publish("SolarInverterBridge/logger/StaRssi", _inverter.getLoggerStaRssi().c_str());
        publish("SolarInverterBridge/logger/StaIp", _inverter.getLoggerStaIp().c_str());
        publish("SolarInverterBridge/logger/StaMac", _inverter.getLoggerStaMac().c_str());
        publish("SolarInverterBridge/remote-server/statusA", _inverter.getRemoteServerStatusA().c_str());
        publish("SolarInverterBridge/remote-server/statusB", _inverter.getRemoteServerStatusB().c_str());
        publish("SolarInverterBridge/remote-server/statusC", _inverter.getRemoteServerStatusC().c_str());
        publish("SolarInverterBridge/lastUpdateTimestamp", String(_inverter.getLastUpdateTimestamp()).c_str());
   
        disconnect();
        action.result = "Done";
        action.resultDetails = "Published";
    } else {
        action.result = "Failed";
        action.resultDetails = "Check connection";
    }
    
    _displayManager.displayAction(action); // Final display update
}
