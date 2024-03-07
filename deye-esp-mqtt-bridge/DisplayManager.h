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
    void drawBigNumberNoHeader(float number, String unit, String annotation, String formattingStr);
    void drawBigNumberWithHeader(String header, float number, String unit, String annotation, String formattingStr);
    void setI2CAddress(uint8_t adr);
    bool verboseDisplay = false;


private:
    U8G2 *u8g2; // Use a pointer to the base class
    const unsigned int SCREEN_WIDTH = 128;
    unsigned long lastUpdateTime = 0; // Last update time in millis
    const unsigned long screenInterval = 3000; // Time interval for screen update
    const unsigned int MAX_SCREENS = 4; // Maximum number of screens
    int currentScreen = 0; // Keep track of the current screen
    // Add more private members if needed, including global variables to display
};

#endif
