// DisplayManager.cpp

#include "DisplayManager.h"
#include "config.h"
#include "Inverter.h"

// Fonts for 64 Pixel Displays
const uint8_t *bigNumberHeadlineFont = u8g2_font_luRS10_tr;
const uint8_t *bigNumberAnnotationFont = u8g2_font_luRS10_tr;
const uint8_t *numberFont = u8g2_font_logisoso32_tr;
const uint8_t *unitFont = u8g2_font_luRS19_tr;
const uint8_t *smallNumberFont = u8g2_font_logisoso24_tr;
const uint8_t *smallUnitFont = u8g2_font_luRS12_tr;
// Action Display
const uint8_t *displayActionFontHeader = u8g2_font_luRS14_tr;
const uint8_t *displayActionFontBody = u8g2_font_6x10_tf;
const uint8_t *displayActionFontBodySmaller = u8g2_font_5x8_tf;


// Smaller 32 Pixel Displays
const uint8_t *bigNumberHeadlineFont_32 = u8g2_font_luRS08_tr;
const uint8_t *bigNumberAnnotationFont_32 = u8g2_font_luRS08_tr;
const uint8_t *numberFont_32 = u8g2_font_logisoso22_tr;
const uint8_t *unitFont_32 = u8g2_font_luRS14_tr;
const uint8_t *smallNumberFont_32 = u8g2_font_logisoso16_tr;
const uint8_t *smallUnitFont_32 = u8g2_font_luRS08_tr;
// Action Display
const uint8_t *displayActionFontHeader_32 = u8g2_font_5x8_tf;
const uint8_t *displayActionFontBody_32 = u8g2_font_5x8_tf;
const uint8_t *displayActionFontBodySmaller_32 = u8g2_font_4x6_tf;




