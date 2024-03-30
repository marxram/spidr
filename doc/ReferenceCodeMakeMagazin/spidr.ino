#include <Arduino.h>
// MQTT Client
#include <PubSubClient.h>
// I2C und Library für das Display
#include <Wire.h>
#include <U8g2lib.h>

// Zeit und RTC Library
#include <time.h>
#include <TimeLib.h>
#include <RTClib.h>

// WiFi Library abhängig von der Hardware
#ifdef ESP32
#include <WiFi.h> // ESP32 specific WiFi library
// ESP32 specific setup and functions
#endif

#ifdef ESP8266
#include <ESP8266WiFi.h> // ESP8266 specific WiFi library
// ESP8266 specific setup and functions
#endif

// HTTP Client
#include <WiFiClient.h>
#include <base64.h>


// Eigene Klassen und Tools inkludieren 
#include "Inverter.h"
#include "InverterUdp.h"
#include "PreferencesManager.h"
#include "DisplayManager.h"
#include "WebServerManager.h"
#include "EnergyDisplay.h"
#include "SerialCaptureLines.h"
#include "MQTTManager.h"

// Passwörter und Konfiguration, die zur Kompilierzeit gesetzt werden können. 
// Die meisten können per Webserver überschrieben werden. 
#include "arduino_secrets.h"
#include "config.h"


////////////////////////////////////////////////////////////////////
// Intializations 
SerialCaptureLines serialCapture(300); // Adjust the buffer size (number of lines) as needed
DisplayManager displayManager(serialCapture);
ActionData action; // Action Structure to Display
MQTTManager* mqttManager = nullptr; // Pointer declaration
Inverter inverter(serialCapture);
InverterUdp inverterUdp(serialCapture);
PreferencesManager prefsManager;
WebServerManager webServerManager (inverter,  serialCapture); // Create an instance of WebServerManager
EnergyDisplay energyDisplay(displayManager, timeSynced, lastSyncTime, serialCapture);


///////////////////////////////////////////////////////////////////////
// Other rather static parameters of the Inveter and the Webserver
String status_page_url = "status.html" ;
const int udpServerPort = 50000; // manual port
const int udpLocalPort = 48899; // Fixed port of deye inverter
const int udpTimeoput_s = 10; // 10 Seconds Timeout 



// Enum zur Definition verschiedener Zustände
enum State {
    INVERTER_NETWORK_MODE, // Modus für Netzwerkbetrieb des Wechselrichters
    HOME_NETWORK_MODE, // Modus für Heimnetzwerk
    AP_MODE // Access-Point-Modus
};
State currentState; // Aktueller Zustand
State previousState; // Vorheriger Zustand

void setup() {
    Serial.begin(115200); // Startet die serielle Verbindung
    delay(500); // Kurze Pause
    prefsManager.begin(); // Initialisiert den Einstellungs-Manager
    loadPreferencesIntoVariables(); // Lädt Einstellungen in Variablen
    // Initialisiert das Display mit Startbildschirm
    displayManager.init();
    action.name = "S|P|I|D|R"; // Setzt den Aktion-Namen
    displayManager.displayAction(action); // Zeigt die Aktion auf dem Display an
    setupTime(); // Initialisiert die Zeit
    // Initialisiert den MQTT Manager
    mqttManager = new MQTTManager(MQTT_BROKER_HOST.c_str(), MQTT_BROKER_PORT,
        MQTT_BROKER_USER.c_str(), MQTT_BROKER_PWD.c_str(), displayManager, 
        inverter, serialCapture);
    currentState = HOME_NETWORK_MODE; // Setzt den aktuellen Zustand
    inverter.initializeDataBuffer(0); // Initialisiert den Datenpuffer des Wechselrichters
}

void loop() {
    static unsigned long lastSetupTimeCalled = 0; // Speichert, wann setupTime zuletzt aufgerufen wurde
    const unsigned long intervalSetupTime = 120000; // Intervall für setupTime in Millisekunden (2 Minuten)
    static unsigned long lastStatemachineCalled = 0; // Speichert, wann die Zustandsmaschine zuletzt aufgerufen wurde
    const unsigned long intervalStatemachine = 200; // Intervall für die Zustandsmaschine in Millisekunden
    // Aktualisiert die Zustandsmaschine regelmäßig
    if (millis() - lastStatemachineCalled >= intervalStatemachine) {
        updateStateMachine(); // Aktualisiert die Zustandsmaschine
        lastStatemachineCalled = millis(); // Aktualisiert den Zeitpunkt des letzten Aufrufs
    }
    // Aktualisiert die Zeit alle 2 Minuten
    if (millis() - lastSetupTimeCalled >= intervalSetupTime) {
        setupTime(); // Aktualisiert die Zeit
        lastSetupTimeCalled = millis(); // Aktualisiert den Zeitpunkt des letzten Aufrufs
    }
    delay(5); // Kurzes Delay, um den Loop zu verlangsamen
}

