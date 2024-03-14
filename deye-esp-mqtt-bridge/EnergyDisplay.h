#ifndef EnergyDisplay_h
#define EnergyDisplay_h

#include <Arduino.h>
#include "Inverter.h"
#include "DisplayManager.h"
#include "SerialCaptureLines.h"

class EnergyDisplay {
public:
    EnergyDisplay(DisplayManager& displayManager, SerialCaptureLines& serialCapture);
    void initializeDisplayIntervals(unsigned int powerDisplayTime, unsigned int energyTodayDisplayTime, unsigned int energyTotalDisplayTime);
    void updateDisplay(const Inverter& inverter);
    void start(); // Start the display update cycle
    void stop();  // Stop the display update cycle

private:
    DisplayManager& _displayManager;
    enum DisplayState {
        DISPLAY_INVERTER_POWER,
        DISPLAY_ENERGY_TODAY,
        DISPLAY_ENERGY_TOTAL,
        DISPLAY_POWER_GRAPH,
        NUM_STATES
    };

    DisplayState _currentState;
    unsigned long _lastUpdateTime;
    unsigned int _intervals[NUM_STATES];
    bool _isActive; // Flag to control the update cycle
    DisplayState _previousState; // Keep track of the previous state
    SerialCaptureLines& serialCapture;
};

#endif // EnergyDisplay_h
