#ifndef WebServerManager_h
#define WebServerManager_h


#ifdef ESP32
    #include <WebServer.h>
#endif

#ifdef ESP8266
    #include <ESP8266WebServer.h>
#endif

 // Or <WebServer.h> for ESP32
#include "PreferencesManager.h" // Include the Preferences Manager

class WebServerManager {
public:
    WebServerManager();
    void begin();
    void stop();
    void handleClient();


private:
    #ifdef ESP32
        WebServer server;
    #endif

    #ifdef ESP8266
        ESP8266WebServer server;
    #endif
    
    PreferencesManager preferencesManager;

    void setupRoutes();
    void handleRootPage();
    void handleUpdate();
    String templateProcessor(const String& var);
};

#endif
