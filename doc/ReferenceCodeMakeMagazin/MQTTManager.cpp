#include "MQTTManager.h"

MQTTManager::MQTTManager(const char* broker, uint16_t port, const char* user, const char* pwd, DisplayManager& displayManager, Inverter& inverter, SerialCaptureLines& serialCapture)
: _broker(broker), _port(port), _user(user), _pwd(pwd), _displayManager(displayManager), _inverter(inverter), mqttClient(espClient), serialCapture(serialCapture)  {
    mqttClient.setServer(_broker, _port);
    mqttClient.setBufferSize(1024); 
    // WICHTIG: Die Buffer Size ist normaler Weise deutlich kleiner. Die Autodiscovery Templates sind zu groß für den Standard Buffer 
}

void MQTTManager::publish(const char* topic, const char* payload) {
    mqttClient.publish(topic, payload);
}

// HomeAssistant Autodiscovery Nachricht für den Power Sensor
// Man muss darauf achten, dass das "state_topic" dem topic entpricht, wo man die Sensordaten später veröffentlicht
// Sehr nützlich ist auch, dass man das Icon direlkt setzen kann.
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
    if ( mqttClient.connected() ) {
        bool publishSuccess;
        // Sende HomeAssistant Auto Discovery Daten für die Sensoren
        publishSuccess = publish("homeassistant/sensor/solar_inverter/power/config",configPower , true);
        publishSuccess = publish("homeassistant/sensor/solar_inverter/energy_today/config", configEnergyToday, true);
        publishSuccess = publish("homeassistant/sensor/solar_inverter/energy_total/config", configEnergyTotal, true);

        // ...
        publish("SolarInverterBridge/inverter/serial", _inverter.getInverterSerial().c_str());
        publish("SolarInverterBridge/inverter/power_W", String(_inverter.getInverterPowerNow_W()).c_str());
        publish("SolarInverterBridge/inverter/energy_today_kWh", String(_inverter.getInverterEnergyToday_kWh()).c_str());
        publish("SolarInverterBridge/inverter/energy_total_kWh", String(_inverter.getInverterEnergyTotal_kWh()).c_str());
        // ... 
        disconnect();
    } else {
        serialCapture.println("[DBG] MQTT Connection not available.");
    }
}


