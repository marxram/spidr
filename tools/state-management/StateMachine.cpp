#include "StateMachine.h"
#include "DisplayManager.h"

StateMachine::StateMachine(DisplayManager& display, ActionData& action) : displayManager(display), currentState(HOME_NETWORK_MODE) {
    this->action = action;
    lastStateChangeMillis = millis();
    lastInverterUpdateMillis = millis();
}

void StateMachine::update() {
    // Display the current state on the screen using the DisplayManager
    
    action.name = "Current State";
    switch (currentState) {
        case INVERTER_NETWORK_MODE:
            action.details = "Inverter Network Mode";
            handleInverterNetworkMode();
            break;
        case HOME_NETWORK_MODE:
            action.details = "Home Network Mode";
            handleHomeNetworkMode();
            break;
        case AP_MODE:
            action.details = "AP Mode";
            handleAPMode();
            break;
    }
    displayManager.displayAction(action); // Update the display with the current state
}

void StateMachine::handleInverterNetworkMode() {
    Serial.println("Entering Inverter Network Mode");
    connectedToHomeNetwork = false;
    connectedToInverterNetwork = true;

    Serial.println("Exiting Inverter Network Mode");
    currentState = HOME_NETWORK_MODE; // Transition to the next mode
    lastStateChangeMillis = millis();


    action.result = "Working";
    displayManager.displayAction(action); // Update the display with the current state
    delay(1000); // Dummy delay to simulate network activity
    action.result = "Done";
    
    lastInverterUpdateMillis = millis();

    displayManager.displayAction(action); // Update the display with the current state
    delay(1000); // Dummy delay to simulate network activity


    action.result = "Switch";
    action.resultDetails = "-> HOME";
    displayManager.displayAction(action); // Update the display with the current state
    delay(2000); 
}

void StateMachine::handleHomeNetworkMode() {
    Serial.println("In Home Network Mode");
    connectedToHomeNetwork = true;

    if (cndHomeNetworkToAPNetwork()) {
        Serial.println("Switching to AP Mode due to timeout.");
        
        action.result = "Switch";
        action.resultDetails = "-> AP";
        displayManager.displayAction(action); // Update the display with the current state
        delay(2000); 
        
        currentState = AP_MODE;
        lastStateChangeMillis = millis();
    } else if (cndHomeNetworkToInverterNetwork()) {
        Serial.println("Switching to Inverter Network Mode for data update.");
        
        action.result = "Switch";
        action.resultDetails = "-> INV";
        displayManager.displayAction(action); // Update the display with the current state
        delay(2000); 

        currentState = INVERTER_NETWORK_MODE;
        lastStateChangeMillis = millis();
    }

    action.result = "Working";
    displayManager.displayAction(action); // Update the display with the current state
    delay(1000); // Dummy delay to simulate network activity
    

}

void StateMachine::handleAPMode() {
    Serial.println("In AP Mode");
    connectedToHomeNetwork = false;
    connectedToInverterNetwork = false;
    activatedAP = true;

    if (cndAPToHomeNetwork()) {
        Serial.println("No client connected. Switching back to Home Network Mode.");
        
        action.result = "Switch";
        action.resultDetails = "-> HOME";
        displayManager.displayAction(action); // Update the display with the current state
        delay(2000); 
        
        currentState = HOME_NETWORK_MODE;
        lastStateChangeMillis = millis();
    }
}

// Implementation of condition checking functions
bool StateMachine::cndHomeNetworkToAPNetwork() {
    return (millis() - lastStateChangeMillis > HOME_NETWORK_TIMEOUT);
}

bool StateMachine::cndHomeNetworkToInverterNetwork() {
    return (millis() - lastInverterUpdateMillis > INVERTER_UPDATE_INTERVAL);
}

bool StateMachine::cndAPToHomeNetwork() {
    return (!hasClientConnected() && millis() - lastStateChangeMillis > AP_MODE_TIMEOUT);
}

bool StateMachine::hasClientConnected() {
    // ToDo: Implement the logic to check if a client is connected to the AP
    return false;
}
