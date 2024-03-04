
//#define BOARD_WEMOS_OLED_128x64_ESP32
//#define BOARD_HELTEC_OLED_128x32_ESP8266


///////////////////////////////////////////////////////////////////////
// HARDWARE SPECIFIC ADAPTIONS 
// DISPLAY ------------------------------------------------------------
#define SCREEN_WIDTH 128 // OLED display width, in pixels

#ifdef BOARD_HELTEC_OLED_128x32_ESP8266
  #define SCREEN_HEIGHT 32 // OLED display height, in pixels
  #define HEADER_FONT       u8g2_font_spleen6x12_me
  #define NORMAL_FONT       u8g2_font_spleen6x12_me
  #define ANNOTATION_FONT   u8g2_font_5x7_tf 
#else 
  #define SCREEN_HEIGHT 64 // OLED display height, in pixels
  #define HEADER_FONT       u8g2_font_spleen8x16_me 
  #define NORMAL_FONT       u8g2_font_spleen6x12_me 
  #define ANNOTATION_FONT   u8g2_font_spleen5x8_me 
#endif 

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)


// Only define Screen Address if display is not working by default
// The display library tries to detect the address automatically
//#define SCREEN_ADDRESS 0x3C 
// Address Examples: 0x3C, 0x3D, 0x78

///////////////////////////////////////////////////////////////////////
// Timing behavior
#define DURATION_SLEEP_IN_HOME_NETWORK 200  // Seconds


#include <Arduino.h>
// Extra library: PubSubClient by Nick O'Leary <nick.oleary@gmail.com>  V2.8.0
#include <PubSubClient.h>

#include <Wire.h>
#include <U8x8lib.h>
#include <U8g2lib.h>

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
  // Special Constructor as Clock = Pin4 and Data = Pin5 not Standard for this board
  //U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(/* clock=*/ 4, /* data=*/ 5, /* reset=*/ U8X8_PIN_NONE);  
  U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ 4, /* data=*/ 5, /* reset=*/ U8X8_PIN_NONE);   // All Boards without Reset of the Display

#elif defined(BOARD_HELTEC_OLED_128x32_ESP8266)
  // This board uses a smaller display
  //U8X8_SSD1306_128X32_UNIVISION_HW_I2C u8x8(16);
  U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE, /* clock=*/ SCL, /* data=*/ SDA);   // pin remapping with ESP8266 HW I2C

#else
  //U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8(/* reset=*/ U8X8_PIN_NONE);
  U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);   // All Boards without Reset of the Display

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
bool timeSynced = false;

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
// Status Variables

enum Status {
  STAT_OK,
  STAT_ERROR,
  STAT_NOTCONFIGURED,
  STAT_SYNCED,
  STAT_NOTSYNCED
};

Status WIFI_HOME = STAT_NOTCONFIGURED;
Status WIFI_INVERTER = STAT_NOTCONFIGURED;
Status NTP = STAT_NOTCONFIGURED;
Status MQTT = STAT_NOTCONFIGURED;


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
    u8g2.setI2CAddress(SCREEN_ADDRESS); 
  #endif 
  
  u8g2.begin();
  //u8g2.setPowerSave(0);

  // Power-On Self-Test for the Display
  u8g2.setFont(NORMAL_FONT); // Set a readable font
  u8g2.clearBuffer(); // Clear the display for the test
  u8g2.drawStr(0, 0*8+8, "Start"); // Display a test message

  // Draw a few patterns to test display
  // Since U8x8 is limited to text in its simplest form, we simulate patterns with characters
  u8g2.drawStr(0, 5*8+8, __DATE__); // Display a test message
  u8g2.drawStr(0, 6*8+8, __TIME__); // Display a test message

  u8g2.sendBuffer();

  delay(2000); // Wait two seconds to view the test pattern

  u8g2.clearBuffer(); // Clear the display after the test
  u8g2.sendBuffer();


  Serial.println("Initialize inverter");
  //inverter.printVariables();

  // Prepare MQTT client
  mqtt_client.setServer(MQTT_BROKER_HOST.c_str(),MQTT_BROKER_PORT);

  delay (1000);

  Serial.print("Init Time Client "); 


#ifdef ESP8266
      timeClient.begin();
        timeClient.setTimeOffset(7200);
