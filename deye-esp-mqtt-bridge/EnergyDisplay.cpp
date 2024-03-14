#include "EnergyDisplay.h"
#include "config.h"

EnergyDisplay::EnergyDisplay(DisplayManager& displayManager, SerialCaptureLines& serialCapture) : 
_displayManager(displayManager), _currentState(DISPLAY_INVERTER_POWER), _lastUpdateTime(0), _isActive(false), _previousState(DISPLAY_INVERTER_POWER), serialCapture(serialCapture)  {
    for (int i = 0; i < NUM_STATES; i++) {
        _intervals[i] = 0;
    }
}

void EnergyDisplay::initializeDisplayIntervals(unsigned int powerDisplayTime, unsigned int energyTodayDisplayTime, unsigned int energyTotalDisplayTime) {
    serialCapture.println(F("EnergyDisplay: Initializing display intervals."));
    _intervals[DISPLAY_INVERTER_POWER] = powerDisplayTime;
    // Maybe create own interval for the graph
    _intervals[DISPLAY_POWER_GRAPH] = powerDisplayTime;
    _intervals[DISPLAY_ENERGY_TODAY] = energyTodayDisplayTime;
    _intervals[DISPLAY_ENERGY_TOTAL] = energyTotalDisplayTime;
    
    serialCapture.print(F("Intervals set to: Power: ")); serialCapture.print(powerDisplayTime);
    serialCapture.print(F(" ms, Today: ")); serialCapture.print(energyTodayDisplayTime);
    serialCapture.print(F(" ms, Total: ")); serialCapture.println(energyTotalDisplayTime);
    
    
}

void EnergyDisplay::updateDisplay(const Inverter& inverter) {
    if (!_isActive) return;
    //serialCapture.println("[DBG] updateDisplay");

    unsigned long currentTime = millis();

    if (currentTime - _lastUpdateTime >= _intervals[_currentState]) {
        //serialCapture.print("Switching display. Current State: ");
        //serialCapture.println(_currentState);
        switch (_currentState) {
        case DISPLAY_INVERTER_POWER:
            //serialCapture.println("Displaying Inverter Power Now.");
            if (SCREEN_HEIGHT == 32) {
                _displayManager.drawBigNumberWithHeader("Power", inverter.getInverterPowerNow_W(), "W", "", "%.0f");
            } else {
                _displayManager.drawBigNumberWithHeader("Leistung aktuell", inverter.getInverterPowerNow_W(), "W", "", "%.0f");
            }
            
            break;
        case DISPLAY_ENERGY_TODAY:
            //serialCapture.println("Displaying Inverter Energy Today.");
            if (SCREEN_HEIGHT == 32) {
                _displayManager.drawBigNumberWithHeader("24h", inverter.getInverterEnergyToday_kWh(), "kWh", "", "%.1f");
            } else {
                _displayManager.drawBigNumberWithHeader("Energie heute", inverter.getInverterEnergyToday_kWh(), "kWh", "", "%.1f");
            }
            break;
        case DISPLAY_ENERGY_TOTAL:
            //serialCapture.println("Displaying Inverter Energy Total.");
            if (SCREEN_HEIGHT == 32) {
                _displayManager.drawBigNumberWithHeader("", inverter.getInverterEnergyTotal_kWh(), "kWh", "", "%.1f");
            } else {
                _displayManager.drawBigNumberWithHeader("Energie Gesamt", inverter.getInverterEnergyTotal_kWh(), "kWh", "", "%.1f");
            }
            break;
        case DISPLAY_POWER_GRAPH:
                _displayManager.drawGraph(inverter.getPowerData(), inverter.getPowerDataSize());
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
    serialCapture.println("EnergyDisplay started.");
}

void EnergyDisplay::stop() {
    _isActive = false;
    serialCapture.println("EnergyDisplay stopped.");
}