// Funktionen der Zustandsmaschine
void updateStateMachine() {
    switch (currentState) {
        case INVERTER_NETWORK_MODE: // Behandelt den Wechselrichter-Netzwerkmodus
            handleInverterNetworkMode();
            break;
        case HOME_NETWORK_MODE: // Behandelt den Heimnetzwerkmodus
            handleHomeNetworkMode();
            break;
        case AP_MODE: // Behandelt den Access-Point-Modus
            handleAPMode();
            break;
    }
}



void setupTime() {
    // Nutzt die Zeit entweder von NTP, oder falls nicht verfügbar die Build Time
    // ...
}

void loadPreferencesIntoVariables() {
    // Laden der Parameter und Schreiben in Lokale Variablen
    // ...
}

void wifi_connect(String ssid, String passkey, String comment) {
    // Zur Fehlervermeidung wird die WiFi Verbindung getrennt
    WiFi.disconnect(true);  
    delay(500); 
    WiFi.mode(WIFI_AP_STA); // Setze den Station Mode
    //...
    // Für das Inverter WiFi wird die IP manuell gesetzt um nicht das WiFi Relais zu stören
    if (ssid == WIFI_INVERTER_SSID) {
        IPAddress staticIP(10, 10, 100, 149);
        IPAddress gateway(10, 10, 100, 254); 
        IPAddress subnet(255, 255, 255, 0); 
        IPAddress dns(8, 8, 8, 8); // Wird nicht benötigt
        WiFi.config(staticIP, gateway, subnet, dns);
    }else{
        // Use DHCP for other Networks
        WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
    }
    // Verbindung zum Netzwerk wird aufgebaut 
    WiFi.begin(ssid.c_str(), passkey.c_str());
    // .. Fehlerbehebung und Versuche ...
}

void activateAPMode() {   
    WiFi.mode(WIFI_AP); // Enable AP+STA mode for simultaneous access point and Wi-Fi client mode
    WiFi.softAP(WIFI_AP_NAME, WIFI_AP_PASSWORD);
    webServerManager.begin();
}

void deactivateAPMode() {
    WiFi.mode(WIFI_STA);
    webServerManager.stop();
}


bool readInverterDataFromWebInterface(String url, String web_user, String web_password){
    String serverIp = WiFi.gatewayIP().toString();
    String website = "http://" + serverIp + "/" + url;
    bool readHTML = false;
    WiFiClient client;

    if (client.connect(serverIp.c_str(), 80)) {
        // Erstelle die Authentifizierungsinformationen
        String authHeaderValue = "Basic " + base64::encode(web_user + ":" + web_password);
        // Sende den Request um die Statusseite auszulesen
        client.println("GET /" + url + " HTTP/1.1");
        client.println("Host: " + serverIp);
        client.println("Authorization: " + authHeaderValue);
        client.println("Connection: close");
        client.println();
        // Alle wichtigen Informationen werden in der Variablen gespeichert. 
        String response = "";
        String line = "";
        while (client.connected()) {
            // So lange Zeichen im Puffer verfügbar sind werden sie eingelesen
            while (client.available()) {
                char c = client.read();
                    if (c == '\n') {
                        // Wenn die Zugangsdaten flasch sind ist die Antwort 401 Unauthorized und muss gesondert behandelt werden
                        if (line.indexOf("<H4>401 Unauthorized</H4>") > 0) {
                            readHTML = false;
                            return readHTML;
                        }
                        // Alle wichtigen parameter starten mit "var ..." nur diese Zeilen sollen gespeichert werden   
                        if (line.startsWith("var webdata") || line.startsWith("var cover") || line.startsWith("var status")) {
                            // Entferne den var Teil
                            line.replace("var ", "");
                            // die Zeile mit der neuen Variablen wird an die Response gehangen
                            response += line + "\n";
                        }
                        line = "";
                    } else if (c != '\r') {
                        line += c;
                    }
            }
        }
        // Parsen der Daten mit der Funktion in der Inverter Klasse 
        ParseStatus result = inverter.updateData(response);
        client.stop();   
    } else {   
        // Keine Verbindung möglich
        // ...
    }
  return readHTML;
}

