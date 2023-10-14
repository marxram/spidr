#include "Arduino.h"
#include "InverterUdp.h"

// ESP8266WiFi Built-In by Ivan Grokhotkov Version 1.0.0
#include <ESP8266WiFi.h> 
// AT Commands
#include <WiFiUdp.h>

#include <stdint.h>
#include <stdio.h> // For sprintf()
#include <stdlib.h> // For strtol()
#include "RTClib.h"
#include <TimeLib.h>

#define MODBUS 0xA001  // CRC variable

// Constructor
InverterUdp::InverterUdp() {
    // as we are switching Nezworks the server IP might change over time. 
    udpLogin = "WIFIKIT-214028-READ";
    //char buffer[64];
    connected = false;
    String noResponse = "NoData";

    modbusIntro = "AT+INVDATA=";
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
    String RESP_TIME_UNSET = "+ok=0103063000000000002485";
    String response;
    
    // Read 0x0003 bytes after address 0x0016 
    response = readModbus("0016", "0003");
    
    if (response != noResponse){
        Serial.print("Response was: ");
        Serial.println(response);
        connected = true; 
        //String(buffer);

        if (response.startsWith(RESP_TIME_UNSET)){
            Serial.println("Inverter Time is unset ");
        }else{
            // Parse if makes sense
            Serial.println("Checking Inverter Time >" + response + "<");
            parseDateTime(response);
        }
        
    } else {
        Serial.print("[ERROR] No Response in time or Parsing Error ");
        Serial.println(response);
        connected = false;  
    }
    return response;
}


String InverterUdp::inverter_settime(unsigned long epochTime){    
    tmElements_t tm;
    breakTime(epochTime, tm);

    String time_reg = decToHex((tm.Year + 1970) % 100) 
            + decToHex(tm.Month)
            + decToHex(tm.Day)
            + decToHex(tm.Hour)
            + decToHex(tm.Minute)
            + decToHex(tm.Second);
    
    Serial.print("Time String: ");
    Serial.println(time_reg);
    
    // "0016000306170A0E101409" 
    //Funktionierendes OPaket: 
    //AT+INVDATA=15,0110 0016 0003 06 170A0E101409 5007
    //AT+INVDATA=15,0110 0016 0003 06 170A0E101409 61CC --> Checksumme ist falsch! ToDo
    String testtime = "170A0E101409";

    String response;
    
    // Write 3 time registers 0x0003 bytes after address 0x0016 
    //response = writeModbus("0016", "0003", time_reg, "06");
    response = writeModbus("0016", "0003", testtime, "06");

    
    //String year_monstring = decToHex((tm.Year + 1970) % 100) + decToHex(tm.Month);
    
    //Serial.print("year_monstring String: ");
    //Serial.println(year_monstring);

    //response = writeModbus("0016", "0001", year_monstring, "02");


    if (response != noResponse){
        Serial.print("Response was: ");
        Serial.println(response);
        connected = true; 
        //String(buffer);

        /*
        if (response.startsWith(RESP_TIME_UNSET)){
            Serial.println("Inverter Time is unset ");
        }else{
            // Parse if makes sense
            Serial.println("Checking Inverter Time >" + response + "<");
            parseDateTime(response);
        }
        */
        
    } else {
        Serial.print("[ERROR] No Response in time or Parsing Error ");
        Serial.println(response);
        connected = false;  
    }
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

    //ToDo:  8 is for most commands OK, but needs calculation
    cmd = modbusIntro+ "8," + cmd + crc + modbusOutro;

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

String InverterUdp::writeModbus(String address, String  length, String payload, String payloadlength){    

    
    String message =  address+length+payloadlength+payload;
    String cmd = modbusWriteToken+message;

    //Serial.println("Modbus Send Command : "+ cmd);
 
    //const char* hexDataStr = "01030022000";
    uint8_t binaryData[6];  // Ensure the size is half the length of hexDataStr
    hexStringToBytes(cmd.c_str(), binaryData, sizeof(binaryData));
    uint8_t crc_bytes[2];
    Modbus(binaryData, sizeof(binaryData), crc_bytes);
    
    
    String crc = byteToHexString(crc_bytes,2);
    //Serial.println("Modbus CRC: "+ crc);
    String msg_length = String(message.length());
    
    Serial.println("Command Length: "+ msg_length);
    
    Serial.println("PayloadLength Length: "+ payloadlength);

    //cmd = modbusIntro + msg_length + "," + cmd + crc + modbusOutro;

    cmd = modbusIntro + "15," + cmd + crc + modbusOutro;



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


void InverterUdp::parseDateTime(String timestring) {
    
    // INTO    |size| year  | month | day   | hour | min  | sec  | CRC
    // 12345678|9 10| 11 12 | 13 14 | 15 16 | 17 18| 19 20| 21 22|  23
    // +ok=0103|0  6|  3  0 |  0  0 |  0  0 |  0  0|  0  0|  0  0|  2485

    if (timestring.startsWith("+ok=010306")){
    int index = 10;

    int year, month, day, hour, minute, second;
    bool isValidYear = hexStringToDec(timestring.substring(index, index + 2), year);
    index += 2; 
    bool isValidMonth = hexStringToDec(timestring.substring(index, index + 2), month);
    index += 2;
    bool isValidDay = hexStringToDec(timestring.substring(index, index + 2), day);
    index += 2;
    bool isValidHour = hexStringToDec(timestring.substring(index, index + 2), hour);
    index += 2;
    bool isValidMinute = hexStringToDec(timestring.substring(index, index + 2), minute);
    index += 2;
    bool isValidSecond = hexStringToDec(timestring.substring(index, index + 2), second);
    
    // Additional basic validations for the datetime components
    isValidYear = isValidYear && (year >= 0 && year <= 99);
    isValidMonth = isValidMonth && (month >= 1 && month <= 12);
    isValidDay = isValidDay && (day >= 1 && day <= 31);
    isValidHour = isValidHour && (hour >= 0 && hour <= 23);
    isValidMinute = isValidMinute && (minute >= 0 && minute <= 59);
    isValidSecond = isValidSecond && (second >= 0 && second <= 59);

    if(isValidYear && isValidMonth && isValidDay && isValidHour && isValidMinute && isValidSecond) {
      DateTime dt((year + 2000), month, day, hour, minute, second);
      Serial.println("Parsed DateTime: " + String(dt.year()) + "/" + String(dt.month()) + "/" + String(dt.day()) + " " + String(dt.hour()) + ":" + String(dt.minute()) + ":" + String(dt.second()));
    } 
    else {
      Serial.println("Invalid hexadecimal or invalid date/time components in string: " + timestring);
    }
  }
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

bool InverterUdp::hexStringToDec(String hexString, int& output) {
  char* endptr;
  long decNum = strtol(hexString.c_str(), &endptr, 16);
  bool isValid = (*endptr == '\0') && (hexString.length() > 0);
  output = static_cast<int>(decNum);
  return isValid;
}

String InverterUdp::decToHex(int dec) {
    String hexString;
    if (dec < 16) {
        hexString = "0" + String(dec, HEX);
    } else {
        hexString = String(dec, HEX);
    }
    hexString.toUpperCase();
    return hexString;
}
