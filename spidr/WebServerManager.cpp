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
    // Print debug message
    //serialCapture.println("WebServerManager: Handling client requests...");
    server.handleClient(); // Handle incoming client requests
}

void WebServerManager::setupRoutes() {
    // Print debug message
    serialCapture.println("WebServerManager: Setting up routes...");
    server.on("/", HTTP_GET, std::bind(&WebServerManager::handleRootPage, this));
    server.on("/serial", HTTP_GET, std::bind(&WebServerManager::handleSerialPage, this));

    server.on("/wiki", HTTP_GET, std::bind(&WebServerManager::handleWikiPage, this));
    server.on("/config", HTTP_GET, std::bind(&WebServerManager::handleConfigPage, this));
    server.on("/configoptions", HTTP_GET, std::bind(&WebServerManager::handleConfigPageOptions, this));

    // Recieve the Config Updates here
    server.on("/update", HTTP_POST, std::bind(&WebServerManager::handleUpdate, this));
    server.on("/updateoptions", HTTP_POST, std::bind(&WebServerManager::handleUpdateOptions, this));
}

String WebServerManager::preparePagetemplate(String htmlRaw, String header, String pageHead){
    // Print debug message    
    //serialCapture.println("WebServerManager: Preparing page template...");
    // Fetch sensor readings from the Inverter instance
    float power = inverter.getInverterPowerNow_W();
    float energyToday = inverter.getInverterEnergyToday_kWh();
    float energyTotal = inverter.getInverterEnergyTotal_kWh();

    //serialCapture.println("Energy Parsed...");

    String inverterRaw = INVERTER_HTML;
    String page = htmlRaw;

    // Dynamically replace placeholders with actual sensor values
    inverterRaw.replace("{{power}}", String(power));
    //serialCapture.println("Replaced power");

    inverterRaw.replace("{{energyToday}}", String(energyToday));
    //serialCapture.println("Replaced Energy Today");

    inverterRaw.replace("{{energyTotal}}", String(energyTotal));
    //serialCapture.println("Replaced Energy Total ");
    
    page.replace("{{INVERTER_STATUS}}", inverterRaw);
    //serialCapture.println("Replaced INVERTER");

    page.replace("{{MENU}}", MENU_HTML);
    //serialCapture.println("Replaced MENU");


    // Preparation for dark and light sheme
    if (true){
        page.replace("{{STYLES}}", STYLES_HTML_LIGHT);
    }else{
        page.replace("{{STYLES}}", STYLES_HTML_DARK);
    }
    //serialCapture.println("Replaced SYTLES");


    page.replace("{{FOOTER}}", FOOTER_HTML);
    //serialCapture.println("Replaced FOOTER");
    page.replace("{{HEADLINE}}", header);
    //serialCapture.println("Replaced HEADLINE");
    page.replace("{{PAGEHEAD}}", pageHead);


    return page;
}

void WebServerManager::handleRootPage() {
    // Print debug message
    //serialCapture.println("WebServerManager: Handling root page...");

    String htmlContent = preparePagetemplate(HomePage_HTML, "General Status", "Welcome to your S|P|I|D|R - Web Interface!");
    
    // Print debug message
    //serialCapture.println("WebServerManager: Sending root page...");
    htmlContent = rootPageTemplateProcessor(htmlContent);
    
    server.send(200, "text/html", htmlContent);
    // Print debug message
    //serialCapture.println("WebServerManager: Root page sent.");
}

void WebServerManager::handleSerialPage() {
    String htmlContent = preparePagetemplate(Serial_HTML, "Last Serial Output for debugging", "S|P|I|D|R Serial Monitor");
    
    String initialSerialData = serialCapture.getBuffer(); // Get the initial serial data
    htmlContent.replace("{{serial_data}}", HTMLEscape(initialSerialData));

    server.send(200, "text/html", htmlContent);
}


void WebServerManager::handleConfigPage() {
    String htmlContent = preparePagetemplate(ConfigPage_HTML, "Configuration of Essential Parameters",  "S|P|I|D|R - Config");
  
    // Dynamically replace placeholders with actual preference values
    htmlContent = configPageTemplateProcessor(htmlContent);
    
    server.send(200, "text/html", htmlContent);
}

void WebServerManager::handleConfigPageOptions() {
    String htmlContent = preparePagetemplate(ConfigPageOptions_HTML, "Configuration of Optional Parameters", "S|P|I|D|R Config Options");
  
    // Dynamically replace placeholders with actual preference values

    // Add print debug message
    //serialCapture.println("WebServerManager: Processing config options template...");
    // print whole htmlContent
    //serialCapture.println(htmlContent);
    htmlContent = configOptionsTemplateProcessor(htmlContent);
    //serialCapture.println(htmlContent);

    server.send(200, "text/html", htmlContent);
}

