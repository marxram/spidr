#include <Arduino.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <U8x8lib.h>
#include <U8g2lib.h>

#include <time.h>

#include <freertos/FreeRTOS.h>   // Include the base FreeRTOS definitions.
#include <freertos/task.h>       // Include the task definitions.
#include <freertos/semphr.h>     // Include the semaphore definitions.
#include <freertos/ringbuf.h>    // Include the ringbuffer definitions.


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
bool connected = false;

unsigned long lastSyncTime; // Last NTP sync time in millis
unsigned long startTime =  0;
bool timeSynced = false;

bool apModeActive = false;
unsigned long apModeStartTime = 0;
int homeNetworkConnectionAttempts = 0;

bool initialAPModeActive = true; // To track the initial AP mode activation
unsigned long initialAPModeStartTime = millis();


bool connectedToHome = false;
bool connectedToInverter = false;


////////////////////////////////////////////////////////////////////
// Intializations 
DisplayManager displayManager;
ActionData action; // Action Structure to Display
MQTTManager* mqttManager = nullptr; // Pointer declaration
Inverter inverter;
InverterUdp inverterUdp;
PreferencesManager prefsManager;
WebServerManager webServerManager; // Create an instance of WebServerManager

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

// Operation Mode Management
SemaphoreHandle_t inverterTaskSemaphore;

SemaphoreHandle_t webServerSemaphore;
SemaphoreHandle_t switchSemaphore;

SemaphoreHandle_t homeNetworkSemaphore;
SemaphoreHandle_t inverterNetworkSemaphore;
SemaphoreHandle_t apModeActiveSemaphore;

// Task Declarations
void MainLoopTask(void *pvParameters);
void WebServerTask(void *pvParameters);
void InverterNetworkTask(void *pvParameters);
void HomeNetworkTask(void *pvParameters);



////////////////////////////////////////////////////////////////////
// SETUP Function

void setup() {
  Serial.begin(115200);
  delay(10); 

  


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
  action.name = "Action Item";
  action.details = "Action details";
  action.params[0] = "params[0]";
  action.params[1] = "params[1]";
  action.params[2] = "params[2]";
  action.params[3] = "params[3]";
  action.result = "Result";
  action.resultDetails = "Details";
  displayManager.displayAction(action);

  delay(5000);

   // Initialize semaphores
  inverterTaskSemaphore = xSemaphoreCreateBinary();
  webServerSemaphore = xSemaphoreCreateBinary();
  homeNetworkSemaphore = xSemaphoreCreateBinary();
  inverterNetworkSemaphore= xSemaphoreCreateBinary();
  apModeActiveSemaphore= xSemaphoreCreateBinary();

  // Initially, block inverter and home tasks, let web server run
  xSemaphoreGive(webServerSemaphore); // Start with the web server allowed to run

  // Create tasks
  xTaskCreatePinnedToCore(MainLoopTask, "MainLoop", 4096, NULL, 1, NULL, tskNO_AFFINITY);
  xTaskCreatePinnedToCore(WebServerTask, "WebServer", 4096, NULL, 1, NULL, tskNO_AFFINITY);
  xTaskCreatePinnedToCore(InverterNetworkTask, "InverterNetwork", 4096, NULL, 1, NULL, tskNO_AFFINITY);
  xTaskCreatePinnedToCore(HomeNetworkTask, "HomeNetwork", 4096, NULL, 1, NULL, tskNO_AFFINITY);
  xTaskCreatePinnedToCore(ConnectivityManagementTask,"ConnectivityManagement", 4096, NULL, 1,NULL, tskNO_AFFINITY );


  
  // Initiliase the NTP client, or fallback to build time if USE_NTP_SYNC is not defined
  Serial.println("Initialize Time...");
  
  setupTime();
  
}


////////////////////////////////////////////////////////////////////
// MAIN LOOP
void loop() {   
    Serial.println("Main Loop: Begin cycle");

    delay(10000); // Pause to observe the displayed information
    Serial.println("Main Loop: End cycle");
}

