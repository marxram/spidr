// DisplayManager.h

#ifndef DISPLAYMANAGER_H
#define DISPLAYMANAGER_H

#include <U8g2lib.h>
#include <Arduino.h> // For access to millis()

struct ActionData {
  String name;
  String details;
  String params[4]; // Holds up to 4 parameters, adjust size as needed
  String result;
  String resultDetails;
};

class DisplayManager {
public:
    DisplayManager();
    void init();
    void update(); // Called regularly to manage screen changes
    void showScreen1();
    void showScreen2();
    void showScreen3();
    void showScreen4();
    void displayAction(const ActionData& action);
    void drawBigNumberWithUnit(float number, String unit, String annotation, String formattingStr, const uint8_t *numberFont, const uint8_t *unitFont);
    float myNumber;
    String myUnit;
    String myFormattingStr;


private:
    U8G2 *u8g2; // Use a pointer to the base class
    unsigned long lastUpdateTime = 0; // Last update time in millis
    const unsigned long screenInterval = 3000; // Time interval for screen update
    const unsigned int MAX_SCREENS = 4; // Maximum number of screens
    int currentScreen = 0; // Keep track of the current screen
    // Add more private members if needed, including global variables to display
};

#endif
