#include "WebServerManager.h"
#include "WebPages.h"

WebServerManager::WebServerManager() : server(80) {} // Initialize the web server on port 80

void WebServerManager::begin() {
    Serial.println("WebServerManager: Initializing preferences manager...");
    preferencesManager.begin(); // Initialize Preferences Manager
    Serial.println("WebServerManager: Setting up routes...");
    setupRoutes();
    server.begin(); // Start the web server
    Serial.println("WebServerManager: Web server started.");
    serverActive = true;
}

void WebServerManager::stop() {
    server.close(); // Close the web server
    Serial.println("WebServerManager: Web server stopped.");
    serverActive = false;
}

bool WebServerManager::isServerActive() {
    return serverActive;
}

void WebServerManager::handleClient() {
    server.handleClient(); // Handle incoming client requests
}

void WebServerManager::setupRoutes() {
    Serial.println("WebServerManager: Configuring routes...");
    server.on("/", HTTP_GET, std::bind(&WebServerManager::handleRootPage, this));
    
    server.on("/wiki", HTTP_GET, std::bind(&WebServerManager::handleWikiPage, this));
    server.on("/config", HTTP_GET, std::bind(&WebServerManager::handleConfigPage, this));

    // Recieve the Config Updates here
    server.on("/update", HTTP_POST, std::bind(&WebServerManager::handleUpdate, this));

    // Define more routes as needed
    Serial.println("WebServerManager: Routes configured.");
}

void WebServerManager::handleRootPage() {
    Serial.println("WebServerManager: Handling root page request...");
    String htmlContent = HomePage_HTML; // Preferences_HTML defined in WebPages.h
    
    // Dynamically replace placeholders with actual preference values
    htmlContent = templateProcessor(htmlContent);
    
    server.send(200, "text/html", htmlContent);
    Serial.println("WebServerManager: Root page served.");
}

void WebServerManager::handleConfigPage() {
    Serial.println("WebServerManager: Handling root page request...");
    String htmlContent = ConfigPage_HTML; // Preferences_HTML defined in WebPages.h
    
    // Dynamically replace placeholders with actual preference values
    htmlContent = templateProcessor(htmlContent);
    
    server.send(200, "text/html", htmlContent);
    Serial.println("WebServerManager: Root page served.");
}

void WebServerManager::handleWikiPage() {
    Serial.println("WebServerManager: Handling root page request...");
    String htmlContent = WikiPage_HTML; // Preferences_HTML defined in WebPages.h
    
    // Dynamically replace placeholders with actual preference values
    htmlContent = templateProcessor(htmlContent);
    
    server.send(200, "text/html", htmlContent);
    Serial.println("WebServerManager: Root page served.");
}