bool readInverterDataFromWebInterface(String url, String web_user, String web_password){
    String serverIp = WiFi.gatewayIP().toString();      // Wandelt die IP des Gateways in einen String um
    String website = "http://" + serverIp + "/" + url;  // Erstellt die URL zur Webseite
    bool readHTML = false;                              // Initialisiert die Variable, die angibt, ob HTML gelesen wurde
    WiFiClient client;                                  // Erstellt ein WiFiClient-Objekt

    if (client.connect(serverIp.c_str(), 80)) {         // Verbindet zum Server über Port 80
        // Erstellt den Wert für die HTTP-Basic-Authentifizierung
        String authHeaderValue = "Basic " + base64::encode(web_user + ":" + web_password); 
        client.println("GET /" + url + " HTTP/1.1");    // Sendet den HTTP GET Request
        client.println("Host: " + serverIp);            // Sendet den Host-Header
        client.println("Authorization: " + authHeaderValue); // Sendet den Authorization-Header
        client.println("Connection: close");            // Fordert an, dass die Verbindung nach der Antwort geschlossen wird
        client.println();                               // Beendet den HTTP-Header
        String response = "";                           // Initialisiert die Antwort-String
        String line = "";                               // Initialisiert den Zeilen-String
        while (client.connected()) {                    // Solange der Client verbunden ist
            while (client.available()) {                // Solange Daten vom Server verfügbar sind
                char c = client.read();                 // Liest das nächste Zeichen
                if (c == '\n') {                        // Wenn das Zeichen ein Zeilenumbruch ist
                    if (line.indexOf("<H4>401 Unauthorized</H4>") > 0) { // Überprüft auf 401 Unauthorized Antwort
                        readHTML = false;               // Setzt readHTML auf false
                        return readHTML;                // Beendet die Funktion frühzeitig
                    }
                    // Überprüft, ob die Zeile mit einer relevanten Variablen beginnt
                    if (line.startsWith("var webdata") || line.startsWith("var cover") || line.startsWith("var status")) { 
                        line.replace("var ", "");       // Entfernt "var " vom Anfang der Zeile
                        response += line + "\n";        // Fügt die Zeile zur Antwort hinzu
                    }
                    line = "";                          // Setzt die Zeile zurück
                } else if (c != '\r') {                 // Wenn das Zeichen kein Wagenrücklauf ist
                    line += c;                          // Fügt das Zeichen zur Zeile hinzu
                }
            }
        }
        ParseStatus result = inverter.updateData(response); // Parsen und Aktualisieren der Daten
        client.stop();                                  // Stoppt die Verbindung zum Server
    } else {   
        // Hier könnte Logik ergänzt werden, um auf fehlgeschlagene Verbindungen zu reagieren
        // Keine Verbindung möglich
    }
  return readHTML; // Gibt zurück, ob das HTML erfolgreich gelesen wurde
}