void WebServerManager::handleWikiPage() {
    String htmlContent = preparePagetemplate(WikiPage_HTML, "Knowledge Base and References", "S|P|I|D|R Wiki");
    
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

        if (preferencesCallback != nullptr) {
            serialCapture.println("RELOADING PREFERENCES");
            preferencesCallback(); // Call the callback function
        }


        server.sendHeader("Location", "/config", true);
        server.send(303, "text/plain", "Preferences updated. Redirecting to main page...");
    } else {
        // Handle incorrect method if needed
        serialCapture.println("Error: HTTP method not supported.");
        server.send(405, "text/plain", "Method Not Allowed");
    }
}

void WebServerManager::handleUpdateOptions() {
    if (server.method() == HTTP_POST) {
        // Previous code for extracting WiFi, MQTT, and Web Access Credentials remains unchanged

        // Extracting NTP Configuration
        String ntpServerA = server.arg("ntpA");
        String ntpServerB = server.arg("ntpB");
        
        bool isNtpActive = server.hasArg("ntpActive") && server.arg("ntpActive") == "on";
        serialCapture.println("NTP Active: " + String(isNtpActive ? "Yes" : "No"));

        int ntpGmtOffset = server.arg("ntpGmtOff").toInt();
        int ntpDstOffset = server.arg("ntpDstOff").toInt();

        // Extracting Timing Behavior Settings
        int homeNetMS = server.arg("homeNetMS").toInt();
        int homeFirstBootMS = server.arg("homeFirstBootMS").toInt();
        int apModeMS = server.arg("apModeMS").toInt();
        int wifiWaitS = server.arg("wifiWaitS").toInt();
        int invOffTimeoutS = server.arg("invOffTimeoutS").toInt();

        String apSSID = server.arg("apSSID");
        String apKey = server.arg("apKey");

        // Previous code for serial output of WiFi, MQTT, and Web Access Credentials remains unchanged

        // Serial output for NTP Configuration and Timing Behavior Settings
         serialCapture.println("\n[UpdateOptions] Values Rcieved:");
        
        serialCapture.println("NTP Configuration:");
        serialCapture.println("NTP Server A: " + ntpServerA);
        serialCapture.println("NTP Server B: " + ntpServerB);
        serialCapture.println("NTP Active: " + String(isNtpActive));
        serialCapture.println("GMT Offset: " + String(ntpGmtOffset));
        serialCapture.println("DST Offset: " + String(ntpDstOffset));

        serialCapture.println("Timing Behavior Settings:");
        serialCapture.println("Home Network Stay (ms): " + String(homeNetMS));
        serialCapture.println("First Boot Home Stay (ms): " + String(homeFirstBootMS));
        serialCapture.println("AP Mode Duration (ms): " + String(apModeMS));
        serialCapture.println("WiFi Connect Wait (s): " + String(wifiWaitS));
        serialCapture.println("Inverter Offline Timeout (s): " + String(invOffTimeoutS));
        serialCapture.println("AP SSID: " + apSSID);
        serialCapture.println("AP Key: " + apKey);

        // Previous code for updating the preferences with WiFi, MQTT, and Web Access Credentials remains unchanged

        // Updating the preferences with NTP Configuration and Timing Behavior Settings
        preferencesManager.setNtpServerA(ntpServerA);
        preferencesManager.setNtpServerB(ntpServerB);
        preferencesManager.setIsNtpActive(isNtpActive);
        preferencesManager.setNtpGmtOffset(ntpGmtOffset);
        preferencesManager.setNtpDstOffset(ntpDstOffset);

        preferencesManager.setTimingStayInHomeNetwork_MS(homeNetMS);
        preferencesManager.setTimingStayInHomeNetworkFirstBoot_MS(homeFirstBootMS);
        preferencesManager.setTimingStayInApMode_MS(apModeMS);
        preferencesManager.setTimingWiFiConnectWaitDuration_S(wifiWaitS);
        preferencesManager.setTimingInverterOfflineTimeout_S(invOffTimeoutS);
        preferencesManager.setApSSID(apSSID);
        preferencesManager.setApKey(apKey);


        /*
        Try to relaod (--> copy to variabled) of Preferences after writing. Otherwise the system should reboot
        if (preferencesCallback != nullptr) {
            serialCapture.println("RELOADING PREFERENCES");
            preferencesCallback(); // Call the callback function
        }
        */

        server.sendHeader("Location", "/configoptions", true);
        server.send(303, "text/plain", "Preferences updated. Redirecting to main page...");
    } else {
        // Handle incorrect method if needed
        serialCapture.println("Error: HTTP method not supported.");
        server.send(405, "text/plain", "Method Not Allowed");
    }
}


