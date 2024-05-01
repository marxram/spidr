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
    "via_device": "SPIDR",
    "manufacturer": "Deye"},
    "name": "Solar Power",
    "state_topic": "spidr/inverter/power_W",
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
    "via_device": "SPIDR",
    "manufacturer": "Deye"},
    "name": "Solar Energy Today",
    "state_topic": "spidr/inverter/energy_today_kWh",
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
    "via_device": "SPIDR",
    "manufacturer": "Deye"},
    "name": "Solar Energy Total",
    "state_topic": "spidr/inverter/energy_total_kWh",
    "unit_of_measurement": "kWh",
    "value_template": "{{ value }}"
})";

const char* configSpidrTemperature = R"({
    "unique_id": "spidr_esp_temperature",
    "device_class": "temperature",
    "suggested_display_precision": 0, 
    "icon": "mdi:thermometer",
    "device": {
    "identifiers": ["deye_600_12345678"],
    "name": "Solar Inverter",
    "model": "EU 600W",
    "via_device": "SPIDR",
    "manufacturer": "Deye"},
    "name": "ESP Temperature",
    "state_topic": "spidr/health/temperature_esp",
    "unit_of_measurement": "°C",
    "value_template": "{{ value }}"
})";

const char* configSpidrHeap = R"({
    "unique_id": "spidr_esp_freeheap",
    "suggested_display_precision": 0, 
    "icon": "mdi:memory",
    "device": {
    "identifiers": ["deye_600_12345678"],
    "name": "Solar Inverter",
    "model": "EU 600W",
    "via_device": "SPIDR",
    "manufacturer": "Deye"},
    "name": "ESP Free Heap",
    "state_topic": "spidr/health/freeheap_esp",
    "unit_of_measurement": "kB",
    "value_template": "{{ value }}"
})";

const char* configSpidrResetReason = R"({
    "unique_id": "spidr_esp_resetreason",
    "suggested_display_precision": 0, 
    "icon": "mdi:reload",
    "device": {
    "identifiers": ["deye_600_12345678"],
    "name": "Solar Inverter",
    "model": "EU 600W",
    "via_device": "SPIDR",
    "manufacturer": "Deye"},
    "name": "ESP Reset Reason",
    "state_topic": "spidr/health/resetreason_esp",
    "unit_of_measurement": "",
    "value_template": "{{ value }}"
})";

// Configuration for the Energy Total sensor
const char* configSpidrUptime = R"({
    "unique_id": "spidr_esp_uptime",
    "suggested_display_precision": 0, 
    "icon": "mdi:timer-sand",
    "device": {
    "identifiers": ["deye_600_12345678"],
    "name": "SPIDR",
    "model": "EU 600W",
    "via_device": "SPIDR",
    "manufacturer": "Deye"},
    "name": "ESP Uptime",
    "state_topic": "spidr/health/uptime_esp",
    "unit_of_measurement": "s",
    "value_template": "{{ value }}"
})";

// Configuration for the Energy Total sensor
const char* configSpidrUptimeString = R"({
    "unique_id": "spidr_esp_uptime", 
    "icon": "mdi:timer-sand",
    "device": {
    "identifiers": ["deye_600_12345678"],
    "name": "SPIDR",
    "model": "EU 600W",
    "via_device": "SPIDR",
    "manufacturer": "Deye"},
    "name": "ESP Uptime String",
    "state_topic": "spidr/health/uptime_string_esp",
    "unit_of_measurement": "",
    "value_template": "{{ value }}"
})";




