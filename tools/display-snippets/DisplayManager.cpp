// DisplayManager.cpp

#include "DisplayManager.h"


const uint8_t *numberFont = u8g2_font_logisoso28_tr;
const uint8_t *unitFont = u8g2_font_luRS18_tr;

const uint8_t *smallNumberFont = u8g2_font_logisoso22_tr;
const uint8_t *smallUnitFont = u8g2_font_luRS14_tr;


float myNumber = 1312.4;
String myUnit = "kWh";
String myFormattingStr = "%.0f";


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

void DisplayManager::init() {
    u8g2->begin();
}

void DisplayManager::update() {
    unsigned long currentTime = millis();
    if (currentTime - lastUpdateTime >= screenInterval) {
        lastUpdateTime = currentTime;
        currentScreen = (currentScreen + 1) % MAX_SCREENS; // Update this based on the number of screens
        
        switch (currentScreen) {
            case 0:
                showScreen1();
                break;
            case 1:
                showScreen2();
                break;
            case 2:
                showScreen3();
                break;
            case 3:
                showScreen4();
                break;
        }
    }
}

void DisplayManager::showScreen1(){
  u8g2->clearBuffer();
  
  u8g2->setFont(u8g2_font_unifont_t_emoticons);
  u8g2->drawGlyph(0, 15, 0x20); // Smile Big
  u8g2->drawGlyph(16, 15, 0x2E); // Sunglasses 
  u8g2->drawGlyph(32, 15, 0x100); 
  u8g2->drawGlyph(48, 15, 0x78); // Pommesgabel
  u8g2->drawGlyph(64, 15, 0x70); // Dollar
  u8g2->drawGlyph(80, 15, 0x100); // Sad
  u8g2->drawGlyph(96, 15, 0xB8); // House
  u8g2->drawGlyph(112, 15, 0xB4); // Server

  
  // Inside your loop or setup, call the function with the additional font arguments
  drawBigNumberNoHeader(myNumber, myUnit, "Energie heute", myFormattingStr, numberFont, unitFont);
  
  // Send the buffer to the display
  u8g2->sendBuffer();
}

void DisplayManager::showScreen2(){
  u8g2->clearBuffer();
  
  /*
  u8g2->setFont(u8g2_font_open_iconic_all_2x_t);
  u8g2->drawGlyph(0, 15, 0x7B); // Clock
  u8g2->drawGlyph(16, 15, 0xFC); // Shield
  u8g2->drawGlyph(32, 15, 0xF7); //WiFi
  u8g2->drawGlyph(48, 15, 0xCD); // recycle
  u8g2->drawGlyph(64, 15, 0x103); // Sun
  u8g2->drawGlyph(80, 15, 0xDF); // Moon
  u8g2->drawGlyph(96, 15, 0xB8); // House
  u8g2->drawGlyph(112, 15, 0xB4); // Server
  */
  // Inside your loop or setup, call the function with the additional font arguments
  
  drawBigNumber("Energie", myNumber, myUnit,"Energie total",  myFormattingStr, numberFont, unitFont);

  u8g2->sendBuffer();
}

void DisplayManager::showScreen3(){
  u8g2->clearBuffer();
  
  /* u8g2->setFont(u8g2_font_unifont_t_weather);
  u8g2->drawGlyph(0, 15, 0x21); // No Moon
  u8g2->drawGlyph(16, 15, 0x22); // 0,25
  u8g2->drawGlyph(32, 15, 0x23); // 0,5
  u8g2->drawGlyph(48, 15, 0x24); // 0,75
  u8g2->drawGlyph(64, 15, 0x25); // 1,0
  u8g2->drawGlyph(80, 15, 0x26); // 1,25
  u8g2->drawGlyph(96, 15, 0x27); // 1,5
  u8g2->drawGlyph(112, 15, 0x28); // 1,75
*/

  // Inside your loop or setup, call the function with the additional font arguments
  drawBigNumber("Gestern", myNumber, myUnit,"Verbrauch",  myFormattingStr, numberFont, unitFont);

  
  u8g2->sendBuffer();
}

void DisplayManager::showScreen4(){
  u8g2->clearBuffer();

  u8g2->setFont(u8g2_font_siji_t_6x10);
  u8g2->drawGlyph(0, 15, 0xE217); // No WiFi
  u8g2->drawGlyph(16, 15, 0xE218); // 1Bar
  u8g2->drawGlyph(32, 15, 0xE219); // 2Bar
  u8g2->drawGlyph(48, 15, 0xE21A); // 3Bar
  u8g2->drawGlyph(64, 15, 0xE09F); // Computer
  u8g2->drawGlyph(80, 15, 0xE0AF); // NetDrive
  u8g2->drawGlyph(96, 15, 0xE140); // Ghost
  u8g2->drawGlyph(112,15,0xE141); // Small Ghost

  // Inside your loop or setup, call the function with the additional font arguments
  drawBigNumberNoHeader(myNumber, myUnit,"24h", myFormattingStr, numberFont, unitFont);

  u8g2->sendBuffer();
}