void MainLoopTask(void *pvParameters) {
    Serial.println("MainLoopTask: Started");
    for (;;) {
        Serial.println("MainLoopTask: Checking conditions");

        if (!apModeActive) {
            Serial.println("MainLoopTask: Print Inverter Data");
            inverter.printVariables();
        
            // Show most important values on the display
            displayInverterStatus(inverter, 6000);
            Serial.println("MainLoopTask: Displayed Inverter Status");
        }
        vTaskDelay(pdMS_TO_TICKS(10000)); // Delay to prevent a tight loop
    }
}

void WebServerTask(void *pvParameters) {
    Serial.println("WebServerTask: Started");
    for (;;) {
        //Serial.println("WebServerTask: Handling client");
        webServerManager.handleClient();
        vTaskDelay(10); // Short delay to yield
    }
}

void InverterNetworkTask(void *pvParameters) {
    Serial.println("InverterNetworkTask: Started");
    for (;;) {
        Serial.println("InverterNetworkTask: Waiting for semaphore");
        if (xSemaphoreTake(inverterNetworkSemaphore, portMAX_DELAY) == pdTRUE) {
            Serial.println("InverterNetworkTask: Semaphore acquired");
            webServerManager.stop();
            if (connected) {  
              Serial.println("InverterNetworkTask: Connected, performing actions");
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
            vTaskDelay(pdMS_TO_TICKS(10000)); // Simulate some work
            Serial.println("InverterNetworkTask: Actions completed, giving semaphore");
            xSemaphoreGive(inverterNetworkSemaphore);
        }
    }
}

void HomeNetworkTask(void *pvParameters) {
    Serial.println("HomeNetworkTask: Started");
    TickType_t lastWakeTime = xTaskGetTickCount();
    const TickType_t totalDurationTicks = pdMS_TO_TICKS(DURATION_STAY_IN_HOME_NETWORK * 1000);
    const TickType_t displayTimeDurationTicks = pdMS_TO_TICKS(DURATION_TO_DISPLAY_TIME_SECONDS * 1000);
    const TickType_t displayInverterDataDurationTicks = pdMS_TO_TICKS(DURATION_TO_DISPLAY_INVERTER_DATA * 1000);

    TickType_t nextSwitchTime = lastWakeTime + displayTimeDurationTicks;
    bool showTime = true;

    for (;;) {
        Serial.println("HomeNetworkTask: Waiting for semaphore");
        if (xSemaphoreTake(homeNetworkSemaphore, portMAX_DELAY) == pdTRUE) {
            Serial.println("HomeNetworkTask: Connected to Home Network, performing actions");
            // Perform Home Network actions
            if (connected) {
                mqttManager->publishAllData();
                // Delay to show the data on the Display
                vTaskDelay(pdMS_TO_TICKS(3000));
            }

                while (xTaskGetTickCount() - lastWakeTime < totalDurationTicks) {
                      if (xTaskGetTickCount() >= nextSwitchTime) {          
                          if (showTime) {
                              displayTime(DURATION_TO_DISPLAY_TIME_SECONDS);
                              nextSwitchTime += displayInverterDataDurationTicks;
                          } else {
                              displayInverterStatus(inverter, DURATION_TO_DISPLAY_INVERTER_DATA * 1000);
                              nextSwitchTime += displayTimeDurationTicks;
                          }
                          showTime = !showTime; // Toggle the display flag
                      }
                    vTaskDelay(pdMS_TO_TICKS(100)); // Short delay for responsiveness
                }
            Serial.println("HomeNetworkTask: Completed Home Network operations, giving semaphore");
            xSemaphoreGive(homeNetworkSemaphore);
            vTaskSuspend(NULL); // Suspend itself
        }
    }
}

void ConnectivityManagementTask(void *pvParameters) {
    for (;;) {
        // Check Home Network
        if (!connectedToHome) {
            Serial.println("Attempting to connect to Home WiFi...");
            wifi_connect(WIFI_HOME_SSID, WIFI_HOME_KEY, "Home WiFi");
            if (connected) {
                Serial.println("Connected to Home WiFi.");
                connectedToHome = true;
                xSemaphoreGive(homeNetworkSemaphore); // Allow Home Network tasks to proceed
            } else {
                Serial.println("Failed to connect to Home WiFi. Entering AP mode...");
                activateAPMode(); // Ensures AP mode is activated
                xSemaphoreGive(apModeActiveSemaphore); // Signal that AP mode is active
                // Wait in AP Mode for a defined time or until a client connects and configures the system
                vTaskDelay(pdMS_TO_TICKS(WIFI_AP_DURATION_SECONDS * 1000));
                xSemaphoreTake(apModeActiveSemaphore, portMAX_DELAY); // Wait until AP mode is concluded
            }
        }

        // Assume connection to Home Network succeeded or AP mode concluded
        if (connectedToHome) {
            xSemaphoreTake(homeNetworkSemaphore, portMAX_DELAY); // Ensure we control the flow
            // Perform operations while connected to Home Network...
            mqttManager->publishAllData();
            // After operations, attempt to connect to the Inverter Network
            wifi_connect(WIFI_INVERTER_SSID, WIFI_INVERTER_KEY, "Inverter WiFi");
            if (connected) {
                connectedToInverter = true; // Flag as connected to the Inverter Network
                xSemaphoreGive(inverterNetworkSemaphore); // Allow Inverter Network tasks to proceed
            }
        }

        // Handle Inverter Network operations
        if (connectedToInverter) {
            xSemaphoreTake(inverterNetworkSemaphore, portMAX_DELAY); // Wait for Inverter Network tasks to complete
            // Once done with Inverter Network, the cycle can start over.
            connectedToInverter = false; // Reset connection status
            connectedToHome = false; // This will cause the loop to try connecting to Home Network again
        }

        // Short delay before the next attempt or action
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

void manageAPMode(void *pvParameters) {
    for (;;) {
        if (apModeActive && (millis() - apModeStartTime > WIFI_AP_DURATION_SECONDS * 1000)) {
            Serial.println("AP mode timeout. Attempting to reconnect to Home WiFi...");
            wifi_connect(WIFI_HOME_SSID, WIFI_HOME_KEY, "Reconnecting to Home WiFi");
            
            // After attempt, reset the AP mode start time or deactivate AP mode based on success
            if (connected) {
                apModeActive = false; // Or another condition to deactivate AP mode
            } else {
                apModeStartTime = millis(); // Reset timer to retry after next interval
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10000)); // Check every 10 seconds or adjust as needed
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
    
    connected = false;
    WiFi.disconnect(true); // Ensure a clean state
    delay(1000); // Wait a bit for disconnect to complete
    
    WiFi.mode(WIFI_AP_STA); // Set mode to allow AP+STA
    WiFi.begin(ssid.c_str(), passkey.c_str());

    int attempts = 0;
    unsigned long attemptStartTime = millis();
    while (WiFi.status() != WL_CONNECTED && (millis() - attemptStartTime) < WIFI_AP_MODE_ATTEMPT_WINDOW_FOR_HOME_NET_S * 1000) { 
        int delay_loop = 1000; 
        delay(delay_loop);
        Serial.print(".");
        attempts++;
        
        String attemptMessage = "Try " + String(attempts) + " / " + String(WIFI_AP_MODE_ATTEMPT_WINDOW_FOR_HOME_NET_S * 1000 / delay_loop );
        // Update attempt count in real-time
        action.params[2] = attemptMessage;
        displayManager.displayAction(action);
    }

    if (WiFi.status() == WL_CONNECTED) {
        connected = true;
        Serial.println("\nConnected to WiFi network.");
        
        // Update Display with Success
        String ip_string = WiFi.localIP().toString();
        action.params[1] = "IP: " + ip_string;
        action.result = "Connected";
        action.resultDetails = "Success!";
        displayManager.displayAction(action);
        delay(3000); // Show success message for a while
    } else {
        connected = false;
        Serial.println("\nFailed to connect to WiFi network.");
        
        // Update Display with Failure
        action.params[1] = "Check Credentials?";
        action.params[2] = "Network Offline?";
        action.result = "FAILED";
        action.resultDetails = "";
        displayManager.displayAction(action);
        delay(5000); // Show failure message for a while
    }
}

void activateAPMode() {
    WiFi.mode(WIFI_AP_STA); // Enable AP+STA mode for simultaneous access point and Wi-Fi client mode
    WiFi.softAP(WIFI_AP_NAME, WIFI_AP_PASSWORD);
    Serial.println("AP mode activated with SSID: " + String(WIFI_AP_NAME) + ", IP: " + WiFi.softAPIP().toString());
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

    action.name     =  "Current Time";
    action.details  = "";


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
    }
}
