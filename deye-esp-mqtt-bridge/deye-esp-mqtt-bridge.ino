
//#define BOARD_WEMOS_OLED_128x64_ESP32
//#define BOARD_HELTEC_OLED_128x32_ESP8266


///////////////////////////////////////////////////////////////////////
// HARDWARE SPECIFIC ADAPTIONS 
// DISPLAY ------------------------------------------------------------
//#define SCREEN_WIDTH 128 // OLED display width, in pixels


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
#define DURATION_STAY_IN_HOME_NETWORK 200  // Seconds


#include <Arduino.h>
// Extra library: PubSubClient by Nick O'Leary <nick.oleary@gmail.com>  V2.8.0
#include <PubSubClient.h>

#include "DisplayManager.h"

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


//#define USE_NTP_SYNC 



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

DisplayManager displayManager;
ActionData action; // Action Structure to Display


// Initialize the MQTT client
WiFiClient espClient;
PubSubClient mqtt_client(espClient);

Inverter inverter;
InverterUdp inverterUdp;

const int udpServerPort = 50000; // manual port
const int udpLocalPort = 48899; // Fixed port of deye inverter
const int udpTimeoput_s = 10; // 10 Seconds Timeout 
    
// Time Synchronization
#ifdef USE_NTP_SYNC
  WiFiUDP ntpUDP;
  NTPClient timeClient(ntpUDP, "pool.ntp.org");
  unsigned long updateInterval = 20000; // 1 minute
  long lastUpdate;
  const int maxNtpRetries = 5;
  unsigned long epochTime = 0;
  bool ntpTimeAvailable = false;
#endif

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
void submitDataViaMQTT();
void mqtt_reconnect();
void wifi_connect(String ssid, String passkey, String comment);
void readInverterDataFromWebInterface(String url, String web_user, String web_password);
void displayInverterStatus(const Inverter& inverter, unsigned int duration_ms);

void setDisplayHeader(String HeaderText);

////////////////////////////////////////////////////////////////////
// SETUP Function

void setup() {
  Serial.begin(115200);
  delay(10);

  #ifdef SCREEN_ADDRESS
    displayManager.setI2CAddress(SCREEN_ADDRESS); 
  #endif 

  displayManager.init();

  // Show also more output and Parameters like Url, IP etc. 
  displayManager.verboseDisplay = true;

  // Prepare MQTT client
  mqtt_client.setServer(MQTT_BROKER_HOST.c_str(),MQTT_BROKER_PORT);

  delay (1000);

  #ifdef USE_NTP_SYNC
  Serial.print("Init Time Client "); 

  // ESP32 and 8266 seem to behave different. ESP8266 can be initialized without a working WiFi connection, 
  // ESP would fail to initialize here. --> Moved to the if(connect) section
  // #ifdef ESP8266
  //       timeClient.begin();
  //       timeClient.setTimeOffset(7200);
  // #endif

  wifi_connect(WIFI_HOME_SSID, WIFI_HOME_KEY, "Home Network");
    Serial.print("Connecting to Home Network first to get the time from NTP "); 
    if (connected) {
       //#ifdef ESP32
        //timeClient.begin();
        timeClient.setTimeOffset(7200);
     // #endif

        delay(2000);
        ntpTimeAvailable = syncTime();
        displayTime();
        delay(5000);
    }
    #endif
}