void WebServerManager::handleUpdate() {
    if (server.method() == HTTP_POST) {
        // Extracting WiFi Settings
        String homeSSID = server.arg("homeSSID");
        String homeKey = server.arg("homeKey");
        String inverterSSID = server.arg("inverterSSID");
        String inverterKey = server.arg("inverterKey");
        String relaisSSID = server.arg("relaisSSID");
        String relaisKey = server.arg("relaisKey");
        
        // Extracting MQTT Broker Settings
        String mqttBrokerHost = server.arg("mqttBrokerHost");
        int mqttBrokerPort = server.arg("mqttBrokerPort").toInt(); // Assuming it's sent as a valid integer
        String mqttBrokerUser = server.arg("mqttBrokerUser");
        String mqttBrokerPwd = server.arg("mqttBrokerPwd");
        String mqttBrokerMainTopic = server.arg("mqttBrokerMainTopic");
        
        // Extracting Web Access Credentials
        String inverterWebUser = server.arg("inverterWebUser");
        String inverterWebPwd = server.arg("inverterWebPwd");
        String relaisWebUser = server.arg("relaisWebUser");
        String relaisWebPwd = server.arg("relaisWebPwd");

        Serial.println("Received form data:");
        Serial.println("WiFi Settings:");
        Serial.println("Home SSID: " + homeSSID);
        Serial.println("Home Key: " + homeKey);
        Serial.println("Inverter SSID: " + inverterSSID);
        Serial.println("Inverter Key: " + inverterKey);
        Serial.println("Relais SSID: " + relaisSSID);
        Serial.println("Relais Key: " + relaisKey);

        Serial.println("MQTT Broker Settings:");
        Serial.println("Broker Host: " + mqttBrokerHost);
        Serial.println("Broker Port: " + String(mqttBrokerPort));
        Serial.println("Broker User: " + mqttBrokerUser);
        Serial.println("Broker Password: " + mqttBrokerPwd);
        Serial.println("Main Topic: " + mqttBrokerMainTopic);

        Serial.println("Web Access Credentials:");
        Serial.println("Inverter Web User: " + inverterWebUser);
        Serial.println("Inverter Web Password: " + inverterWebPwd);
        Serial.println("Relais Web User: " + relaisWebUser);
        Serial.println("Relais Web Password: " + relaisWebPwd);

        // Updating the preferences with the received data
        preferencesManager.setHomeSSID(homeSSID);
        preferencesManager.setHomeKey(homeKey);
        preferencesManager.setInverterSSID(inverterSSID);
        preferencesManager.setInverterKey(inverterKey);
        preferencesManager.setRelaisSSID(relaisSSID);
        preferencesManager.setRelaisKey(relaisKey);
        preferencesManager.setMqttBrokerHost(mqttBrokerHost);
        preferencesManager.setMqttBrokerPort(mqttBrokerPort);
        preferencesManager.setMqttBrokerUser(mqttBrokerUser);
        preferencesManager.setMqttBrokerPwd(mqttBrokerPwd);
        preferencesManager.setMqttBrokerMainTopic(mqttBrokerMainTopic);
        preferencesManager.setInverterWebUser(inverterWebUser);
        preferencesManager.setInverterWebPwd(inverterWebPwd);
        preferencesManager.setRelaisWebUser(relaisWebUser);
        preferencesManager.setRelaisWebPwd(relaisWebPwd);

        // Redirecting to the root page after successful update
        server.sendHeader("Location", "/", true);
        server.send(303, "text/plain", "Preferences updated. Redirecting to main page...");
    } else {
        // Handle incorrect method if needed
        Serial.println("Error: HTTP method not supported.");
        server.send(405, "text/plain", "Method Not Allowed");
    }
}

// Helper function to escape special HTML characters
String HTMLEscape(const String& str) {
    String escapedStr = str;
    escapedStr.replace("&", "&amp;");
    escapedStr.replace("<", "&lt;");
    escapedStr.replace(">", "&gt;");
    escapedStr.replace("\"", "&quot;");
    escapedStr.replace("'", "&#39;");
    return escapedStr;
}

String WebServerManager::templateProcessor(const String& htmlTemplate) {
    Serial.println("WebServerManager: Processing HTML template...");
    String processedHtml = htmlTemplate;

    // Replace each placeholder with the corresponding preference value, using HTMLEscape to ensure safety
    processedHtml.replace("{{homeSSID}}", HTMLEscape(preferencesManager.getHomeSSID()));
    processedHtml.replace("{{homeKey}}", HTMLEscape(preferencesManager.getHomeKey()));
    processedHtml.replace("{{inverterSSID}}", HTMLEscape(preferencesManager.getInverterSSID()));
    processedHtml.replace("{{inverterKey}}", HTMLEscape(preferencesManager.getInverterKey()));
    processedHtml.replace("{{relaisSSID}}", HTMLEscape(preferencesManager.getRelaisSSID()));
    processedHtml.replace("{{relaisKey}}", HTMLEscape(preferencesManager.getRelaisKey()));
    processedHtml.replace("{{mqttBrokerHost}}", HTMLEscape(preferencesManager.getMqttBrokerHost()));
    processedHtml.replace("{{mqttBrokerPort}}", HTMLEscape(String(preferencesManager.getMqttBrokerPort())));
    processedHtml.replace("{{mqttBrokerUser}}", HTMLEscape(preferencesManager.getMqttBrokerUser()));
    processedHtml.replace("{{mqttBrokerPwd}}", HTMLEscape(preferencesManager.getMqttBrokerPwd()));
    processedHtml.replace("{{mqttBrokerMainTopic}}", HTMLEscape(preferencesManager.getMqttBrokerMainTopic()));
    processedHtml.replace("{{inverterWebUser}}", HTMLEscape(preferencesManager.getInverterWebUser()));
    processedHtml.replace("{{inverterWebPwd}}", HTMLEscape(preferencesManager.getInverterWebPwd()));
    processedHtml.replace("{{relaisWebUser}}", HTMLEscape(preferencesManager.getRelaisWebUser()));
    processedHtml.replace("{{relaisWebPwd}}", HTMLEscape(preferencesManager.getRelaisWebPwd()));

    Serial.println("WebServerManager: HTML template processed.");
    return processedHtml;
}