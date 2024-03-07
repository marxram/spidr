#include "WebServerManager.h"
#include "WebPages.h"

WebServerManager::WebServerManager() : server(80) {} // Initialize the web server on port 80

void WebServerManager::begin() {
    preferencesManager.begin(); // Initialize Preferences Manager
    setupRoutes();
    server.begin(); // Start the web server
}

void WebServerManager::stop() {
    server.close(); // Start the web server
}

void WebServerManager::handleClient() {
    server.handleClient(); // Handle incoming client requests
}

void WebServerManager::setupRoutes() {
    server.on("/", HTTP_GET, std::bind(&WebServerManager::handleRootPage, this));
    server.on("/update", HTTP_POST, std::bind(&WebServerManager::handleUpdate, this));
    // Define more routes as needed
}

void WebServerManager::handleRootPage() {
    String htmlContent = Preferences_HTML; // Preferences_HTML defined in WebPages.h
    
    // Dynamically replace placeholders with actual preference values
    htmlContent = templateProcessor(htmlContent);
    
    server.send(200, "text/html", htmlContent);
}


void WebServerManager::handleUpdate() {
    // Ensure content type is correct to avoid any processing errors
    if (server.hasHeader("Content-Type") && server.header("Content-Type") == "application/x-www-form-urlencoded") {
        // Update WiFi Settings
        if (server.hasArg("homeSSID")) preferencesManager.setHomeSSID(server.arg("homeSSID"));
        if (server.hasArg("homeKey")) preferencesManager.setHomeKey(server.arg("homeKey"));
        if (server.hasArg("inverterSSID")) preferencesManager.setInverterSSID(server.arg("inverterSSID"));
        if (server.hasArg("inverterKey")) preferencesManager.setInverterKey(server.arg("inverterKey"));
        if (server.hasArg("relaisSSID")) preferencesManager.setRelaisSSID(server.arg("relaisSSID"));
        if (server.hasArg("relaisKey")) preferencesManager.setRelaisKey(server.arg("relaisKey"));

        // Update MQTT Broker Settings
        if (server.hasArg("mqttBrokerHost")) preferencesManager.setMqttBrokerHost(server.arg("mqttBrokerHost"));
        if (server.hasArg("mqttBrokerPort")) preferencesManager.setMqttBrokerPort(server.arg("mqttBrokerPort").toInt());
        if (server.hasArg("mqttBrokerUser")) preferencesManager.setMqttBrokerUser(server.arg("mqttBrokerUser"));
        if (server.hasArg("mqttBrokerPwd")) preferencesManager.setMqttBrokerPwd(server.arg("mqttBrokerPwd"));
        if (server.hasArg("mqttBrokerMainTopic")) preferencesManager.setMqttBrokerMainTopic(server.arg("mqttBrokerMainTopic"));

        // Update Web Access Credentials
        if (server.hasArg("inverterWebUser")) preferencesManager.setInverterWebUser(server.arg("inverterWebUser"));
        if (server.hasArg("inverterWebPwd")) preferencesManager.setInverterWebPwd(server.arg("inverterWebPwd"));
        if (server.hasArg("relaisWebUser")) preferencesManager.setRelaisWebUser(server.arg("relaisWebUser"));
        if (server.hasArg("relaisWebPwd")) preferencesManager.setRelaisWebPwd(server.arg("relaisWebPwd"));

        // Redirect the client to the main page, or a confirmation page if you have one
        server.sendHeader("Location", "/", true); // Use HTTP 303 See Other for redirection after form submission
        server.send(303, "text/plain", "Preferences updated. Redirecting to main page...");
    } else {
        // If the content type is not as expected, respond with an error
        server.send(415, "text/plain", "Unsupported media type. Please submit the form again.");
    }
}

String WebServerManager::templateProcessor(const String& htmlTemplate) {
    String processedHtml = htmlTemplate;

    // Replace WiFi settings placeholders
    processedHtml.replace("{{homeSSID}}", preferencesManager.getHomeSSID());
    processedHtml.replace("{{homeKey}}", preferencesManager.getHomeKey());
    processedHtml.replace("{{inverterSSID}}", preferencesManager.getInverterSSID());
    processedHtml.replace("{{inverterKey}}", preferencesManager.getInverterKey());
    processedHtml.replace("{{relaisSSID}}", preferencesManager.getRelaisSSID());
    processedHtml.replace("{{relaisKey}}", preferencesManager.getRelaisKey());

    // Replace MQTT broker settings placeholders
    processedHtml.replace("{{mqttBrokerHost}}", preferencesManager.getMqttBrokerHost());
    processedHtml.replace("{{mqttBrokerPort}}", String(preferencesManager.getMqttBrokerPort()));
    processedHtml.replace("{{mqttBrokerUser}}", preferencesManager.getMqttBrokerUser());
    processedHtml.replace("{{mqttBrokerPwd}}", preferencesManager.getMqttBrokerPwd());
    processedHtml.replace("{{mqttBrokerMainTopic}}", preferencesManager.getMqttBrokerMainTopic());

    // Replace web access credentials placeholders
    processedHtml.replace("{{inverterWebUser}}", preferencesManager.getInverterWebUser());
    processedHtml.replace("{{inverterWebPwd}}", preferencesManager.getInverterWebPwd());
    processedHtml.replace("{{relaisWebUser}}", preferencesManager.getRelaisWebUser());
    processedHtml.replace("{{relaisWebPwd}}", preferencesManager.getRelaisWebPwd());

    // Additional placeholders can be replaced here in the same manner

    return processedHtml;
}