////////////////////////////////////////////////////////////////////
// MAIN LOOP
void loop() {   
    // INVERTER NETWORK 
    wifi_connect(WIFI_INVERTER_SSID, WIFI_INVERTER_KEY, "Inverter WiFi");
    // If connected with the Solar Inverter
    if (connected) {  
      // Starting UDP Connection inside the AP Network of inverter
      bool startCon =  inverterUdp.inverter_connect(WiFi.gatewayIP().toString(),udpServerPort, udpLocalPort, udpTimeoput_s);
      
      // Getting time from inverter, stored in inverterUDP object
      String response = inverterUdp.inverter_readtime();

      // ToDo: Only if time differs, do a sync

      // Setting Time inside the inverter via UDP
      // If this is the correct time does not really matter. The most important thing is 
      // that the inverter resets the daily energy production counter, if the time was changed
      // from the default value 
      #ifdef USE_NTP_SYNC
        if (ntpTimeAvailable){
          Serial.println("Triggering time update");
          inverterUdp.inverter_settime(epochTime);
        } else {
          Serial.println("No NTP time available. Skipping sync");
        }
      #else
        // create a mocking time and use it to initialize the inverter
        inverterUdp.inverter_settime(1630000000);
      #endif


      // ToDo: Read other parameters from the UDP interface

      // Close connection before leaving to Home Network
      inverterUdp.inverter_close();

      // Retrieving the data from the inverter via the web interface
      readInverterDataFromWebInterface(status_page_url, INVERTER_WEBACCESS_USER, INVERTER_WEBACCESS_PWD);
    }

    // Output Information 
    Serial.println("Print Inverter Data");
    inverter.printVariables();
  
    // Show most impotrtant values on the display
    displayInverterStatus(inverter, 6000);

    // Switching to Home Network
    wifi_connect(WIFI_HOME_SSID, WIFI_HOME_KEY, "Home WiFi");
    if (connected) {
        submitDataViaMQTT();
        delay(3000); // Show data for 3 Seconds
        
        #ifdef USE_NTP_SYNC
          ntpTimeAvailable = syncTime();
          displayTime();
        #endif

        int secondsInHomeNetwork =0;
        while (secondsInHomeNetwork < DURATION_STAY_IN_HOME_NETWORK ){
          // Toggle between Time and Solar Production Screen
          displayInverterStatus(inverter, 10000);
          secondsInHomeNetwork+=10;
          
          #ifdef USE_NTP_SYNC
            displayTime();
            delay(5000);
            secondsInHomeNetwork+=5;
          #endif

          
        }
    }

}


////////////////////////////////////////////////////////////////////
// Sync Time
#ifdef USE_NTP_SYNC
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
#endif

////////////////////////////////////////////////////////////////////
// WiFi Functions
void wifi_connect(String ssid, String passkey, String comment){
  Serial.println("----------------------------------------------------");
  Serial.print("Connecting to ");
  Serial.print(ssid);


  // Display Initialization
  action.name = comment;
  action.details   = "Connect to WiFi";
  String network = "SSID: " + ssid;
  action.params[0] = network.c_str();
  action.params[1] = "IP:  Waiting...";
  //action.params[2] = "GW-IP: Waiting...";
  action.result = "In Progress";
  action.resultDetails = "";
  displayManager.displayAction(action);
  
  connected = false;
  WiFi.disconnect();
  delay(1000);
  
  Serial.println("----------------------------------------------------");
  Serial.print("Connecting to ");
  Serial.print(ssid);
  
  // Connect
  WiFi.begin(ssid.c_str(), passkey.c_str());

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) { // 10 sec timeout
    delay(500); // Wait for 500ms
    Serial.print("."); // Also print dot on the Serial Monitor.
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    connected = true;
    Serial.println("--> connected");
    delay(3000); // Display IP for 3 seconds
  
    // Display Result
    String ip_string = WiFi.localIP().toString().c_str();

    String ip = "IP:  " + ip_string;
    action.params[1] = ip.c_str();
    //action.params[2] = "GW-IP: Waiting...";
    action.result = "Connected";
    action.resultDetails = "";
    displayManager.displayAction(action);
  
  } else {
    connected = false;
    
    // Display Result
    action.params[1] = "Check Credentials";
    //action.params[2] = "GW-IP: Waiting...";
    action.result = "FAILED";
    action.resultDetails = "";
    displayManager.displayAction(action);
    delay(5000);
  }
}


////////////////////////////////////////////////////////////////////
// Web Parsing Section
void readInverterDataFromWebInterface(String url, String web_user, String web_password){
  String serverIp = WiFi.gatewayIP().toString();
  String website = "http://" + serverIp + "/" + url;

  Serial.print("url: ");
  Serial.println(website);
  
  // Display Initialization
  action.name = "Collect Data";
  action.details = "Read Inverter";
  action.params[0] = "http://" + serverIp;
  action.params[1] = "url";
  action.params[2] = "Waiting...";
  action.result = "In Progress";
  action.resultDetails = "";
  displayManager.displayAction(action);
  delay(2000);

  
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

    action.params[2] = "Fetched content";
    displayManager.displayAction(action);

    // Print the entire response
    //Serial.print(response);
    inverter.updateData(response);

    client.stop();

    Serial.printf("\nPower now: %f\n", inverter.getInverterPowerNow_W());
    Serial.printf("Energy today: %f\n", inverter.getInverterEnergyToday_kWh());
    Serial.printf("Energy total: %f\n", inverter.getInverterEnergyTotal_kWh());

    action.params[2] = "Parse: Done";
    action.result = "Done";
    action.resultDetails = "";
    displayManager.displayAction(action);

    delay(2000);

  } else {   
    action.params[1] = "Fetch: Failed";
    action.params[2] = "Parse: Failed";
    action.result = "FAIL";
    action.resultDetails = "";
    displayManager.displayAction(action);
    delay(5000);    
  }

}