void MQTTManager::publishSystemHealth(const SystemHealth& systemHealth){
ActionData action;
action.name = "MQTT Health";
action.details = "Sende Daten";
action.params[0] = "Broker: " + String(_broker);
action.params[1] = "Port:   " + String(_port);
action.result = "In Arbeit";

_displayManager.displayAction(action); // Initial display update

if (!mqttClient.connected()) { 
    reconnect(action, 3); // Pass action by reference if you want to update it within reconnect
}

if (!mqttClient.connected()) { 
        reconnect(action, 3); // Pass action by reference if you want to update it within reconnect
}
    
if ( mqttClient.connected() ) {
    action.result = "Verbunden";
    action.resultDetails = "Sende...";
    _displayManager.displayAction(action); // Update display after connection

    bool publishSuccess;

    // Only ESP32 has temperature Sensor builtin
    // #ifdef ESP32
    // publishSuccess = mqttClient.publish("homeassistant/sensor/spidr/temperature_esp/config",configSpidrTemperature , true);
    // if (!publishSuccess) {
    //     serialCapture.println("[ERR] Failed to publish config of ESP Temperature.");
    // }
    // #endif

    publishSuccess = mqttClient.publish("homeassistant/sensor/spidr/freeheap/config",configSpidrHeap , true);
    if (!publishSuccess) {
        serialCapture.println("[ERR] Failed to publish config for ESP Free Heap");
    }

    publishSuccess = mqttClient.publish("homeassistant/sensor/spidr/reset_reason/config",configSpidrResetReason , true);
    if (!publishSuccess) {
        serialCapture.println("[ERR] Failed to publish config for ESP Reset Reason.");
    }

    publishSuccess = mqttClient.publish("homeassistant/sensor/spidr/uptime/config",configSpidrUptime , true);
    if (!publishSuccess) {
        serialCapture.println("[ERR] Failed to publish config for ESP Uptime.");
    }

        publishSuccess = mqttClient.publish("homeassistant/sensor/spidr/uptime_string/config",configSpidrUptimeString , true);
    if (!publishSuccess) {
        serialCapture.println("[ERR] Failed to publish config for ESP Uptime.");
    }

    delay(100); // Wait for the config messages to be processed by the broker

    // Publishing all fields using the updated InverterData struct
    // Publish data directly from the Inverter instance
    // Only ESP32 has temperature Sensor builtin
    
    //#ifdef ESP32
    //publish("spidr/health/temperature_esp", String(systemHealth.getTemperature()).c_str());
    //#endif

    publish("spidr/health/freeheap_esp", String(systemHealth.getFreeRAM()).c_str());
    publish("spidr/health/resetreason_esp", String(systemHealth.getResetReason()).c_str());
    publish("spidr/health/uptime_esp", String(systemHealth.getUptimeSeconds()).c_str());
    publish("spidr/health/uptime_string_esp", String(systemHealth.getFormattedUptime()).c_str());
    }
}


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
        publish("spidr/inverter/serial", _inverter.getInverterSerial().c_str());
        publish("spidr/inverter/power_W", String(_inverter.getInverterPowerNow_W()).c_str());
        publish("spidr/inverter/energy_today_kWh", String(_inverter.getInverterEnergyToday_kWh()).c_str());
        publish("spidr/inverter/energy_total_kWh", String(_inverter.getInverterEnergyTotal_kWh()).c_str());
        publish("spidr/status/alarm", _inverter.getWebdataAlarm().c_str());
        publish("spidr/status/utime", _inverter.getWebdataUtime().c_str());
        publish("spidr/logger/serial", _inverter.getLoggerModuleID().c_str());
        publish("spidr/logger/version", _inverter.getLoggerSoftwareVersion().c_str());
        publish("spidr/logger/wifi_mode", _inverter.getLoggerWifiMode().c_str());
        publish("spidr/logger/ApSsid", _inverter.getLoggerApSsid().c_str());
        publish("spidr/logger/ApIp", _inverter.getLoggerApIp().c_str());
        publish("spidr/logger/ApMac", _inverter.getLoggerApMac().c_str());
        publish("spidr/logger/StaSsid", _inverter.getLoggerStaSsid().c_str());
        publish("spidr/logger/StaRssi", _inverter.getLoggerStaRssi().c_str());
        publish("spidr/logger/StaIp", _inverter.getLoggerStaIp().c_str());
        publish("spidr/logger/StaMac", _inverter.getLoggerStaMac().c_str());
        publish("spidr/remote-server/statusA", _inverter.getRemoteServerStatusA().c_str());
        publish("spidr/remote-server/statusB", _inverter.getRemoteServerStatusB().c_str());
        publish("spidr/remote-server/statusC", _inverter.getRemoteServerStatusC().c_str());
        publish("spidr/lastUpdateTimestamp", String(_inverter.getLastUpdateTimestamp()).c_str());
   
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
