#include "Arduino.h"
#include "InverterUdp.h"

// ESP8266WiFi Built-In by Ivan Grokhotkov Version 1.0.0
#include <ESP8266WiFi.h> 
// AT Commands
#include <WiFiUdp.h>

#include <stdint.h>
#include <stdio.h> // For sprintf()
#include <stdlib.h> // For strtol()

#define MODBUS 0xA001  // CRC variable



// Constructor
InverterUdp::InverterUdp() {
    // as we are switching Nezworks the server IP might change over time. 
    udpLogin = "WIFIKIT-214028-READ";
    //char buffer[64];
    connected = false;
    String noResponse = "NoData";

    modbusIntro = "AT+INVDATA=8,";
    modbusOutro = "\n";
    modbusWriteToken = "0110";
    modbusReadToken = "0103";
    noResponse = "NORESPONSE";
    
}

bool InverterUdp::isconnected(){
    return connected;
}


String InverterUdp::inverter_readtime(){    
    //send_message("AT+WAP\n");
    String response;
    
    response = readModbus("0016", "0003");
    
    if (response != noResponse){
        Serial.print("Response was: ");
        Serial.println(response);
        connected = true; 
        String(buffer);

        // Parse if makes sense
        parseDateTime(response.c_str());

    } else {
        Serial.print("[ERROR] No Response in time or Parsing Error ");
        Serial.println(response);
        connected = false;  
    }


    //Sending Message: AT+INVDATA=8,0103 0022 0001 2400
    // response; +ok=0103 02 139C B51D

    // AT+INVDATA=8,01030023000175C0
    // +ok=0103 02 002C B999

    // AT+INVDATA=8,0103 0024 0001 C401
    // +ok=0103 02 0000 B844


    // ToDo: reformatting
    //String response = readModbus("0022", "0003");
    //+ok=010306139C002C00003203
    // Response == 010306139C002C00003203
    //      len  
    // 0103 06   13 9C 002C 0000 3203

    return response;



}