////////////////////////////////////////////////////////////////////
// DISPLAY Section
void displayInverterStatus(const Inverter& inverter, unsigned int duration_ms) {
  
  displayManager.drawBigNumberWithHeader("Leistung aktuell", inverter.getInverterPowerNow_W(), "W", "",  "%.0f");
  delay(duration_ms/3);
  displayManager.drawBigNumberWithHeader("Energie heute", inverter.getInverterEnergyToday_kWh(), "kWh", "",  "%.1f");
  delay(duration_ms/3);
  displayManager.drawBigNumberWithHeader("Energie gesamt", inverter.getInverterEnergyTotal_kWh(), "kWh", "",  "%.1f");
  delay(duration_ms/3);
}

#ifdef USE_NTP_SYNC
void displayTime() {  
  int char_col_1 = 5; // Adjust as needed, based on character width and desired layout
  int char_col_2 = 12; // Additional column if needed for future use

  tmElements_t tm;
  breakTime(epochTime, tm);

  char dateStr[24]; // Enough to hold DD.MM.YYYY\0
  sprintf(dateStr,   "Date:   %02d.%02d.20%02d", tm.Day, tm.Month, (tm.Year + 1970) % 100);

  char timeStr[24]; // Enough to hold HH:MM:SS\0
  sprintf(timeStr,   "Time:   %02d:%02d:%02d", tm.Hour, tm.Minute, tm.Second);

  Serial.println("Displaying Time Sync Status...");
  Serial.print("Date: ");
  Serial.println(dateStr);

  Serial.print("Time: ");
  Serial.println(timeStr);

  
  // Display Sync Interval
  char syncStr[24]; // Assuming it fits, adjust size as needed
  long syncInterval = (millis() - lastUpdate) / 1000;
  sprintf(syncStr, "Last:   %ld s", syncInterval);

  Serial.print("Sync Interval (s): ");
  Serial.println(syncStr);

  u8g2.print(timeSynced ? "Yes" : "No");


  // Later, once more information is available or the action completes:
  action.params[1] = dateStr;
  action.params[2] = timeStr;
  action.params[3] = syncStr;

  if (timeSynced){
    action.result = "In Sync";
  }else{
    action.result = "Out of Sync";
  }

  displayManager.displayAction(action);
}
#endif

// ToDo: TLS Based MQTT connection

///////////////////////////////////////////////////////////////////
// MQTT SECTION
void submitDataViaMQTT(){
    
    action.name = "MQTT Sync";
    action.details = "Publish data";
    String brokerStr  = MQTT_BROKER_HOST.c_str();
    brokerStr  = "Broker " + brokerStr;
    action.params[0] = brokerStr.c_str();

    // Assuming MQTT_BROKER_PORT is an integer, you might need to convert it to a string
    char portStr[24]; // Large enough for typical port numbers (up to 65535) plus a null terminator
    sprintf(portStr, "Port:  %d", MQTT_BROKER_PORT);
    action.params[1] = portStr;

    action.params[2] = "Login: Waiting...";
    action.result = "In Progress";
    action.resultDetails = "";
    displayManager.displayAction(action);


    if (!mqtt_client.connected()) {
        mqtt_reconnect();
    }

     if (mqtt_client.connected()) {
        Serial.println("Connected to MQTT server");
        
        action.params[2] = "Login: Success";
        action.result = "In Progress";
        action.resultDetails = "";
        displayManager.displayAction(action);
        
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

      action.result = "Done";
      action.resultDetails = "Published";
      displayManager.displayAction(action);
      delay(2000);
    }else{
      action.params[2] = "Login: Failed";
      action.result = "Fail";
      action.resultDetails = "";
      displayManager.displayAction(action);
    }
     
}

void mqtt_reconnect() {
  int attempts = 0;
  while (!mqtt_client.connected() && attempts < 5) {
    Serial.print("Attempting MQTT connection...");
    if (mqtt_client.connect("deye-esp-solar-bridge", MQTT_BROKER_USER.c_str(), MQTT_BROKER_PWD.c_str())) {
      Serial.println("connected");  
      
      action.params[2] = "Login: Success";
      action.result = "In Progress";
      action.resultDetails = "";
      displayManager.displayAction(action);
      
    } else {
      action.result = "In Progress";
      action.resultDetails = "retry 2s";
      displayManager.displayAction(action);

      Serial.print("failed, rc=");
      Serial.print(mqtt_client.state());
      Serial.println("try again in 2 seconds");
      
      delay(2000);
    }
    attempts++;
  } 
}
