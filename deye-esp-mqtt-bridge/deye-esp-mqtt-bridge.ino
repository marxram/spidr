#include <Arduino.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <U8x8lib.h>
#include <U8g2lib.h>

#include <time.h>

// Web and WiFi
#include <WiFiClient.h>
#include <base64.h>

#include "config.h"
#include "DisplayManager.h"
#include "MQTTManager.h"
// Inverter Data handling 
#include "Inverter.h"
// Include Udp Channel inverter communication
#include "InverterUdp.h"

///////////////////////////////////////////////////////////////////////
// Configuration and user settings
#include "arduino_secrets.h"

#ifdef ESP32
#include <WiFi.h> // ESP32 specific WiFi library
// ESP32 specific setup and functions
#endif

#ifdef ESP8266
#include <ESP8266WiFi.h> // ESP8266 specific WiFi library
// ESP8266 specific setup and functions
#endif



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


unsigned long lastSyncTime; // Last NTP sync time in millis
unsigned long startTime =  0;
bool timeSynced = false;

///////////////////////////////////////////////////////////////////////
// Global variables
bool connected = false;

////////////////////////////////////////////////////////////////////
// Intializations 
DisplayManager displayManager;
ActionData action; // Action Structure to Display
MQTTManager* mqttManager = nullptr; // Pointer declaration
Inverter inverter;
InverterUdp inverterUdp;

const int udpServerPort = 50000; // manual port
const int udpLocalPort = 48899; // Fixed port of deye inverter
const int udpTimeoput_s = 10; // 10 Seconds Timeout 
    

////////////////////////////////////////////////////////////////////
// Function declarations
void wifi_connect(String ssid, String passkey, String comment);
void readInverterDataFromWebInterface(String url, String web_user, String web_password);
void displayInverterStatus(const Inverter& inverter, unsigned int duration_ms);
void updateAndPublishData();
void setupTime();
unsigned long getCurrentEpochTime();
time_t buildTimeToEpoch(const char* date, const char* time);

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

  // Try to connect to the Home Network
  wifi_connect(WIFI_HOME_SSID, WIFI_HOME_KEY, "Home WiFi");


  if (mqttManager == nullptr) {
    mqttManager = new MQTTManager(MQTT_BROKER_HOST.c_str(), MQTT_BROKER_PORT, MQTT_BROKER_USER.c_str(), MQTT_BROKER_PWD.c_str(), displayManager, inverter);
  }

  // Initiliase the NTP client, or fallback to build time if USE_NTP_SYNC is not defined
  setupTime();
  
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

      unsigned long epochTime = getCurrentEpochTime();
      inverterUdp.inverter_settime(epochTime);
      
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
        unsigned long currentMillis = millis();
        if (timeSynced && (currentMillis - lastSyncTime > 600000)) { // 600,000 milliseconds = 10 minutes
            Serial.println("Attempting time resynchronization...");
            setupTime();
        }
        
        
        mqttManager->publishAllData();
        delay(3000); // Show data for 3 Seconds
        
        #ifdef USE_NTP_SYNC
          displayTime(10);
        #endif

        int secondsInHomeNetwork =0;
        while (secondsInHomeNetwork < DURATION_STAY_IN_HOME_NETWORK ){
          // Toggle between Time and Solar Production Screen
          displayInverterStatus(inverter, 10000);
          secondsInHomeNetwork+=10;
          
          #ifdef USE_NTP_SYNC
            displayTime(10);
            secondsInHomeNetwork+=10;
          #endif          
        }
    }

}




////////////////////////////////////////////////////////////////////
// Sync Time