String InverterUdp::readModbus(String address, String  length){    

    String cmd = modbusReadToken+address+length;
    //Serial.println("Modbus Send Command : "+ cmd);
 
    //const char* hexDataStr = "01030022000";
    uint8_t binaryData[6];  // Ensure the size is half the length of hexDataStr
    hexStringToBytes(cmd.c_str(), binaryData, sizeof(binaryData));
    uint8_t crc_bytes[2];
    Modbus(binaryData, sizeof(binaryData), crc_bytes);
    
    
    String crc = byteToHexString(crc_bytes,2);
    //Serial.println("Modbus CRC: "+ crc);

    cmd = modbusIntro + cmd + crc + modbusOutro;

    Serial.println("Modbus Send Full Command : "+ cmd );

    send_message(cmd);

    String response = getResponse(true);


    if (response != noResponse){
        //Serial.print("Response was: ");
        //Serial.println(response);
        connected = true; 
        String(buffer);
    } else {
        Serial.print("[ERROR] No Response in time or Parsing Error! ");
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

    bool status = false;


    // Starting local listening port
    udp.begin(localPort);   
    
    Serial.println("----------------------------------------------------------");
    Serial.print("\n[INIT] >>>Begin UDP connection to ");
    Serial.print(udpServer);
    Serial.print("  Port: " );
    Serial.println(remotePort);

    send_message(udpLogin);

    // ToDo: check if error occured before setting connected
    
    String response = getResponse(false);
    if (response != noResponse){
        Serial.print("\n[INIT] >>> Response was: ");
        Serial.println(response);
        connected = true; 
        status =  true; 
    } else {
        Serial.print("[INIT] >>> [ERROR] No Response in time or Parsing error! ");
        Serial.println(response);
        connected = false;
        status = false; 
    }

    // Finalize handshake, no response needed
    send_message("+ok");
    Serial.println("[INIT] >>> Handshake complete");
    delay (100);

    return status;

}

String InverterUdp::getResponse(bool deleteNewlines){
    int delay_loop_ms = 200;
    String response = noResponse;

    int maxAttempts = udpTimeout_s *1000 / delay_loop_ms;
    
    for (int attempts = 0; attempts < maxAttempts; attempts++){
        int bytes = udp.parsePacket();
        
        if (bytes > 0) {
            //Serial.print("\nPacket received with size: " + String(bytes));
            int len = udp.read(buffer, 255);
            buffer[len] = 0; // Null-terminate the string
            
            response = String(buffer);
            
            // Check the response
            if(response.startsWith("+ERR=")) {
                // Handle the error case, log it and continue listening
                Serial.println("Error received: " + response);
                response = noResponse;
                break;
            } else if(response.startsWith("+ok=01")) {
                // Handle the valid case
                Serial.println("Valid MODBUS response received: " + response);
                // If newline characters should be removed, do so here
                
                if (deleteNewlines){
                    removeByte(buffer, 0x10, len);
                }
                
                if(String(buffer).startsWith("+ok=0103"))
                    response = String(buffer);
                else
                    response = noResponse;   
                break; // Exit the loop as the response is valid
            } else {
                // Handle unexpected cases
                Serial.println("Other response like Hello Message " + response);
                break; 
                //response = noResponse;
            }
        } else {
            Serial.print(".");
            delay(delay_loop_ms); // Wait for a short period before checking again
        }
    }
    
    return response;
}

bool InverterUdp::inverter_close(){
  
  send_message("AT+Q");
  delay(500); 
  Serial.println("[CLOSE] >>> Stopping local udp port");
  udp.stop();
  delay(500); 
  connected = false; 
  return false;
}

void InverterUdp::send_message(String message){
    //message.toLowerCase();

    //Serial.println("> Sending Message: "+ message);
    udp.beginPacket(udpServer.c_str(), remotePort);
    udp.print(message);
    udp.endPacket();
}


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

String InverterUdp::byteToHexString(const uint8_t* byteArray, size_t length) {
  String hexString = "";
  for(size_t i = 0; i < length; i++) {
    if(byteArray[i] < 16) {
      hexString += "0"; // pad with zero
    }
    hexString += String(byteArray[i], HEX);
  }
  hexString.toUpperCase();
  return hexString;
}

// Example usage:
// uint8_t crc[2] = {0x24, 0x00}; // or whatever your CRC calculation provides
// String crcString = byteToHexString(crc, 2);

void InverterUdp::removeByte(char* buffer, char byteToRemove, size_t bufferSize) {
    size_t newBufferIndex = 0;
    char newBuffer[256]; // Temporary buffer to hold bytes we want to keep
    
    for (size_t i = 0; i < bufferSize; ++i) {
        if (buffer[i] != byteToRemove) {
            // If the byte is not 0x10, add it to the new buffer
            newBuffer[newBufferIndex++] = buffer[i];
        }
    }

    // Optionally, copy the new buffer back into the original buffer
    for (size_t i = 0; i < newBufferIndex; ++i) {
        buffer[i] = newBuffer[i];
    }

    // Ensure the rest of the original buffer is filled with zeros or a sentinel value
    for (size_t i = newBufferIndex; i < bufferSize; ++i) {
        buffer[i] = 0;
    }
}


void InverterUdp::parseDateTime(const char* inputStr) {
    Serial.println("[PARSER] >>> Parsing: " + String(inputStr));
    
    Serial.println("[PARSER] >>> ASCII values of received string:");
    for(int i = 0; i < strlen(inputStr); i++) {
        Serial.print((int)inputStr[i]);
        Serial.print(" ");
    }
    Serial.println();

    String timestring = inputStr;
    
    // INTO    |size| year  | month | day   | hour | min  | sec  | CRC
    // 12345678|9 10| 11 12 | 13 14 | 15 16 | 17 18| 19 20| 21 22|  23
    // +ok=0103|0  6|  3  0 |  0  0 |  0  0 |  0  0|  0  0|  0  0|  2485


    if (timestring.startsWith("+ok=010306")){
        // seems to be a valid 6 bytes response Modbus response 

        int index = 10;
        String year_str = timestring.substring(index,index+2);
        index+=2; 
        String month_str = timestring.substring(index,index+2);
        index+=2;
        String day_str = timestring.substring(index,index+2);
        index+=2;
        String hour_str = timestring.substring(index,index+2);
        index+=2;
        String minute_str = timestring.substring(index,index+2);
        index+=2;
        String second_str = timestring.substring(index,index+2);
        index+=2;
        
        Serial.println("\nYear: "+ year_str +" Month: " + month_str + " Day:  -- Hour: " + hour_str + " Minute: " +minute_str+ " Second " + second_str);



    }


    // // Check if inputStr starts with "+ok="
    // if(strncmp(inputStr, "+ok=", 4) != 0) {
    //     Serial.println("[PARSER] >>> Invalid input string!");
    //     return;
    // }

    // // Get payload size
    // int payloadSize = strtol(inputStr + 8, NULL, 16);
    // Serial.println("[PARSER] >>> Expected payload size: " + String(payloadSize) + " bytes");

    // // Check if payload size is valid
    // if(payloadSize < 0 || inputStr[4 + 2 + payloadSize * 2] == '\0') {
    //     Serial.println("[PARSER] >>>  Invalid payload size or string length!");
    //     return;
    // }

    // // Parsing year, month, day, hour, minute, and second from the payload
    // int year   = strtol(inputStr + 10, NULL, 16) + 2000;
    // Serial.println("Parsed Year: " + String(year));
    // int month  = strtol(inputStr + 12, NULL, 16);
    // Serial.println("Parsed Month: " + String(month));
    // int day    = strtol(inputStr + 14, NULL, 16);
    // Serial.println("Parsed Day: " + String(day));
    // int hour   = strtol(inputStr + 16, NULL, 16);
    // Serial.println("Parsed Hour: " + String(hour));
    // int minute = strtol(inputStr + 18, NULL, 16);
    // Serial.println("Parsed Minute: " + String(minute));
    // int second = strtol(inputStr + 20, NULL, 16);
    // Serial.println("Parsed Second: " + String(second));
    // // Calculating Unix timestamp
    // long timestamp = calculateUnixTimestamp(year, month, day, hour, minute, second);

    // // Displaying Unix timestamp
    // Serial.print("Unix Timestamp: ");
    // Serial.println(timestamp);
}

long InverterUdp::calculateUnixTimestamp(int year, int month, int day, int hour, int minute, int second) {
    const int daysPerMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    // Calculate days since epoch [January 1, 1970]
    long days = (year - 1970) * 365L + (year - 1969) / 4;
    for(int i = 0; i < month - 1; ++i) {
        days += daysPerMonth[i];
    }
    if(month > 2 && year % 4 == 0) {
        ++days; // Add leap day
    }
    days += day - 1;

    // Convert to Unix timestamp (seconds since January 1, 1970)
    return ((days * 24L + hour) * 60 + minute) * 60 + second;
}

