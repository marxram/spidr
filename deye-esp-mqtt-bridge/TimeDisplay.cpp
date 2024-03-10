#include "TimeDisplay.h"

TimeDisplay::TimeDisplay(DisplayManager& displayManager, bool& timeSynced, unsigned long& lastSyncTime) : _displayManager(displayManager), _isActive(false), _lastUpdateTime(0), _updateInterval(300), _timeSynced(timeSynced), _lastSyncTime(lastSyncTime) {}

void TimeDisplay::start(int displayDurationSeconds) {
    _isActive = true;
    _updateInterval = displayDurationSeconds * 1000; // Convert seconds to milliseconds
    _lastUpdateTime = millis() - _updateInterval; // Force immediate update on start
}

void TimeDisplay::stop() {
    _isActive = false;
}

void TimeDisplay::updateDisplay() {
    if (!_isActive) return;

    unsigned long currentTime = millis();
    if (currentTime - _lastUpdateTime >= _updateInterval) {
        displayCurrentTime();
        _lastUpdateTime = currentTime;
    }
}

void TimeDisplay::displayCurrentTime() {
    ActionData action; // Assuming ActionData is properly defined elsewhere

    action.name     =  "Current Time";
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
    action.result = _timeSynced ? "In Sync" : "Out of Sync";    
    _displayManager.displayAction(action); // Update the display with the current time and sync status
}