void setupTime() {
    time_t now = time(nullptr);
    time_t buildEpoch = buildTimeToEpoch(__DATE__, __TIME__);
 
    // Only sync time if USE_NTP_SYNC is defined and it's the first run or last sync was unsuccessful
    #ifdef USE_NTP_SYNC
    if (!timeSynced || now <= buildEpoch) {
        
        action.name     =  "Time Sync";
        action.details  = "NTP_SERVER";
        action.params[0] = "GMT: " + String(GMT_OFFSET_SECONDS/3600);
        action.params[1] = "DST: " + String(DST_OFFSET_SECONDS/3600);
        action.result = "In Progress";
        action.resultDetails = "";
        displayManager.displayAction(action);
        
        configTime(GMT_OFFSET_SECONDS, DST_OFFSET_SECONDS, NTP_SERVER, NTP_FALLBACK_SERVER);
        delay(1000); // Give time for NTP request to complete

        now = time(nullptr); // Update current time after NTP request
        if (now > buildEpoch) {
            Serial.println("NTP sync successful.");
            lastSyncTime = millis(); // Record successful sync time
            timeSynced = true;
            action.result = "Done";
            action.resultDetails = "synced";
            displayManager.displayAction(action);
            delay(2000);
        } else {
            Serial.println("Failed to obtain time from NTP server. Using build time as fallback.");
            timeSynced = false; // NTP sync attempted and failed
            action.result = "Failed";
            action.resultDetails = "No Server";
            displayManager.displayAction(action);
            delay(2000);
        }
    } else {
        Serial.println("Time previously synchronized or NTP sync not required.");
    }
    #else
    if (now <= buildEpoch || !timeSynced) {
        
        // Use build time if NTP sync is disabled or if time hasn't been successfully set yet
        struct timeval tv = { .tv_sec = buildEpoch };
        settimeofday(&tv, NULL);
        Serial.println("NTP sync is disabled. Using build time as fallback.");
        lastSuccessfulSyncTime = millis();
        timeSynced = false; // Consider time as synchronized since we fallback to build time


        time_t now = time(NULL); // Get current time as time_t
        struct tm timeinfo; // Create a tm struct to hold local time
        localtime_r(&now, &timeinfo); // Convert time_t to tm as local time

        char dateStr[24]; // Buffer to hold formatted date string
        char timeStr[24]; // Buffer to hold formatted time string

        // Format date and time into strings
        strftime(dateStr, sizeof(dateStr), "Date: %Y-%m-%d", &timeinfo);
        strftime(timeStr, sizeof(timeStr), "Time: %H:%M:%S", &timeinfo);

        // Assuming 'action' and 'displayManager' are accessible here
        action.name     =  "Set Time";
        action.details  = "Using Build Time";
        action.params[0] = dateStr;
        action.params[1] = timeStr;
        action.params[2] = "";
        action.params[3] = "";
        action.result = "Done";
        action.resultDetails = "";
        displayManager.displayAction(action);
        delay(2000);
    }
    #endif
}

// Implement the helper function to convert build date and time to epoch time
time_t buildTimeToEpoch(const char* date, const char* time) {
    struct tm t;
    // Parse date and time from build strings and convert to time_t
    // Implementation depends on your environment and may require adjustments
    strptime(date, "%b %d %Y", &t);
    strptime(time, "%H:%M:%S", &t);
    return mktime(&t); // Convert to epoch time
}

unsigned long getCurrentEpochTime() {
    return time(nullptr); // Returns the current time as a time_t value which is epoch time
}


////////////////////////////////////////////////////////////////////
// WiFi Functions
void wifi_connect(String ssid, String passkey, String comment){
  Serial.println("----------------------------------------------------");
  Serial.print("Connecting to ");
  Serial.print(ssid);


  // Display Initialization
  action.name = comment;
  action.details    = "Connect to WiFi";
  String network    = "SSID: " + ssid;
  action.params[0]  = network.c_str();
  action.params[1]  = "IP:   Waiting...";
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

    String ip = "IP:   " + ip_string;
    action.params[1] = ip.c_str();
    //action.params[2] = "GW-IP: Waiting...";
    action.result = "Connected";
    action.resultDetails = "";
    displayManager.displayAction(action);
    delay(2000);
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
  action.params[1] = url;
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


void displayTime(int displayDurationSeconds) {
    const int delayPerIteration = 300; // Delay per iteration in ms
    int iterations = (displayDurationSeconds * 1000) / delayPerIteration;

    for (int i = 0; i < iterations; i++) {
        time_t now = time(nullptr); // Get the current time
        struct tm *timeinfo = localtime(&now); // Convert to local time structure

        char dateStr[24], timeStr[24], syncStr[24];
        strftime(dateStr, sizeof(dateStr), "Date:   %d.%m.%Y", timeinfo);
        strftime(timeStr, sizeof(timeStr), "Time:   %H:%M:%S", timeinfo);
        
        long syncInterval = (millis() - lastSyncTime) / 1000; // Convert milliseconds to seconds
        int hours = syncInterval / 3600; // Calculate total hours
        int minutes = (syncInterval % 3600) / 60; // Calculate remaining minutes
        int seconds = syncInterval % 60; // Calculate remaining seconds

        if (hours > 0) {
            // If there are hours, include them in the string
            sprintf(syncStr, "Last:   %dh %dm %2ds", hours, minutes, seconds);
        } else if (minutes > 0) {
            // If there are no hours but there are minutes, only include minutes and seconds
            sprintf(syncStr, "Last:   %dm %ds", minutes, seconds);
        } else {
            // If there are only seconds, just include seconds
            sprintf(syncStr, "Last:   %2ds", seconds);
        }

        // Assume action is a global or properly passed to this function
        action.params[0] = dateStr;
        action.params[1] = timeStr;
        action.params[2] = syncStr;
        action.result = timeSynced ? "In Sync" : "Out of Sync";

        // Call your display manager's method to update the display
        displayManager.displayAction(action);

        delay(delayPerIteration);
    }
}


void updateAndPublishData() {
    if (mqttManager != nullptr) {
    // Now you can call methods on mqttManager
    mqttManager->publishAllData();
    }
}
