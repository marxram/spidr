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
  displayManager.init();
  // Initialize random seed
  srand(time(NULL));

}

void loop() {
  updateValues();
  displayManager.update();
  delay(1000);

    delay(2000);
    action.name = "Home WiFi";
    action.details = "Connect to WiFi";
    action.params[0] = "SSID: marxram-ng";
    action.params[1] = "IP: Waiting...";
    action.result = "In Progress";
    action.resultDetails = "";
    displayManager.displayAction(action);

    delay(2000);

    // Later, once more information is available or the action completes:
    action.params[1] = "IP: 10.1.1.20";
    action.result = "Success";
    action.resultDetails = "Connected";
    displayManager.displayAction(action);


    delay(3000);


}





