// DisplayManager.cpp

#include "DisplayManager.h"
#include "config.h"

// Big Number Font Settings
const uint8_t *bigNumberHeadlineFont = u8g2_font_luRS10_tr;
const uint8_t *bigNumberAnnotationFont = u8g2_font_luRS10_tr;
const uint8_t *numberFont = u8g2_font_logisoso32_tr;
const uint8_t *unitFont = u8g2_font_luRS19_tr;
const uint8_t *smallNumberFont = u8g2_font_logisoso24_tr;
const uint8_t *smallUnitFont = u8g2_font_luRS12_tr;

// Show Actions Font Settings
const uint8_t *displayActionHeadlineFont = u8g2_font_luRS14_tr;


DisplayManager::DisplayManager() {
    // Conditional initialization based on board type
#ifdef BOARD_WEMOS_OLED_128x64_ESP32
    u8g2 = new U8G2_SSD1306_128X64_NONAME_F_SW_I2C(U8G2_R0, /* clock=*/ 4, /* data=*/ 5, /* reset=*/ U8X8_PIN_NONE);
#elif defined(BOARD_HELTEC_OLED_128x32_ESP8266)
    u8g2 = new U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C(U8G2_R0, /* reset=*/ U8X8_PIN_NONE, /* clock=*/ SCL, /* data=*/ SDA);
#else
    u8g2 = new U8G2_SSD1306_128X64_NONAME_F_SW_I2C(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);
#endif
}

void DisplayManager::setI2CAddress(uint8_t adr) {
    u8g2->setI2CAddress(adr); 
}

void DisplayManager::init() {
    u8g2->begin();
}


void DisplayManager::displayAction(const ActionData& action) {
  int yPositionBottomLine = 63;
  String actionStr(action.name.c_str());
  
  //Serial.println("DisplayAction: " + actionStr);
  
  u8g2->clearBuffer();

  //Serial.println("Displaying action name");
  // Center and display the action name in the first row
  u8g2->setFont(displayActionHeadlineFont); // Assuming the font is okay
  int actionNameWidth = u8g2->getStrWidth(action.name.c_str());
  int startX = (SCREEN_WIDTH - actionNameWidth) / 2;
  u8g2->drawStr(startX, 14, action.name.c_str());

  if (verboseDisplay) {
    //Serial.println("Displaying verbose details");
    // Display the details below the name
    u8g2->setFont(u8g2_font_6x10_tf);
    u8g2->drawStr(0, 24, action.details.c_str());

    // Display parameters
    for (int i = 0; i < 3 && i < sizeof(action.params) / sizeof(action.params[0]); ++i) {
      if (action.params[i].length() > 0) {
        u8g2->drawStr(0, 33 + (i * 9), action.params[i].c_str());
      }
    }
  } else {
    //Serial.println("Displaying normal view");
    // Normal View
    u8g2->setFont(u8g2_font_6x10_tf);
    u8g2->drawStr(0, 30, action.details.c_str());
    yPositionBottomLine = 50;
  }

  //Serial.println("Displaying result with background padding");
  // Display result with background padding
  u8g2->setFont(u8g2_font_5x8_tf); // Using a small font

  // Calculate text width for the result and add padding
  int resultWidth = u8g2->getStrWidth(action.result.c_str()) + 8; // 4 pixels padding on each side
  int resultStartX = 0; // Starting X position for result box

  // Draw the background box for the result
  u8g2->setDrawColor(1); // Set color for background
  u8g2->drawBox(resultStartX, yPositionBottomLine - 8, resultWidth, 10); // Height is approximate, adjust as needed

  // Draw the result text in inverted color
  u8g2->setDrawColor(0); // Set color for text
  u8g2->drawStr(resultStartX + 4, yPositionBottomLine, action.result.c_str());

  // Reset draw color for next elements
  u8g2->setDrawColor(1);

  //Serial.println("Displaying result details aligned to the right");
  // Calculate text width for the details to align to the right
  int detailsWidth = u8g2->getStrWidth(action.resultDetails.c_str());
  int detailsStartX = SCREEN_WIDTH - detailsWidth; // Calculate X position for details to align right

  // Display result details without inverted color, aligned to the right
  u8g2->drawStr(detailsStartX, yPositionBottomLine, action.resultDetails.c_str());

  //Serial.println("Sending buffer to display");
  u8g2->sendBuffer();
}



void DisplayManager::drawBigNumberNoHeader(float number, String unit, String annotation, String formattingStr) {
    char numberBuffer[20]; // Buffer to hold the formatted number as a string

    // Format the floating-point number according to the provided formatting string
    snprintf(numberBuffer, sizeof(numberBuffer), formattingStr.c_str(), number);

    // Determine the fonts to use for this invocation
    const uint8_t * currentNumberFont = numberFont; // Assuming default larger font
    const uint8_t* currentUnitFont = unitFont; // Assuming default larger font

    // Initially set the font for the number and calculate its width
    u8g2->setFont(currentNumberFont);
    int numberWidth = u8g2->getStrWidth(numberBuffer);

    // Set the font for the unit and calculate its width
    u8g2->setFont(currentUnitFont);
    int unitWidth = u8g2->getStrWidth(unit.c_str());

    // Check if the total width exceeds the display width and adjust fonts if necessary
    if ((numberWidth + unitWidth + 4) > 128) {
        Serial.println("Switching to smaller fonts");
        currentNumberFont = smallNumberFont; // Switch to smaller font
        currentUnitFont = smallUnitFont; // Switch to smaller font

        // Recalculate widths with smaller fonts
        u8g2->setFont(currentNumberFont);
        numberWidth = u8g2->getStrWidth(numberBuffer);

        u8g2->setFont(currentUnitFont);
        unitWidth = u8g2->getStrWidth(unit.c_str());
    }

    // Calculate positions with potentially updated widths
    int unitX = 128 - unitWidth; // Position unit at the right edge
    int unitY = 63; // Vertical position for the unit, adjust as needed
    int numberX = unitX - numberWidth - 4; // Position number to the left of the unit, 4 pixels apart
    int numberY = unitY; // Align the baseline of the number with the unit

    // Draw the annotation with a predefined font
    u8g2->setFont(bigNumberAnnotationFont);
    Serial.println(annotation);
    u8g2->drawStr(0, 28, annotation.c_str());
    
    // Draw the unit with its (possibly updated) font
    u8g2->setFont(currentUnitFont);
    u8g2->drawStr(unitX, unitY, unit.c_str());

    // Draw the number with its (possibly updated) font
    u8g2->setFont(currentNumberFont);
    u8g2->drawStr(numberX, numberY, numberBuffer);

    // Send the buffer to the display
    u8g2->sendBuffer();
}



void DisplayManager::drawBigNumberWithHeader(String header, float number, String unit, String annotation, String formattingStr) {
  u8g2->clearBuffer();

  // Center and display the action name in the first row
  u8g2->setFont(bigNumberHeadlineFont); // Choose a suitable font for the action name
  int actionNameWidth = u8g2->getStrWidth(header.c_str()); // Get the width of the action name
  //int startX = (SCREEN_WIDTH - actionNameWidth) / 2; // Calculate the starting X to center the text
  u8g2->drawStr(0, 13, header.c_str()); // Draw the action name centered

  drawBigNumberNoHeader(number, unit, annotation, formattingStr);
  u8g2->sendBuffer();
}
