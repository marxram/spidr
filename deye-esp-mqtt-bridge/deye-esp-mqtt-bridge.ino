
//#define BOARD_WEMOS_OLED_128x64_ESP32


///////////////////////////////////////////////////////////////////////
// HARDWARE SPECIFIC ADAPTIONS 
// DISPLAY ------------------------------------------------------------
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)

//#define HEADER_FONT  u8x8_font_chroma48medium8_r
//#define NORMAL_FONT  u8x8_font_chroma48medium8_r


// Only define Screen Address if display is not working by default, 
//#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

// Address Examples: 0x3C, 0x3D, 0x78
// 

///////////////////////////////////////////////////////////////////////
// Timing behavior
#define DURATION_SLEEP_IN_HOME_NETWORK 200  // Seconds




#include <Arduino.h>
// Extra library: PubSubClient by Nick O'Leary <nick.oleary@gmail.com>  V2.8.0
#include <PubSubClient.h>


#include <Wire.h>
#include <U8x8lib.h>


// Web and WiFi
#include <WiFiClient.h>
#include <base64.h>


#ifdef ESP32
#include <WiFi.h> // ESP32 specific WiFi library
// ESP32 specific setup and functions
#endif

#ifdef ESP8266
#include <ESP8266WiFi.h> // ESP8266 specific WiFi library
// ESP8266 specific setup and functions
#endif

#ifdef BOARD_WEMOS_OLED_128x64_ESP32
U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(/* clock=*/ 4, /* data=*/ 5, /* reset=*/ U8X8_PIN_NONE);  
#else 
U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8(/* reset=*/ U8X8_PIN_NONE);
#endif 


// Inverter Data handling 
#include "Inverter.h"

// Include Udp Channel inverter communication
#include "InverterUdp.h"

///////////////////////////////////////////////////////////////////////
// Configuration and user settings
#include "arduino_secrets.h"

// TimeSynchronization and handling
#include <NTPClient.h>
#include <TimeLib.h>


///////////////////////////////////////////////////////////////////////
// User Secrets imported
String WIFI_HOME_SSID = SECRET_WIFI_HOME_SSID;
String WIFI_HOME_KEY = SECRET_WIFI_HOME_KEY;

String WIFI_INVERTER_SSID = SECRET_WIFI_INVERTER_SSID;
String WIFI_INVERTER_KEY = SECRET_WIFI_INVERTER_KEY;

String WIFI_RELAIS_SSID = SECRET_WIFI_RELAIS_SSID;
String WIFI_RELAIS_KEY = SECRET_WIFI_RELAIS_KEY;

String MQTT_BROKER_HOST = SECRET_MQTT_BROKER_HOST;
int    MQTT_BROKER_PORT = SECRET_MQTT_BROKER_PORT;
String MQTT_BROKER_USER = SECRET_MQTT_BROKER_USER;
String MQTT_BROKER_PWD = SECRET_MQTT_BROKER_PWD;
String MQTT_BROKER_MAINTOPIC = SECRET_MQTT_BROKER_MAINTOPIC;

String INVERTER_WEBACCESS_USER = SECRET_INVERTER_WEBACCESS_USER;
String INVERTER_WEBACCESS_PWD = SECRET_INVERTER_WEBACCESS_PWD;

///////////////////////////////////////////////////////////////////////
// Other rather static parameters
String status_page_url = "status.html" ;

unsigned long startTime =  0;


///////////////////////////////////////////////////////////////////////
// Global variables
bool connected = false;


////////////////////////////////////////////////////////////////////
// Intializations 

// Initialize the MQTT client
WiFiClient espClient;
PubSubClient mqtt_client(espClient);

Inverter inverter;
InverterUdp inverterUdp;

const int udpServerPort = 50000; // manual port
const int udpLocalPort = 48899; // Fixed port of deye inverter
const int udpTimeoput_s = 10; // 10 Seconds Timeout 
    