DisplayManager::DisplayManager(SerialCaptureLines& serialCapture): serialCapture(serialCapture)  {
    // Conditional initialization based on board type
#ifdef BOARD_WEMOS_OLED_128x64_ESP32
    u8g2 = new U8G2_SSD1306_128X64_NONAME_F_SW_I2C(U8G2_R0, /* clock=*/ 4, /* data=*/ 5, /* reset=*/ U8X8_PIN_NONE);
#elif defined(BOARD_HELTEC_OLED_128x32_ESP8266)
    u8g2 = new U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C(U8G2_R0, /* reset=*/ U8X8_PIN_NONE, /* clock=*/ SCL, /* data=*/ SDA);
#elif defined(BOARD_HELTEC_WiFiKit_32_V3_OLED_128x32_ESP32)
    u8g2 = new U8G2_SSD1306_128X64_NONAME_F_SW_I2C(U8G2_R0, /* clock=*/ 18, /* data=*/ 17, /* reset=*/ 21); 
#elif defined(BOARD_WEMOS_OLED_128x32_ESP32_S2)
     u8g2 = new U8G2_SSD1306_128X32_UNIVISION_F_SW_I2C (U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ 18);
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

void DisplayManager::clearScreen() {
    u8g2->clearBuffer();
    u8g2->sendBuffer();
}


void DisplayManager::displayAction(const ActionData& action) {
  int yPositionBottomLine = SCREEN_HEIGHT - 1;
  String actionStr(action.name.c_str());
  
  //serialCapture.println("DisplayAction: " + actionStr);
  
  u8g2->clearBuffer();

  uint8_t lowerY_FirstLine = 14; // Action Name
  uint8_t lowerY_SecondLine = 25; // Action Deatails
  uint8_t lowerY_ThirdLine = 35; // Action Parameters [0]

  // For Pixel 32 Displays only first 3 rows can be used
  uint8_t lowerY_FourthLine = 44; // Action Parameters [1]
  uint8_t lowerY_FifthLine = 53; // Action Parameters [2]
  uint8_t lowerY_SixthLine = 63;  // Results

  //serialCapture.println("Displaying action name");
  // Center and display the action name in the first row
  if (SCREEN_HEIGHT == 64){
      u8g2->setFont(displayActionFontHeader); // Assuming the font is okay
      int actionNameWidth = u8g2->getStrWidth(action.name.c_str());
      int startX = (SCREEN_WIDTH - actionNameWidth) / 2;
      u8g2->drawStr(startX, lowerY_FirstLine, action.name.c_str());

      if (verboseDisplay) {
        //serialCapture.println("Displaying verbose details");
        // Display the details below the name
        u8g2->setFont(displayActionFontBody);
        u8g2->drawStr(0, lowerY_SecondLine, action.details.c_str());

        // Display parameters
        for (int i = 0; i < 3 && i < sizeof(action.params) / sizeof(action.params[0]); ++i) {
          if (action.params[i].length() > 0) {
            u8g2->drawStr(0, lowerY_ThirdLine + (i * 9), action.params[i].c_str());
          }
        }
      } else {
        //serialCapture.println("Displaying normal view");
        // Normal View
        u8g2->setFont(displayActionFontBody);
        u8g2->drawStr(0, lowerY_ThirdLine, action.details.c_str());
        yPositionBottomLine = 50;
      }

      //serialCapture.println("Displaying result with background padding");
      // Display result with background padding
      u8g2->setFont(displayActionFontBodySmaller); // Using a small font

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

      //serialCapture.println("Displaying result details aligned to the right");
      // Calculate text width for the details to align to the right
      int detailsWidth = u8g2->getStrWidth(action.resultDetails.c_str());
      int detailsStartX = SCREEN_WIDTH - detailsWidth; // Calculate X position for details to align right

      // Display result details without inverted color, aligned to the right
      u8g2->drawStr(detailsStartX, yPositionBottomLine, action.resultDetails.c_str());
  }
  else if (SCREEN_HEIGHT == 32){
      // For Pixel 32 Displays only first 4 rows can be used
      uint8_t lowerY_FirstLine = 9; // Action Name
      uint8_t lowerY_SecondLine = 16; // Action Deatails
      uint8_t lowerY_ThirdLine = 24; // Action Parameters [0]
      uint8_t lowerY_FourthLine = 31; // Action Parameters [1]

      u8g2->setFont(displayActionFontHeader_32); // Assuming the font is okay
      int actionNameWidth = u8g2->getStrWidth(action.name.c_str());
      int startX = (SCREEN_WIDTH - actionNameWidth) / 2;
      u8g2->drawStr(startX, lowerY_FirstLine, action.name.c_str());

      //serialCapture.println("Displaying verbose details");
      // Display the details below the name

      u8g2->setFont(displayActionFontBody_32);

      // Draw First two parameters
      u8g2->drawStr(0, lowerY_SecondLine, action.params[0].c_str());
      u8g2->drawStr(0, lowerY_ThirdLine, action.params[1].c_str());
      
      u8g2->setFont(displayActionFontBodySmaller_32); // Using a small font
      u8g2->drawStr(0, lowerY_FourthLine, action.result.c_str());

      //serialCapture.println("Displaying result details aligned to the right");
      int detailsWidth = u8g2->getStrWidth(action.resultDetails.c_str());
      int detailsStartX = SCREEN_WIDTH - detailsWidth; // Calculate X position for details to align right

      // Display result details without inverted color, aligned to the right
      u8g2->drawStr(detailsStartX, lowerY_FourthLine, action.resultDetails.c_str());
      u8g2->sendBuffer();
      delay(1500);
  }
  //serialCapture.println("Sending buffer to display");
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
    if (SCREEN_HEIGHT == 64){
      currentNumberFont = numberFont; // Choose a suitable font for the action name

    }else if (SCREEN_HEIGHT == 32){
      currentNumberFont = numberFont_32; // Choose a suitable font for the action name
      currentUnitFont = unitFont_32;
    }
    
    u8g2->setFont(currentNumberFont);
    int numberWidth = u8g2->getStrWidth(numberBuffer);

    // Set the font for the unit and calculate its width
    u8g2->setFont(currentUnitFont);
    int unitWidth = u8g2->getStrWidth(unit.c_str());

    // Check if the total width exceeds the display width and adjust fonts if necessary
    if ((numberWidth + unitWidth + 4) > 128) {
        //serialCapture.println("Switching to smaller fonts");
        if (SCREEN_HEIGHT == 64){
            currentNumberFont = smallNumberFont_32; // Choose a suitable font for the action name
        }else if (SCREEN_HEIGHT == 32){
            currentNumberFont = smallNumberFont_32; // Choose a suitable font for the action name
        }

        // Recalculate widths with smaller fonts
        u8g2->setFont(currentNumberFont);
        numberWidth = u8g2->getStrWidth(numberBuffer);

        u8g2->setFont(currentUnitFont);
        unitWidth = u8g2->getStrWidth(unit.c_str());
    }

    // Calculate positions with potentially updated widths
    int unitX = 128 - unitWidth; // Position unit at the right edge
    int unitY = SCREEN_HEIGHT -1; // Vertical position for the unit, adjust as needed
    int numberX = unitX - numberWidth - 4; // Position number to the left of the unit, 4 pixels apart
    int numberY = unitY; // Align the baseline of the number with the unit

    if (SCREEN_HEIGHT == 64){
      // Draw the annotation with a predefined font
      u8g2->setFont(bigNumberAnnotationFont);
      //serialCapture.println(annotation);
      u8g2->drawStr(0, 28, annotation.c_str());
    }else{
      //serialCapture.println("Annotations in number display are only availabe for 64 Pixel Displays");
    }


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

  uint_fast8_t headlineYPosition = 0;

  if (SCREEN_HEIGHT == 64){
    headlineYPosition = 13;
    u8g2->setFont(bigNumberHeadlineFont); // Choose a suitable font for the action name
  }else if (SCREEN_HEIGHT == 32){
    headlineYPosition = 11;
    u8g2->setFont(bigNumberHeadlineFont_32); // Choose a suitable font for the action name
  }

  // optional centering of the action name
  // int actionNameWidth = u8g2->getStrWidth(header.c_str()); // Get the width of the action name
  //int startX = (SCREEN_WIDTH - actionNameWidth) / 2; // Calculate the starting X to center the text
  u8g2->drawStr(0, headlineYPosition, header.c_str()); // Draw the action name centered

  drawBigNumberNoHeader(number, unit, annotation, formattingStr);
  u8g2->sendBuffer();
}


void DisplayManager::drawGraph(const Inverter::DataPoint powerData[], int dataSize) {
    int pointsToDisplay = SCREEN_WIDTH; // 128 points for 128 pixels wide display
    int verticalMargin = 2; // Margin at the top and bottom of the graph
    float scaleY = (SCREEN_HEIGHT - 2 * verticalMargin) / 610.0; // Scale factor for Y axis

    // Sample or average the data points to fit the display width
    float averagedData[pointsToDisplay];
    int samplesPerPoint = dataSize / pointsToDisplay;
    for (int i = 0; i < pointsToDisplay; ++i) {
        float sum = 0;
        for (int j = 0; j < samplesPerPoint; ++j) {
            int dataIndex = i * samplesPerPoint + j;
            sum += powerData[dataIndex % Inverter::bufferSize].value; // Assume wrap-around for circular buffer
        }
        averagedData[i] = sum / samplesPerPoint;
    }

    // Draw the graph
    u8g2->clearBuffer();
    for (int x = 0; x < pointsToDisplay - 1; ++x) {
        int y1 = SCREEN_HEIGHT - verticalMargin - (averagedData[x] * scaleY);
        int y2 = SCREEN_HEIGHT - verticalMargin - (averagedData[x + 1] * scaleY);
        u8g2->drawLine(x, y1, x + 1, y2);
    }
    u8g2->sendBuffer();
}