// Helper function to escape special HTML characters
String WebServerManager::HTMLEscape(const String& str) {
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

String WebServerManager::configOptionsTemplateProcessor(const String& htmlTemplate) {
    String processedHtml = htmlTemplate;


    // print Debug preferencesManager.getNtpServerA()
    serialCapture.println("WebServerManager: Processing config options template..." + preferencesManager.getNtpServerA());
    
    // New NTP and timing preferences
    processedHtml.replace("{{ntpA}}", HTMLEscape(preferencesManager.getNtpServerA()));
    processedHtml.replace("{{ntpB}}", HTMLEscape(preferencesManager.getNtpServerB()));
    processedHtml.replace("{{ntpActive}}", preferencesManager.getIsNtpActive() ? "checked" : "");
    processedHtml.replace("{{ntpGmtOff}}", HTMLEscape(String(preferencesManager.getNtpGmtOffset())));
    processedHtml.replace("{{ntpDstOff}}", HTMLEscape(String(preferencesManager.getNtpDstOffset())));
    processedHtml.replace("{{homeNetMS}}", HTMLEscape(String(preferencesManager.getTimingStayInHomeNetwork_MS())));
    processedHtml.replace("{{homeFirstBootMS}}", HTMLEscape(String(preferencesManager.getTimingStayInHomeNetworkFirstBoot_MS())));
    processedHtml.replace("{{apModeMS}}", HTMLEscape(String(preferencesManager.getTimingStayInApMode_MS())));
    processedHtml.replace("{{wifiWaitS}}", HTMLEscape(String(preferencesManager.getTimingWiFiConnectWaitDuration_S())));
    processedHtml.replace("{{invOffTimeoutS}}", HTMLEscape(String(preferencesManager.getTimingInverterOfflineTimeout_S())));
    processedHtml.replace("{{apSSID}}", HTMLEscape(String(preferencesManager.getApSSID())));
    processedHtml.replace("{{apKey}}", HTMLEscape(String(preferencesManager.getApKey())));

    return processedHtml;
}

String WebServerManager::rootPageTemplateProcessor(const String& htmlTemplate) {
    String processedHtml = htmlTemplate;
    // Replace placeholders with inverter data
    processedHtml.replace("{{webdata_sn}}", HTMLEscape(inverter.getInverterSerial()));
    processedHtml.replace("{{webdata_msvn}}", HTMLEscape(inverter.getWebdataMsvn()));
    processedHtml.replace("{{webdata_ssvn}}", HTMLEscape(inverter.getWebdataSsvn()));
    processedHtml.replace("{{webdata_pv_type}}", HTMLEscape(inverter.getWebdataPvType()));
    processedHtml.replace("{{webdata_rate_p}}", HTMLEscape(inverter.getWebdataRateP()));
    processedHtml.replace("{{webdata_now_p}}", HTMLEscape(String(inverter.getInverterPowerNow_W())));
    processedHtml.replace("{{webdata_today_e}}", HTMLEscape(String(inverter.getInverterEnergyToday_kWh())));
    processedHtml.replace("{{webdata_total_e}}", HTMLEscape(String(inverter.getInverterEnergyTotal_kWh())));
    processedHtml.replace("{{webdata_alarm}}", HTMLEscape(inverter.getWebdataAlarm()));
    processedHtml.replace("{{webdata_utime}}", HTMLEscape(inverter.getWebdataUtime()));
    processedHtml.replace("{{cover_mid}}", HTMLEscape(inverter.getLoggerModuleID()));
    processedHtml.replace("{{cover_ver}}", HTMLEscape(inverter.getLoggerSoftwareVersion()));
    processedHtml.replace("{{cover_wmode}}", HTMLEscape(inverter.getLoggerWifiMode()));
    processedHtml.replace("{{cover_ap_ssid}}", HTMLEscape(inverter.getLoggerApSsid()));
    processedHtml.replace("{{cover_ap_ip}}", HTMLEscape(inverter.getLoggerApIp()));
    processedHtml.replace("{{cover_ap_mac}}", HTMLEscape(inverter.getLoggerApMac()));
    processedHtml.replace("{{cover_sta_ssid}}", HTMLEscape(inverter.getLoggerStaSsid()));
    processedHtml.replace("{{cover_sta_rssi}}", HTMLEscape(inverter.getLoggerStaRssi()));
    processedHtml.replace("{{cover_sta_ip}}", HTMLEscape(inverter.getLoggerStaIp()));
    processedHtml.replace("{{cover_sta_mac}}", HTMLEscape(inverter.getLoggerStaMac()));
    String statusA = inverter.getRemoteServerStatusA().equals("1") ? "Connected" : "Not Connected";
    String statusB = inverter.getRemoteServerStatusB().equals("1") ? "Connected" : "Not Connected";
    String statusC = inverter.getRemoteServerStatusC().equals("1") ? "Connected" : "Not Connected";

    processedHtml.replace("{{status_a}}", HTMLEscape(statusA));
    processedHtml.replace("{{status_b}}", HTMLEscape(statusB));
    processedHtml.replace("{{status_c}}", HTMLEscape(statusC));
    return processedHtml;
}