// Time Synchronization
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");
unsigned long updateInterval = 20000; // 1 minute
long lastUpdate;
const int maxNtpRetries = 5;
unsigned long epochTime = 0;
bool ntpTimeAvailable = false;


////////////////////////////////////////////////////////////////////
// Function declarations
void mqtt_submit_data();
void mqtt_reconnect();
void wifi_connect(String ssid, String passkey, String comment);
void web_getDataFromWeb(String url, String web_user, String web_password);
void displayInverterStatus(const Inverter& inverter);
void setDisplayHeader(String HeaderText);

////////////////////////////////////////////////////////////////////
// SETUP Function

void setup() {
  Serial.begin(115200);
  delay(10);

  #ifdef SCREEN_ADDRESS
    u8x8.setI2CAddress(SCREEN_ADDRESS); 
  #endif 
  
  u8x8.begin();
  u8x8.setPowerSave(0);

  // Power-On Self-Test for the Display
  u8x8.setFont(u8x8_font_chroma48medium8_r); // Set a readable font
  u8x8.clearDisplay(); // Clear the display for the test
  u8x8.drawString(0, 0, "Display POST"); // Display a test message

  // Draw a few patterns to test display
  // Since U8x8 is limited to text in its simplest form, we simulate patterns with characters
  u8x8.draw2x2String(0, 2, "AB");
  u8x8.draw2x2String(4, 2, "CD");
  
  delay(2000); // Wait two seconds to view the test pattern

  u8x8.clearDisplay(); // Clear the display after the test


  Serial.println("Initialize inverter");
  //inverter.printVariables();

  // Prepare MQTT client
  mqtt_client.setServer(MQTT_BROKER_HOST.c_str(),MQTT_BROKER_PORT);

  delay (1000);

  Serial.print("Init Time Client "); 
  timeClient.begin();
  timeClient.setTimeOffset(7200);

  wifi_connect(WIFI_HOME_SSID, WIFI_HOME_KEY, "Home Network");
    Serial.print("Connecting to Home Network first to get the time from NTP "); 
    if (connected) {
        delay(2000);
        ntpTimeAvailable = syncTime();
        displayTime();
        delay(5000);
    }

}


////////////////////////////////////////////////////////////////////
// MAIN LOOP

void loop() {   
    // INVERTER NETWORK 
    wifi_connect(WIFI_INVERTER_SSID, WIFI_INVERTER_KEY, "Inverter Network");
    // If connected with the Solar Inverter
    if (connected) {
      // ToDo-Option: Replace Web scraping by Modbus Read commands
      web_getDataFromWeb(status_page_url, INVERTER_WEBACCESS_USER, INVERTER_WEBACCESS_PWD);
      
      // Starting UDP Connection inside the AP Network of inverter
      bool startCon =  inverterUdp.inverter_connect(WiFi.gatewayIP().toString(),udpServerPort, udpLocalPort, udpTimeoput_s);
      
      // Getting time from inverter, stored in inverterUDP object
      String response = inverterUdp.inverter_readtime();
      
      if (ntpTimeAvailable){
        Serial.println("Triggering time update");
        inverterUdp.inverter_settime(epochTime);
      } else {
        Serial.println("No NTP time available. Skipping sync");
      }

      // Close connection before leaving to Home Network
      inverterUdp.inverter_close();
    }

    // Output Information 
    Serial.println("Print WebInverter");
    inverter.printVariables();
    displayInverterStatus(inverter);

    // Switching to Home Network
    wifi_connect(WIFI_HOME_SSID, WIFI_HOME_KEY, "Home Network");
    if (connected) {
        mqtt_submit_data();
        delay(3000); // Show data for 3 Seconds
        ntpTimeAvailable = syncTime();
        displayTime();
        int secondsInHomeNetwork =0;
        while (secondsInHomeNetwork < DURATION_SLEEP_IN_HOME_NETWORK ){
          // Toggle between Time and Solar Production Screen
          displayInverterStatus(inverter);
          delay(5000);
          secondsInHomeNetwork+=5;
          
          displayTime();
          delay(5000);
          secondsInHomeNetwork+=5;
        }
    }

}


