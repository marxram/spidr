#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include "DisplayManager.h" // Include the DisplayManager header

// Enum to define different states
enum State {
    INVERTER_NETWORK_MODE,
    HOME_NETWORK_MODE,
    AP_MODE
};

class StateMachine {
public:
    StateMachine(DisplayManager& display, ActionData& action); // Constructor to initialize with DisplayManager
    void update(); // Method to update state machine

private:
    DisplayManager& displayManager; // Reference to DisplayManager for UI updates
    ActionData& action; // Reference to Action structure for display
    State currentState; // Current state of the state machine


    // Constants for timeout intervals
    static const unsigned long HOME_NETWORK_TIMEOUT = 15000;
    static const unsigned long INVERTER_UPDATE_INTERVAL = 15000;
    static const unsigned long AP_MODE_TIMEOUT = 15000;

    // Flags for network status
    bool connectedToHomeNetwork = false;
    bool connectedToInverterNetwork = false ;
    bool activatedAP = false;


};

#endif /* STATEMACHINE_H */
