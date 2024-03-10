#ifndef TimeDisplay_h
#define TimeDisplay_h

#include <Arduino.h>
#include "DisplayManager.h"

class TimeDisplay {
public:
    TimeDisplay(DisplayManager& display, bool& timeSynced, unsigned long& lastSyncTime);
    void updateDisplay();
    void start(int displayDurationSeconds);
    void stop();

private:
    DisplayManager& _displayManager;
    bool _isActive;
    unsigned long _lastUpdateTime;
    unsigned long _updateInterval; // In milliseconds
    unsigned long _lastSyncTime;
    bool _timeSynced; // In milliseconds
    
    void displayCurrentTime();
};

#endif // TimeDisplay_h