////////////////////////////////////////////////////////////////////
// Sync Time
bool syncTime (){
    bool synced = false;
    Serial.println("Syncing time...");
        int retries = 0;
        while (retries < maxNtpRetries) {
            timeClient.update();
            epochTime = timeClient.getEpochTime();
            if (epochTime > 0) {  // If time is valid
                synced = true;
                lastUpdate = millis();
                break; // Exit the while loop if time was retrieved and processed successfully
            } else {
                retries++;
                Serial.println("Failed to get time, retrying...");
                delay(1000); // Delay for a second before retrying
            }
      }
    return synced;

}


////////////////////////////////////////////////////////////////////
// WiFi Functions
void wifi_connect(String ssid, String passkey, String comment){
  connected = false;
  WiFi.disconnect();
  delay(1000);
  
  // Connect to Wi-Fi
  setDisplayHeader("Switching WiFi");

  u8x8.setCursor(0, 2); 
  u8x8.print("SSID: ");
  u8x8.setCursor(0, 3); 
  u8x8.println(ssid);
  
  
  Serial.println("----------------------------------------------------");
  Serial.print("Connecting to ");
  Serial.print(ssid);
  WiFi.begin(ssid.c_str(), passkey.c_str());

  u8x8.setCursor(0, 4); 
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) { // 10 sec timeout
    // Assuming you have set the initial cursor position before entering the loop
    // For example, u8x8.setCursor(0, 4); to start printing dots at Row 4, Column 0.
    u8x8.print(F(".")); // Print dot on the display at the current cursor position.
    
    // No need to call u8x8.display() as U8x8 updates the display immediately.

    delay(500); // Wait for 500ms
    Serial.print("."); // Also print dot on the Serial Monitor.
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    connected = true;
    setDisplayHeader("Connected");
    u8x8.setCursor(0, 2);
    u8x8.print(F("IP: "));
    u8x8.setCursor(0, 3);
    u8x8.print(WiFi.localIP().toString().c_str()); // Assuming localIP() returns an IPAddress object
    Serial.println("--> connected");
    delay(3000); // Display IP for 3 seconds
  } else {
    connected = false;
    setDisplayHeader("Not Connedted");
    u8x8.setCursor(0, 0);
    u8x8.print(F("Failed to connect"));
    u8x8.setCursor(0, 1);
    u8x8.print(F("Check credential"));
    u8x8.setCursor(0, 2);
    u8x8.print(F("or availability"));
    
    // Blinking effect: manually toggling display power
    for (int i = 0; i < 5; i++) {
      u8x8.setPowerSave(1); // Turn off display
      delay(1000); // Wait for 1 second
      u8x8.setPowerSave(0); // Turn on display
      delay(1000); // Display message for 1 second
    }
  }
}


