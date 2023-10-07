#include "Arduino.h"
#include "InverterUdp.h"

// ESP8266WiFi Built-In by Ivan Grokhotkov Version 1.0.0
#include <ESP8266WiFi.h> 
// AT Commands
#include <WiFiUdp.h>



// Constructor
InverterUdp::InverterUdp(String udpSrv, int remPort, int locPort) {

    udpLogin = "WIFIKIT-214028-READ";
    localPort = locPort;
    remotePort = remPort; 
    
    //char buffer[64];
    udpServer = udpSrv;    
    connected = false;

    udpTimeout_s = 10;
}

InverterUdp::InverterUdp(String udpSrv) {
    const int udpServerPort = 50000; // manual port
    const int udpLocalPort = 48899; // Fixed port of deye inverter
    
    InverterUdp(udpSrv, udpServerPort, udpLocalPort);
}


bool InverterUdp::isconnected(){
    return connected;
}



String InverterUdp::inverter_readtime(){
    bool responseReceived = false;
    
    udp.beginPacket(udpServer.c_str(), localPort);
    udp.print("AT+WAP\n");
    udp.endPacket();     

    //while (millis() - startTime < udpTimeout) {
    for (int attempts = 0; attempts < 20 ; attempts++){
      int packets = udp.parsePacket();
      if (packets > 0) {
        Serial.println("\nPacket received: " + String(packets));

        int len = udp.read(buffer, 255);
        buffer[len] = 0;
        Serial.println("Received: " + String(buffer));
      }else {
        Serial.print(".");
        delay(500); // Wait for a short period before checking again
      }
    }
    return String(buffer);
}


////////////////////////////////////////////////////////////////////
// UDP Client Section
bool InverterUdp::inverter_connect(){
    bool responseReceived = false;
    //bool InverterUdp::udp_initialize_connection(String server, int port, int timeout_s){
    udp.begin(localPort);
    Serial.print("\nBegin UDP connection to ");
    Serial.print(udpServer);
    Serial.print("  Port: " );
    Serial.println(remotePort);
    

      udp.beginPacket(udpServer.c_str(), remotePort);
      udp.print(udpLogin); // ohne '\n'
      udp.endPacket();
      
      Serial.print("UDP Login sent: ");
      Serial.println(udpLogin);

      delay(1000);
    
      //while (millis() - startTime < udpTimeout) {
      for (int attempts = 0; attempts <= udpTimeout_s ; attempts++){
        responseReceived = false;
        int packets = udp.parsePacket();
        if (packets > 0) {
          Serial.println("\nPacket received: " + String(packets));

          int len = udp.read(buffer, 255);
          buffer[len] = 0;
          Serial.println("Received: " + String(buffer));
          
          //ToDo: Check if the packet was what we are looking for 
          return true;

        }else {
          Serial.print(".");
          delay(1000); // Wait for a short period before checking again
        }
      }
    return false;
}

bool InverterUdp::inverter_close(){
  Serial.println("Stopping local udp port");
  udp.stop();
  delay(500); 
  connected = false; 
  return false;
}

