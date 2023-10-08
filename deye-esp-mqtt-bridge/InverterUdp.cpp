#include "Arduino.h"
#include "InverterUdp.h"

// ESP8266WiFi Built-In by Ivan Grokhotkov Version 1.0.0
#include <ESP8266WiFi.h> 
// AT Commands
#include <WiFiUdp.h>

#include <stdint.h>

#define MODBUS 0xA001  // CRC variable



// Constructor
InverterUdp::InverterUdp() {
    // as we are switching Nezworks the server IP might change over time. 
    udpLogin = "WIFIKIT-214028-READ";
    //char buffer[64];
    connected = false;
    String noResponse = "NoData";
}

bool InverterUdp::isconnected(){
    return connected;
}


String InverterUdp::inverter_readtime(){    
    //send_message("AT+WAP\n");
    
    String response = readModbus("0022", "0001");
    
    if (response != noResponse){
        Serial.print("Response was: ");
        Serial.println(response);
        connected = true; 
        String(buffer);
    } else {
        Serial.print("[ERROR] No Response in time! ");
        Serial.println(response);
        connected = false;  
    }

    // ToDo: reformatting
    return response;
}


String InverterUdp::readModbus(String address, String  length){    
    
    String cmd = "0103"+address+length;
    Serial.println("Modbus Send Command : "+ cmd);
    String crc = ModbusCrc(cmd.c_str());
    Serial.println("Modbus CRC: "+ crc);
 
    cmd = cmd +crc;

    Serial.println("Modbus Send Full Command : "+ cmd );

    send_message(cmd);

    String response = getResponse();


    if (response != noResponse){
        Serial.print("Response was: ");
        Serial.println(response);
        connected = true; 
        String(buffer);
    } else {
        Serial.print("[ERROR] No Response in time! ");
        Serial.println(response);
        connected = false;  
    }

    // ToDo: reformatting
    return response;
}





////////////////////////////////////////////////////////////////////
// UDP Client Section
bool InverterUdp::inverter_connect(String udpSrv, int remPort, int locPort, int timeOut){
    
    remotePort = locPort;
    localPort = remPort;
    udpServer = udpSrv;
    udpTimeout_s = timeOut;

    // Starting local listening port
    udp.begin(localPort);   
    
    Serial.println("----------------------------------------------------------");
    Serial.print("\nBegin UDP connection to ");
    Serial.print(udpServer);
    Serial.print("  Port: " );
    Serial.println(remotePort);

    send_message(udpLogin);

    // ToDo: check if error occured before setting connected
    
    String response = getResponse();
    if (response != noResponse){
        Serial.print("Response was: ");
        Serial.println(response);
        connected = true; 
        return true; 
    } else {
        Serial.print("[ERROR] No Response in time! ");
        Serial.println(response);
        connected = false;
        return false; 
    }
}

String InverterUdp::getResponse(){
    int delay_loop_ms = 200;
    String response = noResponse;

 //   for (int attempts = 0; (attempts <= (udpTimeout_s*1000/delay_loop_ms)) && !responseReceived ; attempts++){
    for (int attempts = 0; attempts < 10 ; attempts++){
        //Serial.println("Attempts: " + String(attempts) +));
        int bytes = udp.parsePacket();
        
        if (bytes > 0) {
            Serial.print("\nPacket received with size: " + String(bytes));
            int len = udp.read(buffer, 255);
            // adding "0" byte char at the end of the String
            buffer[len] = 0;
            response = String(buffer);
            Serial.println("  Content: " + response );
            break;
        }else {
            Serial.print(".");
            delay(delay_loop_ms); // Wait for a short period before checking again
        }
    }

    return response;
}

bool InverterUdp::inverter_close(){
  Serial.println("Stopping local udp port");
  udp.stop();
  delay(500); 
  connected = false; 
  return false;
}

void InverterUdp::send_message(String message){
    Serial.println("> Sending Message: "+ message);

    udp.beginPacket(udpServer.c_str(), remotePort);
    udp.print(message);
    udp.endPacket();
}



const char* InverterUdp::ModbusCrc(const char* dataStr) {
    static char outputStr[5];  // 4 characters for the hex value + 1 for the null terminator
    uint16_t crc = 0xFFFF;
    uint8_t dataLength = strlen(dataStr);

    for (uint8_t dataIndex = 0; dataIndex < dataLength; dataIndex++) {
        crc = crc ^ (uint16_t) dataStr[dataIndex];
        for (uint8_t i = 0; i < 8; i++) {
            if (crc & 0x0001) {
                crc = (crc >> 1) ^ MODBUS;
            } else {
                crc = crc >> 1;
            }
        }
    }

    // Convert the CRC to a string and return it
    sprintf(outputStr, "%04X", crc);
    return outputStr;
}

// Example usage:
// const char* dataStr = "someData";
// const char* outputStr = ModbusCrc(dataStr);



// Function to convert a hex string to binary data
void InverterUdp::hexStringToBytes(const char* hexString, uint8_t* byteData, uint8_t byteDataLength) {
    for (uint8_t i = 0; i < byteDataLength; i++) {
        char byteChars[3] = {hexString[i*2], hexString[i*2 + 1], '\0'};
        byteData[i] = (uint8_t) strtol(byteChars, NULL, 16);
    }
}

// Modified Modbus CRC function to accept a length instead of using strlen()
void InverterUdp::Modbus(const uint8_t* data, uint8_t dataLength, uint8_t* output) {
    uint16_t crc = 0xFFFF;
    for (uint8_t dataIndex = 0; dataIndex < dataLength; dataIndex++) {
        crc = crc ^ (uint16_t) data[dataIndex];
        for (uint8_t i = 0; i < 8; i++) {
            if (crc & 0x0001) {
                crc = (crc >> 1) ^ MODBUS;
            } else {
                crc = crc >> 1;
            }
        }
    }
    output[0] = (uint8_t)(crc & 0xFF);
    output[1] = (uint8_t)(crc >> 8);
}

// Example usage:
// const char* hexDataStr = "01030022000";
// uint8_t binaryData[5];  // Ensure the size is half the length of hexDataStr
// hexStringToBytes(hexDataStr, binaryData, sizeof(binaryData));
// uint8_t output[2];
// Modbus(binaryData, sizeof(binaryData), output);