void DisplayManager::displayAction(const ActionData& action) {
  u8g2->clearBuffer();

  // Center and display the action name in the first row
  u8g2->setFont(u8g2_font_luRS14_tr); // Choose a suitable font for the action name
  int actionNameWidth = u8g2->getStrWidth(action.name.c_str()); // Get the width of the action name
  int startX = (SCREEN_WIDTH - actionNameWidth) / 2; // Calculate the starting X to center the text
  u8g2->drawStr(startX, 14, action.name.c_str()); // Draw the action name centered

  #ifdef DISPLAY_DEVELOPER
  // Display the details below the name
  u8g2->setFont(u8g2_font_6x10_tf);
  u8g2->drawStr(0, 24, action.details.c_str());


  // Display parameters
  u8g2->setFont(u8g2_font_6x10_tf); // Smaller font for parameters
  for (int i = 0; i < 4 && i < sizeof(action.params) / sizeof(action.params[0]); ++i) {
    if (action.params[i].length() > 0) {
      u8g2->drawStr(0, 33 + (i * 10), action.params[i].c_str());
    }
  }
    // Display result and result details in the last two rows
    u8g2->setFont(u8g2_font_6x10_tf); // Adjust the font as needed
  
    int spaceBetween = 4;
  
    // Calculate the position and size of the inverted background
    int yPosition = 63; // Y position for the text
    int resultWidth = u8g2->getStrWidth(action.result.c_str());
   
    String resultWithPadding = " " + action.result + " ";
  
    // Draw the result string in white on top of the black background
    u8g2->setDrawColor(0); // Set the draw color to white for the text
    u8g2->drawStr(0 , yPosition, resultWithPadding.c_str()); // Adjust text starting point if needed

    // Reset draw color to black for other elements
    u8g2->setDrawColor(1);

    // Continue with other drawing operations...
    
    int secondStrXPosition = resultWidth + 2* spaceBetween;

    String detailstWithPadding = " " + action.resultDetails + " ";

    // If you want the details not inverted, just draw them as normal.
    u8g2->drawStr(secondStrXPosition, yPosition, detailstWithPadding.c_str());
    #else 
    // Normal View

    // Display the details below the name
      u8g2->setFont(u8g2_font_6x10_tf);
      u8g2->drawStr(0, 30, action.details.c_str());

      u8g2->setFont(u8g2_font_6x13B_mr); // Adjust the font as needed

      int yPosition = 50; // Y position for the text
      String resultWithPadding = " " + action.result + " "; // Add padding to the result text
      String detailstWithPadding = " " + action.resultDetails + " "; // Add padding to the details text

      int halfScreenWidth = SCREEN_WIDTH / 2; // Half of the screen width for each text

      // Draw the result string in white on top of the black background
      u8g2->setDrawColor(0); // Set the draw color to white for the text
      u8g2->drawStr(0, yPosition, resultWithPadding.c_str()); // Draw result text starting from the left

      // Calculate the width of the details text with padding
      int detailsWidth = u8g2->getStrWidth(detailstWithPadding.c_str());

      // Calculate the starting X position for the details text so it's right-aligned in its half of the screen
      int secondStrXPosition = halfScreenWidth + (halfScreenWidth - detailsWidth) / 2;

      // Draw the details text
      u8g2->setDrawColor(1); // Reset draw color to black (or as needed) for the details text
      u8g2->drawStr(secondStrXPosition, yPosition, detailstWithPadding.c_str()); // Draw details text aligned to the right in its half

    #endif 

  u8g2->sendBuffer();
}

void DisplayManager::drawBigNumber(float number, String unit, String annotation, String formattingStr, const uint8_t *numberFont, const uint8_t *unitFont) {
   char numberBuffer[20]; // Buffer to hold the formatted number as a string

  // Format the floating-point number according to the provided formatting string
  snprintf(numberBuffer, sizeof(numberBuffer), formattingStr.c_str(), number);

  // Initially set the font for the number and calculate its width
  u8g2->setFont(numberFont);
  int numberWidth = u8g2->getStrWidth(numberBuffer);

  // Set the font for the unit and calculate its width
  u8g2->setFont(unitFont);
  int unitWidth = u8g2->getStrWidth(unit.c_str());

  // Check if the total width exceeds the display width
  if ((numberWidth + unitWidth + 4) > 128) {
    // Switch to smaller fonts
    numberFont = smallNumberFont;
    unitFont = smallUnitFont;

    // Recalculate widths with smaller fonts
    u8g2->setFont(numberFont);
    numberWidth = u8g2->getStrWidth(numberBuffer);

    u8g2->setFont(unitFont);
    unitWidth = u8g2->getStrWidth(unit.c_str());
  }

  // Calculate positions with potentially updated widths
  int unitX = 128 - unitWidth; // Position unit at the right edge
  int unitY = 63; // Vertical position for the unit, adjust as needed
  int numberX = unitX - numberWidth - 4; // Position number to the left of the unit, 4 pixels apart
  int numberY = unitY; // Align the baseline of the number with the unit

  u8g2->setFont(u8g2_font_profont11_mr);
  u8g2->drawStr(0, 24, annotation.c_str());
  
  
  // Draw the unit with its (possibly updated) font
  u8g2->setFont(unitFont);
  u8g2->drawStr(unitX, unitY, unit.c_str());

  // Draw the number with its (possibly updated) font
  u8g2->setFont(numberFont);
  u8g2->drawStr(numberX, numberY, numberBuffer);
}

void DisplayManager::drawBigNumberHEader(String header, float number, String unit, String annotation, String formattingStr, const uint8_t *numberFont, const uint8_t *unitFont) {
  u8g2->clearBuffer();

  // Center and display the action name in the first row
  u8g2->setFont(u8g2_font_luRS14_tr); // Choose a suitable font for the action name
  int actionNameWidth = u8g2->getStrWidth(header.c_str()); // Get the width of the action name
  int startX = (SCREEN_WIDTH - actionNameWidth) / 2; // Calculate the starting X to center the text
  u8g2->drawStr(startX, 14, header.c_str()); // Draw the action name centered

  drawBigNumberWithUnit(number, unit, annotation, formattingStr, numberFont, unitFont);
}
