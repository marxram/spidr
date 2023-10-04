#include <Arduino.h>

// ESP8266WiFi Built-In by Ivan Grokhotkov Version 1.0.0
#include <ESP8266WiFi.h> 

// Extra library: PubSubClient by Nick O'Leary <nick.oleary@gmail.com>  V2.8.0
#include <PubSubClient.h>

// DISPLAY
//
// Adafruit SSD1306 by Adafruit V2.5.7
// Adafruit "GFX Library" by Adafruit Version 1.11.8 INSTALLED
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Web and WiFi
#include <WiFiClient.h>
#include <base64.h>

// Inverter Data handling 
#include "Inverter.h"


///////////////////////////////////////////////////////////////////////
// Configuration and user settings
#include "arduino_secrets.h"


///////////////////////////////////////////////////////////////////////
// HARDWARE SPECIFIC ADAPTIONS 

// DISPLAY ------------------------------------------------------------
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

///////////////////////////////////////////////////////////////////////
// User Secrets imported
String WIFI_HOME_SSID = SECRET_WIFI_HOME_SSID;
String WIFI_HOME_KEY = SECRET_WIFI_HOME_KEY;

String WIFI_INVERTER_SSID = SECRET_WIFI_INVERTER_SSID;
String WIFI_INVERTER_KEY = SECRET_WIFI_INVERTER_KEY;

String WIFI_RELAIS_SSID = SECRET_WIFI_RELAIS_SSID;
String WIFI_RELAIS_KEY = SECRET_WIFI_RELAIS_KEY;

String MQTT_BROKER_HOST = SECRET_MQTT_BROKER_HOST;
int    MQTT_BROKER_PORT = SECRET_MQTT_BROKER_PORT;
String MQTT_BROKER_USER = SECRET_MQTT_BROKER_USER;
String MQTT_BROKER_PWD = SECRET_MQTT_BROKER_PWD;
String MQTT_BROKER_MAINTOPIC = SECRET_MQTT_BROKER_MAINTOPIC;

String INVERTER_WEBACCESS_USER = SECRET_INVERTER_WEBACCESS_USER;
String INVERTER_WEBACCESS_PWD = SECRET_INVERTER_WEBACCESS_PWD;

///////////////////////////////////////////////////////////////////////
// Other rather static parameters
String status_page_url = "status.html" ;


///////////////////////////////////////////////////////////////////////
// Global variables
float energy_today_kWh = -1.0 ;
float energy_total_kWh = -1.0;
float power_actual_W = -1.0;

////////////////////////////////////////////////////////////////////
// Intializations 

// Initialize the display
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Initialize the MQTT client
WiFiClient espClient;
PubSubClient mqtt_client(espClient);

Inverter inverter;


////////////////////////////////////////////////////////////////////
// Function declarations
void mqtt_submit_data();
void mqtt_reconnect();

void log_status();

void wifi_connect(String ssid, String passkey, String comment);

void web_getDataFromWeb(String url, String web_user, String web_password);

void displayInverterStatus(const Inverter& inverter);


////////////////////////////////////////////////////////////////////
// SETUP Function

void setup() {
  Serial.begin(115200);
  delay(10);

  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  Serial.println("Initialize inverter");
  inverter.printVariables();

  // Connect to MQTT broker
  mqtt_client.setServer(MQTT_BROKER_HOST.c_str(),MQTT_BROKER_PORT);
}


////////////////////////////////////////////////////////////////////
// MAIN LOOP

void loop() {
  
    wifi_connect(WIFI_INVERTER_SSID, WIFI_INVERTER_KEY, "Inverter Network");
    
    delay(1000);

    web_getDataFromWeb(status_page_url, INVERTER_WEBACCESS_USER, INVERTER_WEBACCESS_PWD);

    Serial.println("Print Inverter");
    inverter.printVariables();

    delay(1000);
    displayInverterStatus(inverter);

    // Update every 5 seconds (adjust as necessary)
    delay(5000);


    wifi_connect(WIFI_HOME_SSID, WIFI_HOME_KEY, "Home Network");

    mqtt_submit_data();


delay(5000); // Adjust the publishing interval as needed
}

////////////////////////////////////////////////////////////////////
// WiFi Functions

