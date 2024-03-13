#include <Arduino.h>
#include <PubSubClient.h>
#include <Wire.h>
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

#include "PreferencesManager.h"
#include "WebServerManager.h"

#include "EnergyDisplay.h"


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


///////////////////////////////////////////////////////////////////////
// Global variables

// State Machine
unsigned long lastStateChangeMillis = 0; // Last time the state was changed
unsigned long lastInverterUpdateMillis = 0; // Timer for inverter update interval


unsigned long lastSyncTime; // Last NTP sync time in millis
unsigned long startTime =  0;
bool timeSynced = false;

// Flags for network status
bool connectedToHomeNetwork = false;
bool connectedToInverterNetwork = false ;
bool activatedAP = false;

bool connected = false;


////////////////////////////////////////////////////////////////////
// Intializations 
DisplayManager displayManager;
ActionData action; // Action Structure to Display
MQTTManager* mqttManager = nullptr; // Pointer declaration
Inverter inverter;
InverterUdp inverterUdp;
PreferencesManager prefsManager;
WebServerManager webServerManager; // Create an instance of WebServerManager
EnergyDisplay energyDisplay(displayManager);

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
void loadPreferencesIntoVariables();
//void manageAPMode(void *pvParameters);
void activateAPMode();
void displayConnected(String networkType, String ipAddress);
void clearActionDisplay();


// State Management Functions
void updateStateMachine();

void handleInverterNetworkMode();
void handleHomeNetworkMode();
void handleAPMode();

// Condition checking functions
bool cndHomeNetworkToAPNetwork();
bool cndHomeNetworkToInverterNetwork();
bool cndAPToHomeNetwork();
bool hasClientConnected();

// Enum to define different states
enum State {
    INVERTER_NETWORK_MODE,
    HOME_NETWORK_MODE,
    AP_MODE
};
State currentState; 
State previousState; 

unsigned long inverterNotReachableCount = 0;
unsigned long homeNetworkNotReachableCount = 0;
bool newInverterDataAvailable = false;
int remainingTimeInAP = 0;


////////////////////////////////////////////////////////////////////
// SETUP Function

void setup() {
  Serial.begin(115200);
  delay(500); 

  // Initialize Preferences Manager
  Serial.println("Initialize Preferences Manager...");
  prefsManager.begin();

  Serial.println("Load Preferences into Variables...");
  loadPreferencesIntoVariables();

  #ifdef SCREEN_ADDRESS
    displayManager.setI2CAddress(SCREEN_ADDRESS); 
  #endif 

  Serial.println("Initialize Display Manager...");
  displayManager.init();
   // Show also more output and Parameters like Url, IP etc. 
  displayManager.verboseDisplay = true;

  // Display Initialization
  action.name = "Airgap Bridge";
  action.details = "Security & Privacy";
  action.params[0] = "Read Solar Inverters";
  action.params[1] = "Display Data";
  action.params[2] = "Send to SmartHome";
  action.params[3] = "deye-esp-mqtt-bridge";
  action.result = "Star on";
  action.resultDetails = "Github";
  displayManager.displayAction(action);
  delay(5000);
  clearActionDisplay();
  
  // Initiliase the NTP client, or fallback to build time if USE_NTP_SYNC is not defined
  Serial.println("Initialize Time...");
  setupTime();


    // Initialize the MQTT Manager
    
  if (mqttManager == nullptr) {
    Serial.println("Initialize MQTT Manager...");
    mqttManager = new MQTTManager(MQTT_BROKER_HOST.c_str(), MQTT_BROKER_PORT, MQTT_BROKER_USER.c_str(), MQTT_BROKER_PWD.c_str(), displayManager, inverter);
  }

  // Initialize the State Machine
  currentState = HOME_NETWORK_MODE;
  homeNetworkNotReachableCount = 0;
  lastStateChangeMillis = millis();
  wifi_connect(WIFI_HOME_SSID, WIFI_HOME_KEY, "Home WiFi");
  energyDisplay.initializeDisplayIntervals(5000, 5000, 5000);
  energyDisplay.start();

}