#endif

  wifi_connect(WIFI_HOME_SSID, WIFI_HOME_KEY, "Home Network");
    Serial.print("Connecting to Home Network first to get the time from NTP "); 
    if (connected) {
       #ifdef ESP32
        timeClient.begin();
        timeClient.setTimeOffset(7200);
      #endif

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
      //web_getDataFromWeb(status_page_url, INVERTER_WEBACCESS_USER, INVERTER_WEBACCESS_PWD);
      
      // Starting UDP Connection inside the AP Network of inverter
      bool startCon =  inverterUdp.inverter_connect(WiFi.gatewayIP().toString(),udpServerPort, udpLocalPort, udpTimeoput_s);
      
      // Getting time from inverter, stored in inverterUDP object
      String response = inverterUdp.inverter_readtime();
      // Setting Time insode the inverter
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
                Serial.println("Time Synce complete");
                synced = true;
                lastUpdate = millis();
                break; // Exit the while loop if time was retrieved and processed successfully
            } else {
                retries++;
                Serial.println("Failed to get time, retrying...");
                delay(1000); // Delay for a second before retrying
            }
      }
    timeSynced = synced; 
    return synced;

}


////////////////////////////////////////////////////////////////////
// WiFi Functions
void wifi_connect(String ssid, String passkey, String comment){
  connected = false;
  WiFi.disconnect();
  delay(1000);
  
  // Connect to Wi-Fi
  Serial.print("DBG Before Header "); 
  setDisplayHeader("Switching WiFi");
  Serial.print("DBG Afzter  Header "); 

  u8g2.setCursor(0, 2); 
  u8g2.print("SSID: ");
  u8g2.setCursor(0, 3); 
  u8g2.println(ssid);
  u8g2.sendBuffer();
  
  
  Serial.println("----------------------------------------------------");
  Serial.print("Connecting to ");
  Serial.print(ssid);
  WiFi.begin(ssid.c_str(), passkey.c_str());

  u8g2.setCursor(0, 4); 
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) { // 10 sec timeout
    // Assuming you have set the initial cursor position before entering the loop
    // For example, u8g2.setCursor(0, 4); to start printing dots at Row 4, Column 0.
    u8g2.print(F(".")); // Print dot on the display at the current cursor position.
    
    // No need to call u8g2.display() as U8x8 updates the display immediately.

    delay(500); // Wait for 500ms
    Serial.print("."); // Also print dot on the Serial Monitor.
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    connected = true;
    setDisplayHeader("Connected");
    u8g2.setCursor(0, 2);
    u8g2.print(F("IP: "));
    u8g2.setCursor(0, 3);
    u8g2.print(WiFi.localIP().toString().c_str()); // Assuming localIP() returns an IPAddress object
    Serial.println("--> connected");
    u8g2.sendBuffer();
    delay(3000); // Display IP for 3 seconds
  } else {
    connected = false;
    setDisplayHeader("Not Connected");
    u8g2.setCursor(0, 2);
    u8g2.print(F("Inverter Offline"));
    u8g2.setCursor(0, 3);
    u8g2.print(F("Check credential"));
    u8g2.setCursor(0, 4);
    u8g2.print(F("or availability"));
    u8g2.sendBuffer();
    display_invert_blink(5,2000);
  }
}


////////////////////////////////////////////////////////////////////
// Web Parsing Section
void web_getDataFromWeb(String url, String web_user, String web_password){
  
  String serverIp = WiFi.gatewayIP().toString();
  String website = "http://" + serverIp + "/" + url;

  Serial.print("url: ");
  Serial.println(website);

  setDisplayHeader("Collect Data");

  // Display Power
  u8g2.setFont(NORMAL_FONT);
  u8g2.setCursor(0, 2); // Start label at the beginning of line 2
  u8g2.print("IP:");
  u8g2.print(serverIp.c_str());
  
  u8g2.setCursor(0, 3); // Start label at the beginning of line 2
  u8g2.print("/");
  u8g2.print(url.c_str());

  u8g2.sendBuffer();
  
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

    u8g2.setCursor(0, 5); // Start label at the beginning of line 2
    u8g2.print("> Parsing > OK");
u8g2.sendBuffer();

    delay(2000);

  } else {   
    u8g2.setCursor(0, 4);
    u8g2.print(F("> Failed to connect to server."));
    Serial.println("Failed to connect to server.");
    u8g2.setCursor(0, 5);
    u8g2.print(F("> Parsing > FAILED!"));

    display_invert_blink(5,2000);
    u8g2.sendBuffer();
  }

}


