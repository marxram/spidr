#ifndef WebServerManager_h
#define WebServerManager_h

#include "Inverter.h"


#ifdef ESP32
    #include <WebServer.h>
#endif

#ifdef ESP8266
    #include <ESP8266WebServer.h>
#endif

 // Or <WebServer.h> for ESP32
#include "PreferencesManager.h" // Include the Preferences Manager

#include "SerialCaptureLines.h"

class WebServerManager {
public:
    WebServerManager(Inverter& inverter, SerialCaptureLines& serialCapture); // Modified constructor
    void begin();
    void stop();
    void handleClient();
    bool isServerActive();


private:
    #ifdef ESP32
        WebServer server;
    #endif

    #ifdef ESP8266
        ESP8266WebServer server;
    #endif
    
    SerialCaptureLines& serialCapture;
    PreferencesManager preferencesManager;

    void setupRoutes();
    void handleRootPage();
    void handleConfigPage();
    void handleWikiPage();
    void handleSerialPage();

    bool serverActive = false;

    void handleUpdate();
    String templateProcessor(const String& var);
    Inverter& inverter; // Reference to the Inverter instance
};

#endif