////////////////////////////////////////////////////////////////////
// MAIN LOOP
void loop() {   
    // Update the state machine
    updateStateMachine();

    delay(200); // Dummy delay to simulate network activity
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
        lastSyncTime = millis();
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

void loadPreferencesIntoVariables() {
    WIFI_HOME_SSID = prefsManager.getHomeSSID();
    WIFI_HOME_KEY = prefsManager.getHomeKey();
    Serial.println("Loaded Home WiFi SSID: " + WIFI_HOME_SSID);
    Serial.println("Loaded Home WiFi Key: [HIDDEN]"); // For security reasons, you might not want to print the actual key

    WIFI_INVERTER_SSID = prefsManager.getInverterSSID();
    WIFI_INVERTER_KEY = prefsManager.getInverterKey();
    Serial.println("Loaded Inverter WiFi SSID: " + WIFI_INVERTER_SSID);
    Serial.println("Loaded Inverter WiFi Key: [HIDDEN]");

    WIFI_RELAIS_SSID = prefsManager.getRelaisSSID();
    WIFI_RELAIS_KEY = prefsManager.getRelaisKey();
    Serial.println("Loaded Relais WiFi SSID: " + WIFI_RELAIS_SSID);
    Serial.println("Loaded Relais WiFi Key: [HIDDEN]");

    MQTT_BROKER_HOST = prefsManager.getMqttBrokerHost();
    MQTT_BROKER_PORT = prefsManager.getMqttBrokerPort();
    Serial.println("Loaded MQTT Broker Host: " + MQTT_BROKER_HOST);
    Serial.print("Loaded MQTT Broker Port: ");
    Serial.println(MQTT_BROKER_PORT);

    MQTT_BROKER_USER = prefsManager.getMqttBrokerUser();
    MQTT_BROKER_PWD = prefsManager.getMqttBrokerPwd();
    Serial.println("Loaded MQTT Broker User: " + MQTT_BROKER_USER);
    Serial.println("Loaded MQTT Broker Password: [HIDDEN]");

    MQTT_BROKER_MAINTOPIC = prefsManager.getMqttBrokerMainTopic();
    Serial.println("Loaded MQTT Broker Main Topic: " + MQTT_BROKER_MAINTOPIC);

    INVERTER_WEBACCESS_USER = prefsManager.getInverterWebUser();
    INVERTER_WEBACCESS_PWD = prefsManager.getInverterWebPwd();
    Serial.println("Loaded Inverter Web Access User: " + INVERTER_WEBACCESS_USER);
    Serial.println("Loaded Inverter Web Access Password: [HIDDEN]");
}

////////////////////////////////////////////////////////////////////
// WiFi Functions
void wifi_connect(String ssid, String passkey, String comment) {
    Serial.println("----------------------------------------------------");
    Serial.println("Attempting to connect to WiFi");
    
    // Display Initialization
    action.name = comment;
    action.details = "Connect to WiFi";
    action.params[0] = "SSID: " + ssid;
    action.params[1] = "IP: Waiting...";
    action.params[2] = "";
    action.params[3] = "";
    action.result = "In Progress";
    action.resultDetails = "";
    displayManager.displayAction(action);
    
    WiFi.disconnect(true); // Ensure a clean state
    delay(1000); // Wait a bit for disconnect to complete
    WiFi.mode(WIFI_AP_STA); // Set mode to allow AP+STA

    bool connected = false;
    int connectionAttempts = 0;
    const int maxConnectionAttempts = 2; // Try to connect up to 2 times
    
    while (!connected && connectionAttempts < maxConnectionAttempts) {
        WiFi.begin(ssid.c_str(), passkey.c_str());
        
        unsigned long attemptStartTime = millis();
        while (WiFi.status() != WL_CONNECTED && (millis() - attemptStartTime) < WIFI_AP_MODE_ATTEMPT_WINDOW_FOR_HOME_NET_S * 1000) {
            delay(300);
            Serial.print(".");
            
            unsigned long timeElapsed = millis() - attemptStartTime;
            int secondsLeft = (WIFI_AP_MODE_ATTEMPT_WINDOW_FOR_HOME_NET_S * 1000 - timeElapsed) / 1000;
            String attemptMessage = "Time left: " + String(secondsLeft) + " s";
            action.params[2] = attemptMessage;
            displayManager.displayAction(action);
        }

        if (WiFi.status() == WL_CONNECTED) {
            connected = true;
            Serial.println("\nConnected to WiFi network.");

            if (ssid == WIFI_HOME_SSID) {
                connectedToHomeNetwork = true;
                connectedToInverterNetwork = false;
            } else if (ssid == WIFI_INVERTER_SSID) {
                connectedToHomeNetwork = false;
                connectedToInverterNetwork = true;
            } else {
                connectedToHomeNetwork = false;
                connectedToInverterNetwork = false;
            }

            String ip_string = WiFi.localIP().toString();
            action.params[1] = "IP: " + ip_string;
            action.params[2] = "";
            action.result = "Connected";
            action.resultDetails = "Success!";
            displayManager.displayAction(action);
            delay(3000);
            break; // Exit the while loop
        } else {
            connected = false;
            Serial.println("\nFailed to connect to WiFi network. Retrying...");
            
            action.result = "Retry";
            action.resultDetails = "";
            displayManager.displayAction(action);

            // Increment connection attempts
            connectionAttempts++;
            
            // Reset WiFi module before next attempt
            if (connectionAttempts < maxConnectionAttempts) {
                WiFi.disconnect(true);
                delay(1000); // Wait for the disconnect and reset to complete
            }
        }
    }

    if (!connected) {
        connectedToHomeNetwork = false;
        connectedToInverterNetwork = false;

        if (ssid == WIFI_HOME_SSID) {
            homeNetworkNotReachableCount++;
        } else if (ssid == WIFI_INVERTER_SSID) {
            inverterNotReachableCount++;
        }

        Serial.println("Failed to connect after multiple attempts.");
        action.params[1] = "Check Credentials?";
        action.params[2] = "Network Offline?";
        action.result = "FAILED";
        action.resultDetails = "Attempts";
        displayManager.displayAction(action);
        delay(5000);
    }
    clearActionDisplay();
}

void clearActionDisplay(){
    // Display Initialization
    action.name = "";
    action.details = "";
    action.params[0] = "";
    action.params[1] = "";
    action.params[2] = "";
    action.params[3] = "";
    action.result = "";
    action.resultDetails = "";
    displayManager.clearScreen();
}

void activateAPMode() {
    WiFi.mode(WIFI_AP); // Enable AP+STA mode for simultaneous access point and Wi-Fi client mode
    WiFi.softAP(WIFI_AP_NAME, WIFI_AP_PASSWORD);
    Serial.println("AP mode activated with SSID: " + String(WIFI_AP_NAME) + ", IP: " + WiFi.softAPIP().toString());
    delay(1000); // Wait for the AP to start
    webServerManager.begin();

    action.name = "AP Starting";
    action.details = "";
    action.result = "";
    action.resultDetails = "";
    displayManager.displayAction(action); // Update the display with the current state
    delay(2000); 
}

void deactivateAPMode() {
    WiFi.mode(WIFI_STA);
    Serial.println("AP mode deactivated. Switched to STA mode.");

    // Add action and displaymanager update to inform about the status
    action.name = "AP Mode";
    action.details = "Deactivated";
    action.params[0] = "";
    action.params[1] = "";
    action.params[2] = "";
    action.params[3] = "";
    action.result = "Closed";
    action.resultDetails = "";
    displayManager.displayAction(action);
    webServerManager.stop();
    delay(2000);
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
  action.params[3] = "";
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
    
    Serial.print("Start Parsing");
    ParseStatus result = inverter.updateData(response);
    
    Serial.print(String(result));

    client.stop();

    Serial.printf("\nPower now: %f\n", inverter.getInverterPowerNow_W());
    Serial.printf("Energy today: %f\n", inverter.getInverterEnergyToday_kWh());
    Serial.printf("Energy total: %f\n", inverter.getInverterEnergyTotal_kWh());

    // Check parse result and update display accordingly
    if (result == OK) {
      action.params[2] = "Parse: Done";
      action.result = "Done";
      action.resultDetails = "Success";
    } else {
      action.params[2] = "Parse: Failed";
      action.result = "FAIL";
      action.resultDetails = "Parsing Error";
    }

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


void displayConnected(String networkType, String ipAddress) {
    action.name = networkType;
    action.details = "Connected";
    action.params[0] = "SSID: " + (networkType == "Home Network" ? WIFI_HOME_SSID : WIFI_AP_NAME);
    action.params[1] = "IP: " + ipAddress;
    action.params[2] = "";
    action.params[3] = "";
    action.result = "Active";
    action.resultDetails = "";
    displayManager.displayAction(action);
}

void updateAndPublishData() {
    if (mqttManager != nullptr) {
    // Now you can call methods on mqttManager
    mqttManager->publishAllData();
    }else{
        Serial.println("MQTT Manager not initialized");
    }
}

// State Machine

void updateStateMachine() {
    switch (currentState) {
        case INVERTER_NETWORK_MODE:
            handleInverterNetworkMode();
            break;
        case HOME_NETWORK_MODE:
            handleHomeNetworkMode();
            break;
        case AP_MODE:
            handleAPMode();
            break;
    }
}

void handleInverterNetworkMode() {   
    // Store the previous state
    previousState = currentState;

    Serial.println("Entering Inverter Network Mode");
    // If connected with the Solar Inverter
    
    // check if connection is available
    if (connectedToInverterNetwork  && (WiFi.status() == WL_CONNECTED) ) {  
        inverterNotReachableCount = 0; 
        // Starting UDP Connection inside the AP Network of inverter
        
        // Display Initialization
        action.name = "Inverter";
        action.details = "Init UDP Connection";
        action.params[0] = "Status: Login";
        action.params[1] = "";
        action.params[2] = "";
        action.params[3] = "";
        action.result = "Pending";
        action.resultDetails = "";
        displayManager.displayAction(action);

        bool udpConnectionCreated =  inverterUdp.inverter_connect(WiFi.gatewayIP().toString(),udpServerPort, udpLocalPort, udpTimeoput_s);

        if (!udpConnectionCreated){
            action.params[0] = "Status: Failed";
            action.result = "Retry";
            action.resultDetails = "";
            displayManager.displayAction(action);
            delay(1000);
            udpConnectionCreated =  inverterUdp.inverter_connect(WiFi.gatewayIP().toString(),udpServerPort, udpLocalPort, udpTimeoput_s);
        }

        if (udpConnectionCreated){
            action.params[0] = "Status: Connected";
            action.result = "Connected";
            action.resultDetails = "";
            displayManager.displayAction(action);
            delay(1000);

            action.details = "Get Time";
            action.params[0] = "";
            action.result = "Waiting...";
            action.resultDetails = "";
            displayManager.displayAction(action);
            delay(1000);

            // Getting time from inverter, stored in inverterUDP object
            String response = inverterUdp.inverter_readtime();
            // Print the entire response
            Serial.print(response);
        
            if (response == TIME_NOT_INITIALIZTED_TOKEN){
                Serial.print("Time needs to be set");
                
                action.params[0] = "Status: Time not set";
                action.params[1] = "Trigger Time Sync ";
                action.result = "Waiting...";
                action.resultDetails = "";
                displayManager.displayAction(action);
                delay(1000);
                response = inverterUdp.inverter_settime(getCurrentEpochTime());
                Serial.print(response);

                String response_new = inverterUdp.inverter_readtime();
                // Print the entire response
                Serial.print(response_new);
                if (response_new == TIME_NOT_INITIALIZTED_TOKEN){
                    Serial.print(response_new);
                    action.params[0] = "Status: Time set";
                    action.params[1] = "Time Sync";
                    action.result = "FAILED";
                    action.resultDetails = "NOT synced";
                    displayManager.displayAction(action);
                    delay(2000);
                }else{
                    action.params[0] = "Status: Time set";
                    action.params[1] = "Time Sync ";
                    action.result = "Done";
                    action.resultDetails = "synced";
                    displayManager.displayAction(action);
                    delay(2000);
                }
            }
        }else{            
            action.params[0] = "Status: Failed";
            action.result = "Failed";
            action.resultDetails = "";
            displayManager.displayAction(action);
            delay(2000);
        }


        // Set the time of the inverter to the current time
        // This is also resetting the energy Production Today Counter, if time was not set before
        unsigned long epochTime = getCurrentEpochTime();
        inverterUdp.inverter_settime(epochTime);

        // Close connection before leaving to Home Network
        inverterUdp.inverter_close();

        // Retrieving the data from the inverter via the web interface
        readInverterDataFromWebInterface(status_page_url, INVERTER_WEBACCESS_USER, INVERTER_WEBACCESS_PWD);
        newInverterDataAvailable = true; 
        lastInverterUpdateMillis = millis();

        // Output Information 
        Serial.println("Print Inverter Data");
        inverter.printVariables();
    }else{
        connectedToInverterNetwork = false;
        connectedToHomeNetwork = false;
        inverterNotReachableCount++;
        Serial.println("[ERR] No WiFi Connection!!");

        // Special case if inverter is not reachable for a long time, like every night ;-)
        if (inverter.isInverterActive() && (lastInverterUpdateMillis > INVERTER_OFFLINE_TIMEOUT_SECONDS * 1000 )) {
            Serial.println("Inverter not reachable for too long. Setting Power to 0W");
            inverter.setInactiveValues();

            // Enforce MQTT sync with 0W Power
            newInverterDataAvailable = true;
        }
    }

    if (cndInverterNetworkToHomeNetwork()) {
        Serial.println("Exiting Inverter Network Mode --> Home Network Mode");
       
        action.result = "Inverter";
        action.resultDetails = "-> HOME";
        displayManager.displayAction(action); // Update the display with the current state
        delay(2000); 


        wifi_connect(WIFI_HOME_SSID, WIFI_HOME_KEY, "Home WiFi");
        currentState = HOME_NETWORK_MODE;
        lastStateChangeMillis = millis();
        energyDisplay.start(); // Start display updates
    }   
}

void handleHomeNetworkMode() {
    // Store the previous state
    previousState = currentState;
    // Display Inverter Data for 10 Seconds
    // Now in an async way
    energyDisplay.updateDisplay(inverter);

    // check if connection is available
    if (connectedToHomeNetwork && (WiFi.status() == WL_CONNECTED)) {
        //Serial.println("[DBG] HomeNetwork Internal Loop");
        homeNetworkNotReachableCount = 0; 

        if (!webServerManager.isServerActive()) {
            Serial.println("Start Web Server Manager");
            webServerManager.begin();
        }

        if (webServerManager.isServerActive()) {
            //Serial.println("Handle Webserver Client");
            webServerManager.handleClient();
        }
        
        // Update and Publish Data if new data is available  
        if (newInverterDataAvailable){
        // Debugging MQTT
        //if (true){
            Serial.println("New inverter Data available --> Send data via MQTT");
            updateAndPublishData();
            newInverterDataAvailable = false;
            delay(3000); // Show data for 3 Seconds
        }
        

        // Display Time for 10 Seconds
        //displayTime(DURATION_TO_DISPLAY_TIME_SECONDS);

    }else{
        connectedToInverterNetwork = false;
        connectedToHomeNetwork = false;
        homeNetworkNotReachableCount++;
        Serial.println("[ERR] No WiFi Connection!!");
    }
    
    // Check if we need to switch to AP Mode or Inverter Network Mode
    if (cndHomeNetworkToAPNetwork()) {
        Serial.println("Switching to AP Mode due to timeout.");
        
        // Close the WebServer
        webServerManager.stop();

        action.result = "Home";
        action.resultDetails = "-> AP";
        displayManager.displayAction(action); // Update the display with the current state
        delay(2000); 
        
        currentState = AP_MODE;
        lastStateChangeMillis = millis();
        homeNetworkNotReachableCount = 0; 
        activateAPMode();
    } else if (cndHomeNetworkToInverterNetwork()) {
        Serial.println("Switching to Inverter Network Mode for data update.");
        
        // Close the WebServer
        webServerManager.stop();

        action.result = "Home";
        action.resultDetails = "-> INV";
        displayManager.displayAction(action); // Update the display with the current state
        delay(2000); 

        wifi_connect(WIFI_INVERTER_SSID, WIFI_INVERTER_KEY, "Inverter WiFi");
        currentState = INVERTER_NETWORK_MODE;
        lastStateChangeMillis = millis();
        homeNetworkNotReachableCount = 0; 
    }
}

void handleAPMode() {
    // Store the previous state
    previousState = currentState;
    
    // Serial.println("In AP Mode");
    
    connectedToHomeNetwork = false;
    connectedToInverterNetwork = false;
    activatedAP = true;

    remainingTimeInAP = ( DURATION_STAY_IN_AP_NETWORK_MS - (millis() - lastStateChangeMillis)  ) / 1000;
    action.result = "Remaining";
    action.resultDetails = String(remainingTimeInAP) + "s";
    displayManager.displayAction(action);

    if (webServerManager.isServerActive()) {
        webServerManager.handleClient();
    }

    // Display Initialization
    action.name = "AP Mode";
    action.details = "Connect Now";
    // Convert SSID from WIFI_AP_NAME to String and add to action.params[0]
    action.params[0] = "SSID: " + String(WIFI_AP_NAME);

    // get AP IP and create a String Variable to display
    String ip_string = WiFi.softAPIP().toString();
    
    action.params[1] = "PSWD: " + String(WIFI_AP_PASSWORD);
    action.params[2] = "IP:   "+ ip_string;
    
    action.params[3] = "";
    // calculate how long the AP Mode will still be actiove baed on the millis count
    int remainingTime = DURATION_STAY_IN_AP_NETWORK_MS - (millis() - lastStateChangeMillis) / 1000;
    action.result = "Remaining";
    action.resultDetails = String(remainingTimeInAP) + "s";
    displayManager.displayAction(action);


    if (cndAPToHomeNetwork()) {
        Serial.println("No client connected. Switching back to Home Network Mode.");
        
        // deactivate AP Mode
        deactivateAPMode();
        activatedAP = false;


        action.name = "AP Stop";
        action.details = "Shutdown";
        action.result = "AP";
        action.resultDetails = "-> HOME";
        displayManager.displayAction(action); // Update the display with the current state
        delay(2000); 

        currentState = HOME_NETWORK_MODE;
        lastStateChangeMillis = millis();
        homeNetworkNotReachableCount = 0;

        wifi_connect(WIFI_HOME_SSID, WIFI_HOME_KEY, "Home WiFi");

        energyDisplay.start(); // Start display updates
    }
}

// Implementation of condition checking functions
bool cndHomeNetworkToAPNetwork() {
   // return ( ( millis() - lastStateChangeMillis > HOME_NETWORK_TIMEOUT) && homeNetworkNotReachableCount > 3  );
    return ( homeNetworkNotReachableCount > 3 );
}

bool cndHomeNetworkToInverterNetwork() {
    long timeToNextTry = DURATION_STAY_IN_HOME_NETWORK_MS ;
    if (inverterNotReachableCount > 3) {
        timeToNextTry = (10 * 60 * 1000 + DURATION_STAY_IN_HOME_NETWORK_MS);
    }

    return (millis() - lastStateChangeMillis > timeToNextTry);
}

bool cndInverterNetworkToHomeNetwork(){
    // No restrictions to leave Inverter Network Mode
    return true;
};

bool cndAPToHomeNetwork() {
    return (!hasClientConnected() && millis() - lastStateChangeMillis > DURATION_STAY_IN_AP_NETWORK_MS);
}

bool hasClientConnected() {
    // ToDo: Implement the logic to check if a client is connected to the AP
    return false;
}



