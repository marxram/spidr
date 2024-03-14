#include "WebServerManager.h"
#include "WebPages.h"
#include "SerialCaptureLines.h"

WebServerManager::WebServerManager(Inverter& inverter, SerialCaptureLines& serialCapture) : server(80), inverter(inverter), serialCapture(serialCapture)   {}

void WebServerManager::begin() {
    serialCapture.println("WebServerManager: Initializing preferences manager...");
    preferencesManager.begin(); // Initialize Preferences Manager
    setupRoutes();
    server.begin(); // Start the web server
    serialCapture.println("WebServerManager: Web server started.");
    serverActive = true;
}

void WebServerManager::stop() {
    server.close(); // Close the web server
    serialCapture.println("WebServerManager: Web server stopped.");
    serverActive = false;
}

bool WebServerManager::isServerActive() {
    return serverActive;
}

void WebServerManager::handleClient() {
    server.handleClient(); // Handle incoming client requests
}

void WebServerManager::setupRoutes() {
    server.on("/", HTTP_GET, std::bind(&WebServerManager::handleRootPage, this));
    
    server.on("/wiki", HTTP_GET, std::bind(&WebServerManager::handleWikiPage, this));
    server.on("/config", HTTP_GET, std::bind(&WebServerManager::handleConfigPage, this));

    // Recieve the Config Updates here
    server.on("/update", HTTP_POST, std::bind(&WebServerManager::handleUpdate, this));
    // Define more routes as needed
}

void WebServerManager::handleRootPage() {
    String htmlContent = HomePage_HTML;

    // Fetch sensor readings from the Inverter instance
    float power = inverter.getInverterPowerNow_W();
    float energyToday = inverter.getInverterEnergyToday_kWh();
    float energyTotal = inverter.getInverterEnergyTotal_kWh();

    // Dynamically replace placeholders with actual sensor values
    htmlContent.replace("{{power}}", String(power));
    htmlContent.replace("{{energyToday}}", String(energyToday));
    htmlContent.replace("{{energyTotal}}", String(energyTotal));

    
    // Replace the {{serial_data}} placeholder with the initial serial data
    String initialSerialData = serialCapture.getBuffer(); // Get the initial serial data
    htmlContent.replace("{{serial_data}}", HTMLEscape(initialSerialData));

    // Use configPageTemplateProcessor to replace other placeholders
    //htmlContent = configPageTemplateProcessor(htmlContent);

    server.send(200, "text/html", htmlContent);
}

void WebServerManager::handleConfigPage() {
    String htmlContent = ConfigPage_HTML; // Preferences_HTML defined in WebPages.h
    
    // Dynamically replace placeholders with actual preference values
    htmlContent = configPageTemplateProcessor(htmlContent);

    // Fetch sensor readings from the Inverter instance
    float power = inverter.getInverterPowerNow_W();
    float energyToday = inverter.getInverterEnergyToday_kWh();
    float energyTotal = inverter.getInverterEnergyTotal_kWh();
    // Dynamically replace placeholders with actual sensor values
    htmlContent.replace("{{power}}", String(power));
    htmlContent.replace("{{energyToday}}", String(energyToday));
    htmlContent.replace("{{energyTotal}}", String(energyTotal));
    
    server.send(200, "text/html", htmlContent);
}

void WebServerManager::handleWikiPage() {
    String htmlContent = WikiPage_HTML; // Preferences_HTML defined in WebPages.h
    
    // Dynamically replace placeholders with actual preference values
    //htmlContent = configPageTemplateProcessor(htmlContent);

    // Fetch sensor readings from the Inverter instance
    float power = inverter.getInverterPowerNow_W();
    float energyToday = inverter.getInverterEnergyToday_kWh();
    float energyTotal = inverter.getInverterEnergyTotal_kWh();

    // Dynamically replace placeholders with actual sensor values
    htmlContent.replace("{{power}}", String(power));
    htmlContent.replace("{{energyToday}}", String(energyToday));
    htmlContent.replace("{{energyTotal}}", String(energyTotal));
    
    server.send(200, "text/html", htmlContent);
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

        serialCapture.println("Received form data:");
        serialCapture.println("WiFi Settings:");
        serialCapture.println("Home SSID: " + homeSSID);
        serialCapture.println("Home Key: " + homeKey);
        serialCapture.println("Inverter SSID: " + inverterSSID);
        serialCapture.println("Inverter Key: " + inverterKey);
        serialCapture.println("Relais SSID: " + relaisSSID);
        serialCapture.println("Relais Key: " + relaisKey);

        serialCapture.println("MQTT Broker Settings:");
        serialCapture.println("Broker Host: " + mqttBrokerHost);
        serialCapture.println("Broker Port: " + String(mqttBrokerPort));
        serialCapture.println("Broker User: " + mqttBrokerUser);
        serialCapture.println("Broker Password: " + mqttBrokerPwd);
        serialCapture.println("Main Topic: " + mqttBrokerMainTopic);

        serialCapture.println("Web Access Credentials:");
        serialCapture.println("Inverter Web User: " + inverterWebUser);
        serialCapture.println("Inverter Web Password: " + inverterWebPwd);
        serialCapture.println("Relais Web User: " + relaisWebUser);
        serialCapture.println("Relais Web Password: " + relaisWebPwd);

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
        serialCapture.println("Error: HTTP method not supported.");
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

String WebServerManager::configPageTemplateProcessor(const String& htmlTemplate) {
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

    return processedHtml;
}

String WebServerManager::HTMLEscape(const String& str) {
    String escapedStr = str;
    escapedStr.replace("&", "&amp;");
    escapedStr.replace("<", "&lt;");
    escapedStr.replace(">", "&gt;");
    escapedStr.replace("\"", "&quot;");
    escapedStr.replace("'", "&#39;");
    return escapedStr;
}