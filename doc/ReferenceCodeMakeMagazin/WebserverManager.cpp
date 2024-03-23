
void WebServerManager::handleClient() {
    server.handleClient();
}

// Setup the web server. Welche ULRs sollen aufgerufen werden und welche Funtkione kümmert
// sich um den Zusammenbau und die Auslieferung der Inhalte
void WebServerManager::setupRoutes() {
    server.on("/", HTTP_GET, std::bind(&WebServerManager::handleRootPage, this));
    server.on("/serial", HTTP_GET, std::bind(&WebServerManager::handleSerialPage, this));
    server.on("/wiki", HTTP_GET, std::bind(&WebServerManager::handleWikiPage, this));
    server.on("/config", HTTP_GET, std::bind(&WebServerManager::handleConfigPage, this));
    server.on("/configoptions", HTTP_GET, std::bind(&WebServerManager::handleConfigPageOptions, this));
    server.on("/update", HTTP_POST, std::bind(&WebServerManager::handleUpdate, this));
    server.on("/updateoptions", HTTP_POST, std::bind(&WebServerManager::handleUpdateOptions, this));
}

// Diese Funktion nimmt die Roh Templates und ersetzt die Platzhalter mit echten Werten 
String WebServerManager::preparePagetemplate(String htmlRaw, String header, String pageHead){
    float power = inverter.getInverterPowerNow_W();
    float energyToday = inverter.getInverterEnergyToday_kWh();
    float energyTotal = inverter.getInverterEnergyTotal_kWh();

    // Variable Page für die später auszuliefernde Seite
    String page = htmlRaw;

    // Lade das Inferter Template in eine Variable
    String inverterRaw = INVERTER_HTML;
    // Ersetze die Werte in der Inverter HTML. Hier kommen dynamische Werte rein
    inverterRaw.replace("{{power}}", String(power));
    inverterRaw.replace("{{energyToday}}", String(energyToday));
    inverterRaw.replace("{{energyTotal}}", String(energyTotal));
    
    // Ersetze den Inverter Platzhalter mit der gerade angepassten version
    page.replace("{{INVERTER_STATUS}}", inverterRaw);

    // Ersetzeweitere statische Modulare Seitenmodule
    page.replace("{{MENU}}", MENU_HTML);
    page.replace("{{STYLES}}", STYLES_HTML);
    page.replace("{{FOOTER}}", FOOTER_HTML);
    page.replace("{{HEADLINE}}", header);
    page.replace("{{PAGEHEAD}}", pageHead);

    // Übergabe der vorbereiteten page
    return page;
}


void WebServerManager::handleConfigPage() {
    // Vorbereiten des Seitengerüsts, HTML, CSS, MENÜ, Inverter Hauptdaten
    String htmlContent = preparePagetemplate(ConfigPage_HTML, "Configuration of Essential Parameters",  "S|P|I|D|R - Config");
    // Ersetzen der Configurationsvariablen im HTML Template
    // Werden aus dem Preferences Manager gelesen und ersetzen die Platzhalter
    htmlContent = configPageTemplateProcessor(htmlContent);
    // Senden der Seite
    server.send(200, "text/html", htmlContent);
}

// Diese Funktion nimmt das HTML Template und ersetzt die Platzhalter mit den aktuellen Werten
String WebServerManager::configPageTemplateProcessor(const String& htmlTemplate) {
    String processedHtml = htmlTemplate;
    // Replace each placeholder with the corresponding preference value, using HTMLEscape to ensure safety
    processedHtml.replace("{{homeSSID}}", HTMLEscape(preferencesManager.getHomeSSID()));
    processedHtml.replace("{{homeKey}}", HTMLEscape(preferencesManager.getHomeKey()));
    // ... weitere analog
    return processedHtml;
}

// Diese Funktion is essentiell um die Formulardaten mit den geänderten Parametern
// entgegenzunehmen, die Daten zu speichern und dann auf eine Ergebnisseite zu leiten
void WebServerManager::handleUpdate() {
    if (server.method() == HTTP_POST) {
        // Extracting WiFi Settings
        String homeSSID = server.arg("homeSSID");
        String homeKey = server.arg("homeKey");
        // ... weitere analog

        // Schreiben der Werte in die Preferences
        preferencesManager.setHomeSSID(homeSSID);
        preferencesManager.setHomeKey(homeKey);
        // ...  weitere analog

        // Weiterleiten an eine Ergebnisseite
        server.sendHeader("Location", "/config", true);
        server.send(303, "text/plain", "Preferences updated. Redirecting to main page...");
    } else {
        server.send(405, "text/plain", "Method Not Allowed");
    }
}


// Ersetzen von HTML Sonderzeichen
String WebServerManager::HTMLEscape(const String& str) {
    String escapedStr = str;
    escapedStr.replace("&", "&amp;");
    escapedStr.replace("<", "&lt;");
    escapedStr.replace(">", "&gt;");
    escapedStr.replace("\"", "&quot;");
    escapedStr.replace("'", "&#39;");
    return escapedStr;
}



// Optional


void WebServerManager::handleSerialPage() {
    // Vorbereiten des Seitengerüsts, HTML, CSS, MENÜ, Inverter Hauptdaten
    String htmlContent = preparePagetemplate(Serial_HTML, "Last Serial Output for debugging", "S|P|I|D|R Serial Monitor");
    
    // Lesen der Serial Log Daten und ersetzen des Platzhalters
    String initialSerialData = serialCapture.getBuffer(); // Get the initial serial data
    htmlContent.replace("{{serial_data}}", HTMLEscape(initialSerialData));

    // Senden der Seite
    server.send(200, "text/html", htmlContent);
}