void wifi_connect(String ssid, String passkey, String comment){

WiFi.disconnect();
  delay(1000);
  // Connect to Wi-Fi
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.println(F("Connecting to"));
  display.println(ssid);
  display.display();
  
  
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, passkey);

 
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) { // 10 sec timeout
    delay(500);
    display.print(F("."));
    display.display();

    Serial.print(".");
    
    attempts++;
  }

  display.clearDisplay();
  display.setCursor(0,0);
  if(WiFi.status() == WL_CONNECTED) {
    display.println(F("Connected!"));
    display.print(F("IP: "));
    display.println(WiFi.localIP());

    Serial.println("");
    Serial.println("WiFi connected");


  } else {
    display.println(F("Failed to connect"));
    display.println(F("to WiFi. Check"));
    display.println(F("credentials"));
  }
  display.display();
  delay(2000);

}


////////////////////////////////////////////////////////////////////
// Web Parsing Section
void web_getDataFromWeb(String url, String web_user, String web_password){
  
  String serverIp = WiFi.gatewayIP().toString();
  String website = "http://" + serverIp + "/" + url;

  Serial.print("url: ");
  Serial.println(website);

  // Create an instance of WiFiClient
  WiFiClient client;

  // Connect to the server
  if (client.connect(serverIp.c_str(), 80)) {
    // Prepare the HTTP request headers including the Authorization header
    
    Serial.println("Client connected: trying Basic Auth");
    String authHeaderValue = "Basic " + base64::encode(web_user + ":" + web_password);

    client.println("GET /" + url + " HTTP/1.1");
    client.println("Host: " + serverIp);
    client.println("Authorization: " + authHeaderValue);
    client.println("Connection: close");
    client.println();

    // Wait for the server's response
    String response = "";
    String line = "";
    while (client.connected()) {
      while (client.available()) {
        char c = client.read();
            if (c == '\n') {
                // Check if line starts with "var " and append it to the response string
                if (line.startsWith("var ")) {
                    response += line + "\n";
                }
                // Clear the line string for the next line
                line = "";
            } else if (c != '\r') {
                line += c;
            }
      }
    }

    // Print the entire response
    Serial.print(response);
    inverter.updateData(response);

    client.stop();

    // updating variables  
    power_actual_W = inverter.getWebdataNowP();
    energy_today_kWh = inverter.getWebdataTodayE();
    energy_total_kWh = inverter.getWebdataTotalE();

    Serial.printf("Power now: %f", power_actual_W);
    Serial.printf("Energy today: %f", energy_today_kWh);
    Serial.printf("Energy total: %f", energy_total_kWh);

    
  } else {
    Serial.println("Failed to connect to server.");
  }

}


////////////////////////////////////////////////////////////////////
// LOGGING SECTION

void log_status(){

}

////////////////////////////////////////////////////////////////////
// DISPLAY Section

void displayInverterStatus(const Inverter& inverter) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  
  // Display Power
  display.setCursor(0,0);
  display.print("Power: ");
  display.print(inverter.getWebdataNowP(), 2);
  display.println(" W");

  // Display Energy Today
  display.print("Energy Today: ");
  display.print(inverter.getWebdataTodayE(), 2);
  display.println(" kWh");

  // Display Total Energy
  display.print("Total Energy: ");
  display.print(inverter.getWebdataTotalE(), 2);
  display.println(" kWh");

  display.display();
}

void display_invert_blink(int times, int delay_ms){
    for (int i =0; i < times ; i++){
        display.invertDisplay(true);
        display.display();
        delay(delay_ms);
        display.invertDisplay(false);
        display.display();
    }

}



////////////////////////////////////////////////////////////////////
// MQTT SECTION
void mqtt_submit_data(){
    if (!mqtt_client.connected()) {
        mqtt_reconnect();
    }
  
    Serial.println("Connected to MQTT server");
    mqtt_client.loop();

    // Publish data to a topic
    String data = "";
        
    data = power_actual_W;
    String topic = MQTT_BROKER_MAINTOPIC + "/power";
    mqtt_client.publish(topic.c_str() , data.c_str());

  	Serial.println("> MQTT data published");

    // Global variables
    //energy_today_kWh
    //energy_total_kWh
    //power_actual_W

    mqtt_client.disconnect();
    Serial.println("> MQTT CLOSED");
}

void mqtt_reconnect() {
  while (!mqtt_client.connected()) {
    Serial.print("Attempting MQTT connection...");
    
    if (mqtt_client.connect("deye-esp-solar-bridge", MQTT_BROKER_USER.c_str(), MQTT_BROKER_PWD.c_str())) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqtt_client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}