////////////////////////////////////////////////////////////////////
// DISPLAY Section

void displayInverterStatus(const Inverter& inverter) {
  int char_col_1 = 7; // Position for values
  int char_col_2 = 13; // Position for units, adjust as needed

  setDisplayHeader("Inverter Status");

  // Display Power
  u8g2.setFont(NORMAL_FONT);
  u8g2.setCursor(0, 2); // Start label at the beginning of line 2
  u8g2.print("Power");
  u8g2.setCursor(char_col_1, 2); // Set cursor for value
  u8g2.print(inverter.getInverterPowerNow_W());
  u8g2.setCursor(char_col_2, 2); // Set cursor for unit
  u8g2.print("W");

  // Display Energy Today - smaller annotation for "today"
  u8g2.setFont(NORMAL_FONT);
  u8g2.setCursor(0, 3);
  u8g2.print("E");
  u8g2.setFont(ANNOTATION_FONT); // Set smaller font for annotation
  u8g2.setCursor(1, 3); // Place "today" annotation at the beginning
  u8g2.print("today");
  u8g2.setFont(NORMAL_FONT); // Reset to normal font for value
  u8g2.setCursor(char_col_1, 3); // Adjust cursor position after "today"
  char bufferToday[5]; // Buffer for formatting float with one decimal
  sprintf(bufferToday, "%.1f", inverter.getInverterEnergyToday_kWh());
  u8g2.print(bufferToday);
  u8g2.setCursor(char_col_2, 3); // Set cursor for unit
  u8g2.print("kWh");

  // Display Total Energy - smaller annotation for "total"
  u8g2.setFont(NORMAL_FONT);
  u8g2.setCursor(0, 4);
  u8g2.print("E");
  u8g2.setFont(ANNOTATION_FONT); // Set smaller font for annotation
  u8g2.setCursor(1, 4); // Place "today" annotation at the beginning
  u8g2.print("total");
  u8g2.setFont(NORMAL_FONT); // Reset to normal font for value
  u8g2.setCursor(char_col_1, 4); // Adjust cursor position after "total"
  char bufferTotal[5]; // Buffer for formatting integer value
  sprintf(bufferTotal, "%d", (int)inverter.getInverterEnergyTotal_kWh());
  u8g2.print(bufferTotal);
  u8g2.setCursor(char_col_2, 4); // Set cursor for unit
  u8g2.print("kWh");

  u8g2.sendBuffer();
}

void displayTime() {
  int char_col_1 = 5; // Adjust as needed, based on character width and desired layout
  int char_col_2 = 12; // Additional column if needed for future use

  tmElements_t tm;
  breakTime(epochTime, tm);

  setDisplayHeader("Time Sync");

  Serial.println("Displaying Time Sync Status...");

  // Display Date
  u8g2.setCursor(0, 2); // Label at the beginning of line 2
  u8g2.print("Date");
  u8g2.setCursor(char_col_1, 2); // Set cursor for value
  char dateStr[11]; // Enough to hold DD.MM.YYYY\0
  sprintf(dateStr, "%02d.%02d.20%02d", tm.Day, tm.Month, (tm.Year + 1970) % 100);
  u8g2.print(dateStr);

  Serial.print("Date: ");
  Serial.println(dateStr);

  // Display Time
  u8g2.setCursor(0, 3); // Label at the beginning of line 3
  u8g2.print("Time");
  u8g2.setCursor(char_col_1, 3); // Set cursor for value
  char timeStr[9]; // Enough to hold HH:MM:SS\0
  sprintf(timeStr, "%02d:%02d:%02d", tm.Hour, tm.Minute, tm.Second);
  u8g2.print(timeStr);

  Serial.print("Time: ");
  Serial.println(timeStr);

  // Display Sync Interval
  u8g2.setCursor(0, 4); // Label at the beginning of line 4
  u8g2.print("Sync");
  u8g2.setCursor(char_col_1, 4); // Set cursor for value
  char syncStr[11]; // Assuming it fits, adjust size as needed
  long syncInterval = (millis() - lastUpdate) / 1000;
  sprintf(syncStr, "%ld s", syncInterval);
  u8g2.print(syncStr);

  Serial.print("Sync Interval (s): ");
  Serial.println(syncStr);

  // Print the status message
  u8g2.setCursor(0, 5);
  u8g2.print("NTP Sync: ");
  u8g2.print(timeSynced ? "Yes" : "No");

  // Report the status of time synchronization with NTP
  Serial.print("Time Synced with NTP: ");
  Serial.println(timeSynced ? "Yes" : "No");

  u8g2.sendBuffer();
}