////////////////////////////////////////////////////////////////////
// Web Parsing Section
void web_getDataFromWeb(String url, String web_user, String web_password){
  
  String serverIp = WiFi.gatewayIP().toString();
  String website = "http://" + serverIp + "/" + url;

  Serial.print("url: ");
  Serial.println(website);


  // Clear the display
  u8x8.clearDisplay();

  // Set the desired font (if not already set elsewhere in your code)
  u8x8.setFont(u8x8_font_chroma48medium8_r); // Example font, choose one that fits your needs

  // Set cursor position and print the collecting data message
  u8x8.setCursor(0, 0); // Column 0, Row 0
  u8x8.print(F("++ Collecting Data ++"));

  // Since U8x8 doesn't automatically advance to the next line after printing like println does,
  // you need to manually move the cursor for each new line.
  u8x8.setCursor(0, 1); // Move to the next line
  u8x8.print(serverIp);

  // Concatenating the "/" with url requires a bit more work since U8x8's print method
  // does not support String objects directly in the same way as the Adafruit library.
  // You'd typically handle concatenation before printing, or print in parts if dynamic.
  u8x8.setCursor(0, 2); // Move to the next line
  u8x8.print("/"); // Print the slash first,
  u8x8.print(url.c_str()); // then print the URL. Assuming `url` is a String object; use c_str() to convert.


  // Create an instance of WiFiClient
  WiFiClient client;

  // Connect to the server
  if (client.connect(serverIp.c_str(), 80)) {
    // Prepare the HTTP request headers including the Authorization header
    
    Serial.println("Client connected: trying Basic Auth");
    String authHeaderValue = "Basic " + base64::encode(web_user + ":" + web_password);

    client.println("GET /" + url + " HTTP/1.1");
    client.println("Host: " + serverIp);
    client.println("Authorization: " + authHeaderValue);
    client.println("Connection: close");
    client.println();

    // Wait for the server's response
    String response = "";
    String line = "";
    while (client.connected()) {
      while (client.available()) {
        char c = client.read();
            if (c == '\n') {
                // Check if line starts with "var " and append it to the response string
                if (line.startsWith("var ")) {
                    response += line + "\n";
                }
                // Clear the line string for the next line
                line = "";
            } else if (c != '\r') {
                line += c;
            }
      }
    }

    // Print the entire response
    //Serial.print(response);
    inverter.updateData(response);

    client.stop();

    Serial.printf("\nPower now: %f\n", inverter.getInverterPowerNow_W());
    Serial.printf("Energy today: %f\n", inverter.getInverterEnergyToday_kWh());
    Serial.printf("Energy total: %f\n", inverter.getInverterEnergyTotal_kWh());

    u8x8.setCursor(0, 3);
    u8x8.print(F("> Parsing > OK"));
    delay(2000);

  } else {
    u8x8.setCursor(0, 4);
    u8x8.print(F("> Failed to connect to server."));
    Serial.println("Failed to connect to server.");
    u8x8.setCursor(0, 5);
    u8x8.print(F("> Parsing > FAILED!"));

    display_invert_blink(5,2000);
  }

}


////////////////////////////////////////////////////////////////////
// DISPLAY Section

void displayInverterStatus(const Inverter& inverter) {
  int char_col_1 = 7; // Position for values
  int char_col_2 = 13; // Position for units, adjust as needed
  
  setDisplayHeader("Inverter Status");

  // Display Power
  u8x8.setCursor(0, 2); // Start label at the beginning of line 2
  u8x8.print("Power");
  u8x8.setCursor(char_col_1, 2); // Set cursor for value
  u8x8.print(inverter.getInverterPowerNow_W());
  u8x8.setCursor(char_col_2, 2); // Set cursor for unit
  u8x8.print("W");

  // Display Energy Today
  u8x8.setCursor(0, 3); // Start label at the beginning of line 3
  u8x8.print("E today");
  u8x8.setCursor(char_col_1, 3); // Set cursor for value
  char bufferToday[10]; // Buffer for formatting float with one decimal
  sprintf(bufferToday, "%.1f", inverter.getInverterEnergyToday_kWh());
  u8x8.print(bufferToday);
  u8x8.setCursor(char_col_2, 3); // Set cursor for unit
  u8x8.print("kWh");

  // Display Total Energy
  u8x8.setCursor(0, 4); // Start label at the beginning of line 4
  u8x8.print("E total");
  u8x8.setCursor(char_col_1, 4); // Set cursor for value
  char bufferTotal[10]; // Buffer for formatting integer value
  sprintf(bufferTotal, "%d", (int)inverter.getInverterEnergyTotal_kWh());
  u8x8.print(bufferTotal);
  u8x8.setCursor(char_col_2, 4); // Set cursor for unit
  u8x8.print("kWh");
}


