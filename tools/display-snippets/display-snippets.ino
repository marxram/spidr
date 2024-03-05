#include <U8g2lib.h>

#include <Arduino.h>

#include "DisplayManager.h"

#include <cstdlib> // For rand() and srand()
#include <ctime> // For time()



//#define BOARD_WEMOS_OLED_128x64_ESP32
//#define BOARD_HELTEC_OLED_128x32_ESP8266

  



//https://github.com/olikraus/u8g2/wiki/fntgrpstreamline


DisplayManager displayManager;


float generateRandomNumber() {
  // rand() generates a random number between 0 and RAND_MAX
  float randomFloat = static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 2999.0));
  return randomFloat + 1.0;
}

String getRandomUnit() {
  // Array of units
  String units[] = {"W", "Wh", "kWh"};
  
  // Get the size of the units array
  int numUnits = sizeof(units) / sizeof(units[0]);
  
  // Generate a random index between 0 and numUnits-1
  int index = rand() % numUnits;
  
  // Return the unit at the randomly selected index
  return units[index];
}

String getRandomFormatStrings() {
  // Array of units
  String formats[] = {"%.1f", "%.2f", "%.0f"};
  
  // Get the size of the units array
  int numFormats = sizeof(formats) / sizeof(formats[0]);
  
  // Generate a random index between 0 and numUnits-1
  int index = rand() % numFormats;
  
  // Return the unit at the randomly selected index
  return formats[index];
}

void updateValues(){
  displayManager.myNumber = generateRandomNumber();
  displayManager.myUnit = getRandomUnit();
  displayManager.myFormattingStr = getRandomFormatStrings();
}

ActionData action;

void setup() {
  
  pinMode(LED_BUILTIN, OUTPUT);  // Initialize the LED_BUILTIN pin as an output
  
  displayManager.init();
  // Initialize random seed
  srand(time(NULL));

}

void loop() {
  updateValues();
  displayManager.update();
  delay(1000);

    delay(2000);
    // WiFi Home
    digitalWrite(LED_BUILTIN, HIGH);
    action.name = "Home WiFi";
    action.details   = "Connect to WiFi";
    action.params[0] = "SSID:  marxram-ng";
    action.params[1] = "IP:    Waiting...";
    action.params[2] = "GW-IP: Waiting...";
    action.result = "In Progress";
    action.resultDetails = "";
    displayManager.displayAction(action);

    delay(2000);
    digitalWrite(LED_BUILTIN, LOW);
    // Later, once more information is available or the action completes:
    action.params[1] = "IP:   101.110.110.1";
    action.params[2] = "GWIP: 101.110.110.111";
    action.result = "Success";
    action.resultDetails = "Connected";
    displayManager.displayAction(action);

    // NTP
    delay(3000);
    digitalWrite(LED_BUILTIN, HIGH);
    action.name = "Time Sync ";
    action.details = "Update time via NTP";
                    // "---------------------" 
    action.params[0] = "ntp.pool.domain.1.de";
    action.params[1] = "Date: waiting...";
    action.params[2] = "Time: waiting..." ;
    action.result = "In Progress";
    action.resultDetails = "";
    displayManager.displayAction(action);

    delay(2000);
    digitalWrite(LED_BUILTIN, LOW);

    // Later, once more information is available or the action completes:
    action.params[1] = "Date:   05.03.2024";
    action.params[2] = "Time:   17:17:02" ;
    action.result = "Success";
    action.resultDetails = "in sync";
    displayManager.displayAction(action);

    // Inverter WiFi 
    delay(3000);
    digitalWrite(LED_BUILTIN, HIGH);
    action.name = "Inverter WiFi";
    action.details = "Connect to WiFi";
    action.params[0] = "SSID: AP12345678";
    action.params[1] = "IP:   Waiting...";
    action.params[2] = "Inv:  Waiting...";
    action.result = "In Progress";
    action.resultDetails = "";
    displayManager.displayAction(action);

    delay(2000);
    digitalWrite(LED_BUILTIN, LOW);

    // Later, once more information is available or the action completes:
    action.params[1] = "IP:   101.101.110.152";
    action.params[2] = "Inv:  101.101.110.152";
    action.result = "Success";
    action.resultDetails = "Connected";
    displayManager.displayAction(action);



    // Inverter Time
    delay(3000);
    digitalWrite(LED_BUILTIN, HIGH);
    action.name = "Inverter Time";
    action.params[1] = "Date:   05.03.2024";
    action.params[2] = "Time:   17:17:02" ;
    action.result = "In Progress";
    action.resultDetails = "";
    displayManager.displayAction(action);

    delay(2000);
    digitalWrite(LED_BUILTIN, LOW);

    // Later, once more information is available or the action completes:
    action.result = "Success";
    action.resultDetails = "in Sync";
    displayManager.displayAction(action);


    
    // Get Data Inv  
    delay(3000);
    digitalWrite(LED_BUILTIN, HIGH);
    action.name = "Energy Data";
    action.details = "Read Inverter";
    action.params[0] = "Login: Waiting...";
    action.params[1] = "Fetch: Waiting...";
    action.params[2] = "Parse: Waiting...";
    action.result = "In Progress";
    action.resultDetails = "";
    displayManager.displayAction(action);

    delay(2000);
    digitalWrite(LED_BUILTIN, LOW);
    // Later, once more information is available or the action completes:
    action.params[0] = "Login: Done";
    displayManager.displayAction(action);
    delay(1000);
    
    action.params[1] = "Fetch: Done";
    displayManager.displayAction(action);
    delay(100);
    
    action.params[2] = "Parse: Done";
    displayManager.displayAction(action);

    delay(1000);
    
    action.result = "Success";
    action.resultDetails = "Done";
    displayManager.displayAction(action);


    // Send MQTT
    delay(3000);
    // WiFi Home
    digitalWrite(LED_BUILTIN, HIGH);
    action.name = "MQTT Sync";
    action.details = "Sending data to server";
    action.params[0] = "Server 10.1.1.15";
    action.params[1] = "Port:  1883";
    action.params[2] = "Login: Waiting...";
    action.result = "In Progress";
    action.resultDetails = "";
    displayManager.displayAction(action);

    delay(2000);
    digitalWrite(LED_BUILTIN, LOW);
    // Later, once more information is available or the action completes:
    action.params[1] = "IP:    10.1.1.20";
    action.params[2] = "PGWIP: 10.1.1.1";
    action.params[2] = "Login: done";

    action.result = "Success";
    action.resultDetails = "20 Items";
    displayManager.displayAction(action);

    delay(2000);


}