void setDisplayHeader(String HeaderText) {
  u8g2.clearBuffer();
  u8g2.setFont(HEADER_FONT); // Assuming this font has a fixed width of 8 pixels per character

  int charWidth = 8; // Character width in pixels
  int screenWidthChars = SCREEN_WIDTH / charWidth; // Calculate max number of characters per line
  int textLength = HeaderText.length();

  // Calculate padding to center the text
  int padding = (screenWidthChars - textLength) / 2;
  if (padding < 0) padding = 0; // Ensure padding is not negative

  // Construct the padded header text
  String paddedHeaderText = "";
  for (int i = 0; i < padding; i++) {
    paddedHeaderText += " "; // Add leading spaces
  }
  paddedHeaderText += HeaderText;
  for (int i = 0; i <= padding; i++) { // potentially one additional last character for odd numbers. Extensions will be trimmed.
    paddedHeaderText += " "; // Add trailing spaces
  }

  // Trim the paddedHeaderText if it exceeds the display width
  if (paddedHeaderText.length() > screenWidthChars) {
    paddedHeaderText = paddedHeaderText.substring(0, screenWidthChars);
  }

//  u8g2.setInverseFont(1); // Set inverse font for the header
  u8g2.printf(0, 0, paddedHeaderText.c_str()); // Always start from the first column
  
//  u8g2.setInverseFont(0); // Reset inverse font after the header
  u8g2.setFont(NORMAL_FONT); // Set back to normal font for subsequent text
  u8g2.sendBuffer();
}


void display_invert_blink(int times, int delay_ms){
    for (int i = 0; i < times; i++){
        // Turn off the display
        u8g2.setPowerSave(1);
        delay(100);
        
        // Turn on the display
        u8g2.setPowerSave(0);
        delay(delay_ms - 100);
    }
}


// ToDo: TLS Based MQTT connection

////////////////////////////////////////////////////////////////////
// MQTT SECTION
void mqtt_submit_data(){
    
    // Connect to Wi-Fi
    
    setDisplayHeader("Publish to MQTT");
    u8g2.setCursor(0, 2);
    u8g2.print("Server: ");
    // Since U8x8 does not support `println`, we manually move to the next line after printing the server info
    u8g2.setCursor(0, 3); // Adjust this line number based on your actual display content and layout
    u8g2.print(MQTT_BROKER_HOST.c_str());
    u8g2.setCursor(0, 4);
    u8g2.print("Port: ");
    // Assuming MQTT_BROKER_PORT is an integer, you might need to convert it to a string
    char portStr[6]; // Large enough for typical port numbers (up to 65535) plus a null terminator
    sprintf(portStr, "%d", MQTT_BROKER_PORT);
    u8g2.print(portStr);
    u8g2.sendBuffer();

    u8g2.setCursor(0, 5);
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
      u8g2.setCursor(0, 6);
      u8g2.print(" > published");
      u8g2.sendBuffer();
      delay(2000);
    }else{
      u8g2.setCursor(0, 6);
      u8g2.print(" > FAIL !");
      u8g2.sendBuffer();
      display_invert_blink(5,2000);
    }
     
}

void mqtt_reconnect() {
  int attempts = 0;
  while (!mqtt_client.connected() && attempts < 5) {
    Serial.print("Attempting MQTT connection...");
    if (mqtt_client.connect("deye-esp-solar-bridge", MQTT_BROKER_USER.c_str(), MQTT_BROKER_PWD.c_str())) {
      Serial.println("connected");  
      u8g2.print("connected");
      u8g2.sendBuffer();
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqtt_client.state());
      Serial.println("try again in 2 seconds");
      u8g2.print(".");
      u8g2.sendBuffer();
      delay(2000);
    }
    attempts++;
  } 
}