void handleInverterNetworkMode() {   
    // Überprüfung, ob die Verbindung zum Wechselrichter-Netzwerk erfolgreich war
    if (connectedToInverterNetwork && (WiFi.status() == WL_CONNECTED)) {  
        // Herstellung einer UDP-Verbindung, um die korrekte Zeit zu setzen
        bool udpConnectionCreated =  inverterUdp.inverter_connect(WiFi.gatewayIP().toString(), udpServerPort, udpLocalPort, udpTimeout_s);
        if (udpConnectionCreated){
            String response = inverterUdp.inverter_readtime(); // Liest die Zeit vom Wechselrichter
            if (inverterUdp.isDefaultTimeSet()){ // Überprüfung, ob die Standardzeit gesetzt ist und ggf. Anpassung
                response = inverterUdp.inverter_settime(getCurrentEpochTime()); // Setzt die aktuelle Zeit
                String response_new = inverterUdp.inverter_readtime(); // Liest die neu gesetzte Zeit
            }
            inverterUdp.inverter_close(); // Schließt die UDP-Session
        }else {
            // Keine UDP-Verbindung möglich
            // ...
        }
        // Auslesen der HTML-Seite vom Wechselrichter und Parsen der Daten
        bool readHTML = readInverterDataFromWebInterface(status_page_url, INVERTER_WEBACCESS_USER, INVERTER_WEBACCESS_PWD);
        if (readHTML) {
            newInverterDataAvailable = true; // Neue Daten sind verfügbar
            lastInverterUpdateMillis = millis();        
        }        
        inverter.printVariables(); // Ausgabe des Wechselrichter-Status im Serial Monitor
    }else{
         if ((millis() - lastInverterUpdateMillis > INVERTER_OFFLINE_TIMEOUT_SECONDS * 1000)) {
            inverter.setInactiveValues(); // Setzt inaktive Werte, falls der Wechselrichter nicht erreichbar ist
            newInverterDataAvailable = true; // Signalisiert, dass neue Daten verfügbar sind
        }

        if (hour() == 0 && inverter.getInverterEnergyToday_kWh() > 0.0) {
            inverter.resetEnergyTodayCounter(); // Zurücksetzen des Tageszählers um Mitternacht
            newInverterDataAvailable = true; // Signalisiert, dass neue Daten verfügbar sind
        }
    }
    currentState = HOME_NETWORK_MODE; // Wechselt zurück zum Heimnetzwerk-Modus
    wifi_connect(WIFI_HOME_SSID, WIFI_HOME_KEY, "Home WiFi"); // Verbindet mit dem Heim-WiFi
    energyDisplay.start(); // Startet die Display-Anzeige
}




void handleHomeNetworkMode() {
    if (connectedToHomeNetwork && (WiFi.status() == WL_CONNECTED)) {
        homeNetworkNotReachableCount = 0; // Zurücksetzen des Zählers für Nichterreichbarkeit
        webServerManager.handleClient(); // Bearbeitung eingehender Webserver-Anfragen
        if (newInverterDataAvailable){
            mqttManager->publishAllData(); // Versenden der neuen Inverter-Daten per MQTT
            newInverterDataAvailable = false; // Zurücksetzen des Flags für neue Daten
        }
    } 
    // Überprüfung, ob in einen anderen Modus gewechselt werden soll
    if (cndHomeNetworkToAPNetwork()) {
        currentState = AP_MODE; // Wechsel in den AP-Modus, falls Heimnetzwerk nicht gefunden
        activateAPMode();
    } else if (cndHomeNetworkToInverterNetwork()) {
        wifi_connect(WIFI_INVERTER_SSID, WIFI_INVERTER_KEY, "Inverter WiFi"); // Verbinden mit dem Wechselrichter-WiFi
        currentState = INVERTER_NETWORK_MODE;
    }
}



void handleAPMode() {
    webServerManager.handleClient(); // Bearbeitung eingehender Webserver-Anfragen
    // Anzeige der Zugangsdaten auf dem Display
    action.name = "AP Online";
    action.details = "Verbinden unter";
    action.params[0] = "SSID: " + String(WIFI_AP_NAME);
    String ip_string = WiFi.softAPIP().toString();
    action.params[1] = "PSWD: " + String(WIFI_AP_PASSWORD);
    action.params[2] = "IP:   " + ip_string;
    displayManager.displayAction(action); // Anzeigen der Aktion auf dem Display

    if (cndAPToHomeNetwork()) {
        deactivateAPMode(); // Deaktivieren des AP-Modus
        activatedAP = false;
        action.name = "AP Stop";
        action.details = "Beendet";
        displayManager.displayAction(action); // Aktualisierung der Anzeige mit dem aktuellen Zustand

        currentState = HOME_NETWORK_MODE; // Wechsel in den Heimnetzwerk-Modus
        wifi_connect(WIFI_HOME_SSID, WIFI_HOME_KEY, "Home WiFi"); // Aktivieren des Heimnetzwerks
        energyDisplay.start(); // Starten der Display-Aktualisierungen
    }
}




    // Display Initialization
    action.name = "Home Network";
    action.details = "WLAN";
    action.params[0] = "SSID: " + ssid;
    action.params[1] = "IP:   Warte...";
    action.params[2] = "";
    action.params[3] = "";
    action.result = "Verbinde";
    action.resultDetails = "";
    displayManager.displayAction(action);


