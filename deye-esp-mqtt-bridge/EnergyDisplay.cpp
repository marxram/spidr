#include "EnergyDisplay.h"

EnergyDisplay::EnergyDisplay(DisplayManager& displayManager) : 
_displayManager(displayManager), _currentState(DISPLAY_INVERTER_POWER), _lastUpdateTime(0), _isActive(false), _previousState(DISPLAY_INVERTER_POWER) {
    for (int i = 0; i < NUM_STATES; i++) {
        _intervals[i] = 0;
    }
}

void EnergyDisplay::initializeDisplayIntervals(unsigned int powerDisplayTime, unsigned int energyTodayDisplayTime, unsigned int energyTotalDisplayTime) {
    Serial.println(F("EnergyDisplay: Initializing display intervals."));
    _intervals[DISPLAY_INVERTER_POWER] = powerDisplayTime;
    _intervals[DISPLAY_ENERGY_TODAY] = energyTodayDisplayTime;
    _intervals[DISPLAY_ENERGY_TOTAL] = energyTotalDisplayTime;
    Serial.print(F("Intervals set to: Power: ")); Serial.print(powerDisplayTime);
    Serial.print(F(" ms, Today: ")); Serial.print(energyTodayDisplayTime);
    Serial.print(F(" ms, Total: ")); Serial.println(energyTotalDisplayTime);
    
    
}

void EnergyDisplay::updateDisplay(const Inverter& inverter) {
    if (!_isActive) return;
    //Serial.println("[DBG] updateDisplay");

    unsigned long currentTime = millis();

    if (currentTime - _lastUpdateTime >= _intervals[_currentState]) {
        //Serial.print("Switching display. Current State: ");
        //Serial.println(_currentState);
        switch (_currentState) {
        case DISPLAY_INVERTER_POWER:
            Serial.println("Displaying Inverter Power Now.");
            _displayManager.drawBigNumberWithHeader("Leistung aktuell", inverter.getInverterPowerNow_W(), "W", "", "%.0f");
            break;
        case DISPLAY_ENERGY_TODAY:
            Serial.println("Displaying Inverter Energy Today.");
            _displayManager.drawBigNumberWithHeader("Energie heute", inverter.getInverterEnergyToday_kWh(), "kWh", "", "%.1f");
            break;
        case DISPLAY_ENERGY_TOTAL:
            Serial.println("Displaying Inverter Energy Total.");
            _displayManager.drawBigNumberWithHeader("Energie gesamt", inverter.getInverterEnergyTotal_kWh(), "kWh", "", "%.1f");
            break;
        }
        
        // Switch to next state
        _currentState = static_cast<DisplayState>((_currentState + 1) % NUM_STATES);

        _previousState = _currentState ;
        _lastUpdateTime = currentTime;
    }
}


void EnergyDisplay::start() {
    _isActive = true;
    _lastUpdateTime = millis(); // Reset the timer to allow immediate switch
    Serial.println("EnergyDisplay started.");
}

void EnergyDisplay::stop() {
    _isActive = false;
    Serial.println("EnergyDisplay stopped.");
}