void displayTime() {
  int char_col_1 = 7; // Adjust as needed, based on character width and desired layout
  int char_col_2 = 12; // Additional column if needed for future use

  tmElements_t tm;
  breakTime(epochTime, tm);

  setDisplayHeader("Time Sync");

  // Display Date
  u8x8.setCursor(0, 2); // Label at the beginning of line 2
  u8x8.print("Date");
  u8x8.setCursor(char_col_1, 2); // Set cursor for value
  char dateStr[11]; // Enough to hold DD.MM.YYYY\0
  sprintf(dateStr, "%02d.%02d.20%02d", tm.Day, tm.Month, (tm.Year + 1970) % 100);
  u8x8.print(dateStr);

  // Display Time
  u8x8.setCursor(0, 3); // Label at the beginning of line 3
  u8x8.print("Time");
  u8x8.setCursor(char_col_1, 3); // Set cursor for value
  char timeStr[9]; // Enough to hold HH:MM:SS\0
  sprintf(timeStr, "%02d:%02d:%02d", tm.Hour, tm.Minute, tm.Second);
  u8x8.print(timeStr);

  // Display Sync Interval
  u8x8.setCursor(0, 4); // Label at the beginning of line 4
  u8x8.print("Sync");
  u8x8.setCursor(char_col_1, 4); // Set cursor for value
  char syncStr[11]; // Assuming it fits, adjust size as needed
  long syncInterval = lastUpdate - millis();
  sprintf(syncStr, "%ld", syncInterval);
  u8x8.print(syncStr);
}

void setDisplayHeader(String HeaderText) {
  u8x8.clearDisplay();
  u8x8.setFont(u8x8_font_chroma48medium8_r); // Set the font. Adjust if you have a preferred font.
  
  // Assuming each character is 8 pixels wide, adjust if your font is different
  int charWidth = 8;
  int screenWidthChars = SCREEN_WIDTH / charWidth; // Calculate the max number of characters per line
  int textLength = HeaderText.length();
  
  // Calculate starting column to center the text
  int startColumn = (screenWidthChars - textLength) / 2;
  if (startColumn < 0) startColumn = 0; // Ensure startColumn is not negative
  
  u8x8.setInverseFont(1); // Set inverse font for the header
  // Ensure the text fits within the display bounds
  if (textLength > screenWidthChars) {
    HeaderText = HeaderText.substring(0, screenWidthChars);
  }
  u8x8.drawString(startColumn, 0, HeaderText.c_str()); // Row 0, dynamically calculated start column
  
  u8x8.setInverseFont(0); // Reset inverse font after the header
  u8x8.setCursor(0, 2); // Move cursor to Row 2, Column 0 for the next text.
  u8x8.setFont(u8x8_font_chroma48medium8_r);
}


void display_invert_blink(int times, int delay_ms){
    for (int i = 0; i < times; i++){
        // Turn off the display
        u8x8.setPowerSave(1);
        delay(delay_ms / 2);
        
        // Turn on the display
        u8x8.setPowerSave(0);
        delay(delay_ms / 2);
    }
}


// ToDo: TLS Based MQTT connection

