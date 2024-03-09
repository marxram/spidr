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
}

void WebServerManager::stop() {
    server.close(); // Close the web server
    Serial.println("WebServerManager: Web server stopped.");
}

void WebServerManager::handleClient() {
    server.handleClient(); // Handle incoming client requests
}

void WebServerManager::setupRoutes() {
    Serial.println("WebServerManager: Configuring routes...");
    server.on("/", HTTP_GET, std::bind(&WebServerManager::handleRootPage, this));
    server.on("/update", HTTP_POST, std::bind(&WebServerManager::handleUpdate, this));
    // Define more routes as needed
    Serial.println("WebServerManager: Routes configured.");
}

void WebServerManager::handleRootPage() {
    Serial.println("WebServerManager: Handling root page request...");
    String htmlContent = Preferences_HTML; // Preferences_HTML defined in WebPages.h
    
    // Dynamically replace placeholders with actual preference values
    htmlContent = templateProcessor(htmlContent);
    
    server.send(200, "text/html", htmlContent);
    Serial.println("WebServerManager: Root page served.");
}

void WebServerManager::handleUpdate() {
    Serial.println("WebServerManager: Handling update...");
    // Ensure content type is correct to avoid any processing errors
    if (server.hasHeader("Content-Type") && server.header("Content-Type") == "application/x-www-form-urlencoded") {
        Serial.println("WebServerManager: Processing form submission...");
        // Update WiFi Settings
        // [Update code remains the same as you provided]
        
        // After updates are applied
        Serial.println("WebServerManager: Preferences updated, redirecting...");
        server.sendHeader("Location", "/", true); // Use HTTP 303 See Other for redirection
        server.send(303, "text/plain", "Preferences updated. Redirecting to main page...");
    } else {
        // If the content type is not as expected, respond with an error
        Serial.println("WebServerManager: Unsupported media type.");
        server.send(415, "text/plain", "Unsupported media type. Please submit the form again.");
    }
}

String WebServerManager::templateProcessor(const String& htmlTemplate) {
    Serial.println("WebServerManager: Processing HTML template...");
    String processedHtml = htmlTemplate;
    
    // [Placeholder replacement code remains the same as you provided]

    Serial.println("WebServerManager: HTML template processed.");
    return processedHtml;
}
