// YourMainSketch.ino

#include "StateMachine.h"
#include "DisplayManager.h"

#define BOARD_WEMOS_OLED_128x64_ESP32

DisplayManager displayManager;
ActionData action; // Action Structure to Display
StateMachine stateMachine(displayManager, action); // Initialize state machine with display manager

void setup() {
    Serial.begin(115200);
    // Other setup code like initializing WiFi, MQTT, etc.
    Serial.println("Initialize Display Manager...");
   
    displayManager.init();
    // Show also more output and Parameters like Url, IP etc. 
    displayManager.verboseDisplay = true;

    action.name = "Action Item";
    action.details = "Action details";
    action.params[0] = "params[0]";
    action.params[1] = "params[1]";
    action.params[2] = "params[2]";
    action.params[3] = "params[3]";
    action.result = "Result";
    action.resultDetails = "Details";
    displayManager.displayAction(action);
    delay(2000);
    action.params[0] = "";
    action.params[1] = "";
    action.params[2] = "";
    action.params[3] = "";
    action.result = "";
    action.resultDetails = "";
    displayManager.displayAction(action);
}

void loop() {
    stateMachine.update();
    // Any other loop code, if needed
    delay(50); // Dummy delay to slow down the loop for demonstration purposes
}