////////////////////////////////////////////////////////////////////
// MQTT SECTION
void mqtt_submit_data(){
    
    // Connect to Wi-Fi
    // Clear the display for fresh output
    u8x8.clearDisplay();

    // Set the initial cursor position to the top left corner of the display
    u8x8.setCursor(0, 0);
    // Print the header message
    u8x8.print(F("++ Publish to MQTT ++"));

    // Move to the next line to print the server details
    u8x8.setCursor(0, 1);
    u8x8.print("Server: ");

    // Since U8x8 does not support `println`, we manually move to the next line after printing the server info
    u8x8.setCursor(0, 2); // Adjust this line number based on your actual display content and layout
    u8x8.print(MQTT_BROKER_HOST.c_str());
    u8x8.print(":");
    // Assuming MQTT_BROKER_PORT is an integer, you might need to convert it to a string
    char portStr[6]; // Large enough for typical port numbers (up to 65535) plus a null terminator
    sprintf(portStr, "%d", MQTT_BROKER_PORT);
    u8x8.print(portStr);
    
    if (!mqtt_client.connected()) {
        mqtt_reconnect();
    }

     if (mqtt_client.connected()) {
        Serial.println("Connected to MQTT server");
        mqtt_client.loop();

        // Publish data to a topic
        String data = "";
        String topic = "";


      // Publish getWebdataSn
      String snTopic = MQTT_BROKER_MAINTOPIC + "/inverter/serial";
      mqtt_client.publish(snTopic.c_str(), inverter.getInverterSerial().c_str());

    /* Removing unpopulated 
      // Publish getWebdataMsvn
      String msvnTopic = MQTT_BROKER_MAINTOPIC + "/inverter/msvn";
      mqtt_client.publish(msvnTopic.c_str(), inverter.getWebdataMsvn().c_str());

      // Publish getWebdataSsvn
      String ssvnTopic = MQTT_BROKER_MAINTOPIC + "/inverter/ssvn";
      mqtt_client.publish(ssvnTopic.c_str(), inverter.getWebdataSsvn().c_str());

      // Publish getWebdataPvType
      String pvTypeTopic = MQTT_BROKER_MAINTOPIC + "/inverter/pvType";
      mqtt_client.publish(pvTypeTopic.c_str(), inverter.getWebdataPvType().c_str());

      // Publish getWebdataRateP
      String ratePTopic = MQTT_BROKER_MAINTOPIC + "/inverter/rateP";
      mqtt_client.publish(ratePTopic.c_str(), inverter.getWebdataRateP().c_str());
    */

      // Publish getInverterPowerNow_W (float)
      float nowPTopic = inverter.getInverterPowerNow_W();
      String nowPTopicName = MQTT_BROKER_MAINTOPIC + "/inverter/power_W";
      mqtt_client.publish(nowPTopicName.c_str(), String(nowPTopic).c_str());

      // Publish getInverterEnergyToday_kWh (float)
      float todayETopic = inverter.getInverterEnergyToday_kWh();
      String todayETopicName = MQTT_BROKER_MAINTOPIC + "/inverter/energy_today_kWh";
      mqtt_client.publish(todayETopicName.c_str(), String(todayETopic).c_str());

      // Publish getInverterEnergyTotal_kWh (float)
      float totalETopic = inverter.getInverterEnergyTotal_kWh();
      String totalETopicName = MQTT_BROKER_MAINTOPIC + "/inverter/energy_total_kWh";
      mqtt_client.publish(totalETopicName.c_str(), String(totalETopic).c_str());

      // Publish getWebdataAlarm
      String alarmTopic = MQTT_BROKER_MAINTOPIC + "/status/alarm";
      mqtt_client.publish(alarmTopic.c_str(), inverter.getWebdataAlarm().c_str());

      // Publish getWebdataUtime
      String utimeTopic = MQTT_BROKER_MAINTOPIC + "/status/utime";
      mqtt_client.publish(utimeTopic.c_str(), inverter.getWebdataUtime().c_str());

      // Publish getLoggerModuleID
      String coverMidTopic = MQTT_BROKER_MAINTOPIC + "/logger/serial";
      mqtt_client.publish(coverMidTopic.c_str(), inverter.getLoggerModuleID().c_str());

      // Publish getLoggerSoftwareVersion
      String coverVerTopic = MQTT_BROKER_MAINTOPIC + "/logger/version";
      mqtt_client.publish(coverVerTopic.c_str(), inverter.getLoggerSoftwareVersion().c_str());

      // Publish getLoggerWifiMode
      String coverWmodeTopic = MQTT_BROKER_MAINTOPIC + "/logger/wifi_mode";
      mqtt_client.publish(coverWmodeTopic.c_str(), inverter.getLoggerWifiMode().c_str());

      // Publish getLoggerApSsid
      String coverApSsidTopic = MQTT_BROKER_MAINTOPIC + "/logger/ApSsid";
      mqtt_client.publish(coverApSsidTopic.c_str(), inverter.getLoggerApSsid().c_str());

      // Publish getLoggerApIp
      String coverApIpTopic = MQTT_BROKER_MAINTOPIC + "/logger/ApIp";
      mqtt_client.publish(coverApIpTopic.c_str(), inverter.getLoggerApIp().c_str());

      // Publish getLoggerApMac
      String coverApMacTopic = MQTT_BROKER_MAINTOPIC + "/logger/ApMac";
      mqtt_client.publish(coverApMacTopic.c_str(), inverter.getLoggerApMac().c_str());

      // Publish getLoggerStaSsid
      String coverStaSsidTopic = MQTT_BROKER_MAINTOPIC + "/logger/StaSsid";
      mqtt_client.publish(coverStaSsidTopic.c_str(), inverter.getLoggerStaSsid().c_str());

      // Publish getLoggerStaRssi
      String coverStaRssiTopic = MQTT_BROKER_MAINTOPIC + "/logger/StaRssi";
      mqtt_client.publish(coverStaRssiTopic.c_str(), inverter.getLoggerStaRssi().c_str());

      // Publish getLoggerStaIp
      String coverStaIpTopic = MQTT_BROKER_MAINTOPIC + "/logger/StaIp";
      mqtt_client.publish(coverStaIpTopic.c_str(), inverter.getLoggerStaIp().c_str());

      // Publish getLoggerStaMac
      String coverStaMacTopic = MQTT_BROKER_MAINTOPIC + "/logger/StaMac";
      mqtt_client.publish(coverStaMacTopic.c_str(), inverter.getLoggerStaMac().c_str());

      // Publish getRemoteServerStatusA
      String statusATopic = MQTT_BROKER_MAINTOPIC + "/remote-server/statusA";
      mqtt_client.publish(statusATopic.c_str(), inverter.getRemoteServerStatusA().c_str());

      // Publish getRemoteServerStatusB
      String statusBTopic = MQTT_BROKER_MAINTOPIC + "/remote-server/statusB";
      mqtt_client.publish(statusBTopic.c_str(), inverter.getRemoteServerStatusB().c_str());

      // Publish getRemoteServerStatusC
      String statusCTopic = MQTT_BROKER_MAINTOPIC + "/remote-server/statusC";
      mqtt_client.publish(statusCTopic.c_str(), inverter.getRemoteServerStatusC().c_str());

      // Publish getLastUpdateTimestamp (unsigned long)
      unsigned long lastUpdateTimestamp = inverter.getLastUpdateTimestamp();
      String lastUpdateTopic = MQTT_BROKER_MAINTOPIC + "/lastUpdateTimestamp";
      mqtt_client.publish(lastUpdateTopic.c_str(), String(lastUpdateTimestamp).c_str());


      Serial.println("> MQTT data published"); 
      Serial.println("> MQTT CLOSED");

      mqtt_client.disconnect();
      u8x8.setCursor(0, 3);
      u8x8.print(" > published");
     
    }else{
      u8x8.setCursor(0, 3);
      u8x8.print(" > FAIL !");
      display_invert_blink(5,2000);
    }
     
}

void mqtt_reconnect() {
  int attempts = 0;
  while (!mqtt_client.connected() && attempts < 5) {
    Serial.print("Attempting MQTT connection...");
    
    u8x8.setCursor(0, 4);
    if (mqtt_client.connect("deye-esp-solar-bridge", MQTT_BROKER_USER.c_str(), MQTT_BROKER_PWD.c_str())) {
      Serial.println("connected");  
      u8x8.print(" connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqtt_client.state());
      Serial.println(" try again in 2 seconds");
      u8x8.print(".");
      delay(2000);
    }
    attempts++;
  } 
}
