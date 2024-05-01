#include "EnergyDisplay.h"
#include "config.h"

EnergyDisplay::EnergyDisplay(DisplayManager& displayManager, bool& timeSynced, unsigned long& lastSyncTime, SerialCaptureLines& serialCapture) : 
_displayManager(displayManager), _currentState(DISPLAY_INVERTER_POWER), _lastUpdateTime(0), _isActive(false), _previousState(DISPLAY_INVERTER_POWER), serialCapture(serialCapture), _timeSynced(timeSynced), _lastSyncTime(lastSyncTime) {
    for (int i = 0; i < NUM_STATES; i++) {
        _intervals[i] = 0;
    }

}

void EnergyDisplay::initializeDisplayIntervals(unsigned int powerDisplayTime, unsigned int energyTodayDisplayTime, unsigned int energyTotalDisplayTime, unsigned int energyGraphTime, unsigned int timeDisplayTime) {
    serialCapture.println(F("EnergyDisplay: Initializing display intervals."));
    
    _intervals[DISPLAY_INVERTER_POWER] = powerDisplayTime;
    _intervals[DISPLAY_POWER_GRAPH] = energyGraphTime;
    _intervals[DISPLAY_ENERGY_TODAY] = energyTodayDisplayTime;
    _intervals[DISPLAY_ENERGY_TOTAL] = energyTotalDisplayTime;
    _intervals[DISPLAY_TIME] = timeDisplayTime;
    
    serialCapture.print(F("Intervals set to: Power: ")); serialCapture.print(powerDisplayTime);
    serialCapture.print(F(" ms, Today: ")); serialCapture.print(energyTodayDisplayTime);
    serialCapture.print(F(" ms, Total: ")); serialCapture.print(energyTotalDisplayTime);
    serialCapture.print(F(" ms, Graph: ")); serialCapture.print(energyGraphTime);
    serialCapture.print(F(" ms, Time: ")); serialCapture.println(timeDisplayTime);
    
}

void EnergyDisplay::updateDisplay(const Inverter& inverter) {
    if (!_isActive) return;
    //serialCapture.println("[DBG] updateDisplay");

    unsigned long currentTime = millis();

    if (currentTime - _lastUpdateTime >= _intervals[(_currentState + NUM_STATES -1) % NUM_STATES]) {
        switch (_currentState) {
        // Only Triggerin on changes from one state to the other.
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
    // Weired Logic Ahaed Unfortunatly needs to be like this ;-) 
    if (_currentState == 0 ) {
        displayCurrentTime();
        //serialCapture.print("TIME DISPLAY ");
        //serialCapture.println(_currentState);
        //_lastUpdateTime = currentTime;
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

void EnergyDisplay::displayCurrentTime() {
    ActionData action; // Assuming ActionData is properly defined elsewhere

    action.name     =  "Aktuelle Zeit";
    action.details  = "";

    time_t now = time(nullptr); // Get the current time
    struct tm *timeinfo = localtime(&now); // Convert to local time structure

    char dateStr[24], timeStr[24], syncStr[24];
    strftime(dateStr, sizeof(dateStr), "Date:   %d.%m.%Y", timeinfo);
    strftime(timeStr, sizeof(timeStr), "Time:   %H:%M:%S", timeinfo);
    
    long syncInterval = (millis() - _lastSyncTime) / 1000; // Convert milliseconds to seconds
    int hours = syncInterval / 3600; // Calculate total hours
    int minutes = (syncInterval % 3600) / 60; // Calculate remaining minutes
    int seconds = syncInterval % 60; // Calculate remaining seconds

    if (hours > 0) {
        // If there are hours, include them in the string
        sprintf(syncStr, "Last:   %dh %dm %2ds", hours, minutes, seconds);
    } else if (minutes > 0) {
        // If there are no hours but there are minutes, only include minutes and seconds
        sprintf(syncStr, "Last:   %dm %ds", minutes, seconds);
    } else {
        // If there are only seconds, just include seconds
        sprintf(syncStr, "Last:   %2ds", seconds);
    }

    // Assume action is a global or properly passed to this function
    action.params[0] = dateStr;
    action.params[1] = timeStr;
    action.params[2] = syncStr;
    action.result = _timeSynced ? "Synchron" : "Nicht Synchron";    
    _displayManager.displayAction(action); // Update the display with the current time and sync status
}

