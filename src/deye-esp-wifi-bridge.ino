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


// Configuration and user settings
#include arduino_secrets.h

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
String status_page_url = "status.html"


///////////////////////////////////////////////////////////////////////
// Global variables
float energy_today_kWh = 0;
float energy_total_kWh = 0;
float power_actual_W = 0;

////////////////////////////////////////////////////////////////////
// Intializations 

// Initialize the display
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Initialize the MQTT client
WiFiClient espClient;
PubSubClient mqtt_client(espClient);


////////////////////////////////////////////////////////////////////
// Function declarations
void mqtt_submit_data();
void mqtt_reconnect();

void log_status();

void wifi_connect(String ssid, String passkey, String comment);

void display_status();

void web_getDataFromWeb(String url, String web_user, String web_password);
void web_parseResponse(String response);



////////////////////////////////////////////////////////////////////
// SETUP Function

void setup() {
  Serial.begin(115200);
  delay(10);

  

  // Connect to MQTT broker
  mqtt_client.setServer(MQTT_BROKER_HOST.c_str(),MQTT_BROKER_PORT);
}


////////////////////////////////////////////////////////////////////
// MAIN LOOP

void loop() {
  
    wifi_connect(WIFI_INVERTER_SSID, WIFI_INVERTER_KEY, "Inverter Network");
    
    web_getDataFromWeb(status_page_url, INVERTER_WEBACCESS_USER, INVERTER_WEBACCESS_PWD)



    wifi_connect(WIFI_HOME_SSID, WIFI_HOME_KEY, "Home Network");

    mqtt_submit_data();


delay(5000); // Adjust the publishing interval as needed
}

////////////////////////////////////////////////////////////////////
// WiFi Functions

void wifi_connect(String ssid, String passkey, String comment){

// Connect to Wi-Fi
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, passkey);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

}


////////////////////////////////////////////////////////////////////
// Web Parsing Section
void web_getDataFromWeb(String url, String web_user, String web_password){

    // 1 Login and get Status Page

    // 2 get Response

}

void web_parseResponse(String response){

}


////////////////////////////////////////////////////////////////////
// LOGGING SECTION

void log_status(){

}

////////////////////////////////////////////////////////////////////
// DISPLAY Section

void display_status(){

}


////////////////////////////////////////////////////////////////////
// MQTT SECTION
void mqtt_submit_data(){
    if (!mqtt_client.connected()) {
        mqtt_reconnect();
    }
  
    mqtt_client.loop();

    // Publish data to a topic
    String data = "Hello from bridge";
    mqtt_client.publish("MQTT_BROKER_MAINTOPIC", data.c_str());
    String data = power_actual_W.toString();
    mqtt_client.publish("MQTT_BROKER_MAINTOPIC/power", data.c_str());


    // Global variables
    energy_today_kWh
    energy_total_kWh
    power_actual_W

    mqtt_client.disconnect();
}

void mqtt_reconnect() {
  while (!mqtt_client.connected()) {
    Serial.print("Attempting MQTT connection...");
    
    if (mqtt_client.connect("deye-esp-solar-bridge", MQTT_BROKER_USER, MQTT_BROKER_PWD)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqtt_client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}