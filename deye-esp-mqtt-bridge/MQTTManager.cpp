#include "MQTTManager.h"

MQTTManager::MQTTManager(const char* broker, uint16_t port, const char* user, const char* pwd, DisplayManager& displayManager, Inverter& inverter, SerialCaptureLines& serialCapture)
: _broker(broker), _port(port), _user(user), _pwd(pwd), _displayManager(displayManager), _inverter(inverter), mqttClient(espClient), serialCapture(serialCapture)  {
    mqttClient.setServer(_broker, _port);
    mqttClient.setBufferSize(1024); // Increase the buffer size to 512 bytes
}

void MQTTManager::reconnect(ActionData& action, uint8_t attempts) {
    uint8_t attempt = 0;
    while (!mqttClient.connected() && attempt < attempts) {
        attempt++;
        serialCapture.println("Attempting MQTT connection...");
        if (mqttClient.connect("MQTTClient", _user, _pwd)) {
            serialCapture.println("connected");
            action.resultDetails = "Connected";
_displayManager.displayAction(action); // Optional: update display on successful connection
        } else {
            serialCapture.print("failed, rc=");
            serialCapture.print(mqttClient.state());
            serialCapture.println(" try again in 5 seconds");
            delay(5000);
        }
    }
}

void MQTTManager::disconnect() {
    mqttClient.disconnect();
    serialCapture.println("MQTT Disconnected");
    //_displayManager.displayAction("MQTT Disconnection", "", "Disconnected");
}

void MQTTManager::publish(const char* topic, const char* payload) {
    mqttClient.publish(topic, payload);
}

// Configuration for the Power sensor
const char* configPower = R"({
    "unique_id": "solar_inverter_power",
    "device_class": "power",
    "suggested_display_precision": 0,
    "icon": "mdi:solar-power",
    "device": {
    "identifiers": ["deye_600_12345678"],
    "name": "Solar Inverter",
    "model": "EU 600W",
    "via_device": "esp-mqtt-bridge",
    "manufacturer": "Deye"},
    "name": "Solar Power",
    "state_topic": "SolarInverterBridge/inverter/power_W",
    "unit_of_measurement": "W",
    "value_template": "{{ value }}"
})";

// Configuration for the Energy Today sensor
const char* configEnergyToday = R"({
    "unique_id": "solar_inverter_energy_today",
    "device_class": "energy",
    "suggested_display_precision": 1,
    "icon": "mdi:solar-panel",
    "device": {
    "identifiers": ["deye_600_12345678"],
    "name": "Solar Inverter",
    "model": "EU 600W",
    "via_device": "esp-mqtt-bridge",
    "manufacturer": "Deye"},
    "name": "Solar Energy Today",
    "state_topic": "SolarInverterBridge/inverter/energy_today_kWh",
    "unit_of_measurement": "kWh",
    "value_template": "{{ value }}"
})";

// Configuration for the Energy Total sensor
const char* configEnergyTotal = R"({
    "unique_id": "solar_inverter_energy_total",
    "device_class": "energy",
    "suggested_display_precision": 1, 
    "icon": "mdi:sigma",
    "device": {
    "identifiers": ["deye_600_12345678"],
    "name": "Solar Inverter",
    "model": "EU 600W",
    "via_device": "esp-mqtt-bridge",
    "manufacturer": "Deye"},
    "name": "Solar Energy Total",
    "state_topic": "SolarInverterBridge/inverter/energy_total_kWh",
    "unit_of_measurement": "kWh",
    "value_template": "{{ value }}"
})";




void MQTTManager::publishAllData() {
    ActionData action;
    action.name = "MQTT Sync";
    action.details = "Sende Daten";
    action.params[0] = "Broker: " + String(_broker);
    action.params[1] = "Port:   " + String(_port);
    action.result = "In Arbeit";
    
_displayManager.displayAction(action); // Initial display update
    
    if (!mqttClient.connected()) { 
        reconnect(action, 3); // Pass action by reference if you want to update it within reconnect
    }
    
    if ( mqttClient.connected() ) {
        action.result = "Verbunden";
        action.resultDetails = "Sende...";
        _displayManager.displayAction(action); // Update display after connection

        bool publishSuccess;

        publishSuccess = mqttClient.publish("homeassistant/sensor/solar_inverter/power/config",configPower , true);
        if (!publishSuccess) {
            serialCapture.println("[ERR] Failed to publish config for Power sensor.");
        }

        publishSuccess = mqttClient.publish("homeassistant/sensor/solar_inverter/energy_today/config", configEnergyToday, true);
        if (!publishSuccess) {
            serialCapture.println("[ERR] Failed to publish config for Energy Today sensor.");
        } 

        publishSuccess = mqttClient.publish("homeassistant/sensor/solar_inverter/energy_total/config", configEnergyTotal, true);
        if (!publishSuccess) {
            serialCapture.println("[ERR] Failed to publish config for Energy Total sensor.");
        } 

        delay(100); // Wait for the config messages to be processed by the broker

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
        serialCapture.println("[DBG] MQTT Connection not available.");
        action.result = "Failed";
        action.resultDetails = "Check connection";
    }
    
    _displayManager.displayAction(action); // Final display update

}
