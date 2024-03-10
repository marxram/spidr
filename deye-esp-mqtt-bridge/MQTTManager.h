#ifndef MQTTMANAGER_H
#define MQTTMANAGER_H

#include <Arduino.h>

#define MQTT_MAX_PACKET_SIZE 1024 // Giving more space for MQTT packets
#include <PubSubClient.h>
#include "DisplayManager.h" // Ensure DisplayManager is accessible
#include "Inverter.h" // Ensure Inverter is accessible
#include <WiFiClient.h>


class MQTTManager {
public:
    MQTTManager(const char* broker, uint16_t port, const char* user, const char* pwd, DisplayManager& displayManager, Inverter& inverter);
    void publishAllData(); // Single method to publish all predefined topics

private:
    WiFiClient espClient;
    PubSubClient mqttClient;
    DisplayManager& _displayManager;

    const char* _broker;
    uint16_t _port;
    const char* _user;
    const char* _pwd;

    Inverter& _inverter; // Reference to Inverter instance

    void reconnect(ActionData& action);
    void disconnect();

    void publish(const char* topic, const char* payload); // Private method to publish a single topic
};

#endif // MQTTMANAGER_H
