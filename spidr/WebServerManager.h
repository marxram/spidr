#ifndef WebServerManager_h
#define WebServerManager_h

#include "Inverter.h"


#ifdef ESP32
    #include <WebServer.h>
    #include <Update.h>
#endif

#ifdef ESP8266
    #include <ESP8266WebServer.h>
    #include <ESP8266httpUpdate.h>
#endif

 // Or <WebServer.h> for ESP32
#include "PreferencesManager.h" // Include the Preferences Manager

#include "SerialCaptureLines.h"

// Define a type for a callback function that takes no arguments and returns void
typedef void (*PreferencesCallback)();

class WebServerManager {

public:
    WebServerManager(Inverter& inverter, SerialCaptureLines& serialCapture); // Modified constructor
    void begin();
    void stop();
    void handleClient();
    bool isServerActive();
    void setPreferencesCallback(PreferencesCallback callback) {
        this->preferencesCallback = callback;
    }


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
    void handleSerialPage();
    void handleConfigPageOptions();
    void handleWikiPage();
    void handleOTAPage();
    String preparePagetemplate(String htmlRaw, String header, String pageHead);

    String HTMLEscape(const String& str);

    bool serverActive = false;

    void handleUpdate();
    void handleUpdateOptions();
    String configPageTemplateProcessor(const String& var);
    String configOptionsTemplateProcessor(const String& var);
    String rootPageTemplateProcessor(const String& htmlTemplate);

    Inverter& inverter; // Reference to the Inverter instance
    
    PreferencesCallback preferencesCallback = nullptr; // Initialize to nullptr
};

#endif
