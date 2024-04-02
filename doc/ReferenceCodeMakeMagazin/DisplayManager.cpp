#include "DisplayManager.h" 
// # Benutzung von u8Gg2
//


// Die wichtigsten u8g2 Funktionen sind:

// Setzt den Font, der für die Textdarstellung verwendet wird. u8g2 unterstützt eine Vielzahl von Fonts in unterschiedlichen Größen und Stilen.
u8g2.setFont(font);

// drawStr(startX, lowerY, "Hallo Display"):
// Zeichnet einen String an der Position (startX, lowerY), wobei lowerY die untere Kante des Textes angibt.
// Diese Funktion ermöglicht es, Texte präzise auf dem Display zu positionieren.
u8g2.drawStr(startX, lowerY, "Hallo Display");

// Gibt die Breite eines Strings zurück, abhängig vom aktuell gesetzten Font. Diese Funktion zeichnet den String nicht,
// ist aber nützlich, um Layout-Berechnungen durchzuführen, z.B. um Texte zu zentrieren oder um festzustellen,
// ob ein Text innerhalb eines bestimmten Bereichs passt.
u8g2.getStrWidth("Hallo Welt");

// clearBuffer():
// Löscht den Inhalt des Display-Buffers. Diese Aktion entfernt alles, was zuvor gezeichnet wurde,
// sodass man mit einem leeren Bildschirm beginnen kann. Es ist besonders nützlich, um das Display
// für die nächste Anzeige vorzubereiten, ohne Reste der vorherigen Inhalte zu hinterlassen.
u8g2.clearBuffer();

// sendBuffer():
// Sendet das Update an das Display. Diese Funktion überträgt den Inhalt des Buffers auf das Display,
// wodurch alle zuvor mit Zeichenfunktionen hinzugefügten Inhalte angezeigt werden. Es ist wichtig,
// diese Funktion aufzurufen, nachdem man mit dem Zeichnen des Bildschirminhalts fertig ist,
// um die Änderungen sichtbar zu machen.
u8g2.sendBuffer();


// Zu den Konstruktoren:
// Gefühlt sind alle boards mit OLED modulen und auch die OLED module selbst unterschieldich.
// Es unterscheiden asich natürlich die Pixeldimensionen, was aber das einfachste ist. Daneben gibt es noch die I2C Adresse, die z.T. aufgedruckt und änderbar ist.
// Bei den boards gibt es welche, bei denen der I2C Bus mit den dafür vorgesehenen HW-OPins verbunden ist, bei manchen boards wurden belibige Pins genommen. 
// Stellenweise habe ich hier auch für manche boards widersprüchliche Informationen gefunden.

// Sehr hilfreich war hier die Library u8g2. Im Vergleich zur Adafruit Graphics Library konnte man hier viel mehr einstellen, als die I2C Adresse. 
// Mit der Adafruit Library gelang es mir auch ein OLEd anzusprechen, allerdings unter einer anderen I2C Adresse und nur mit halber Auflösung. 

// Die U8g2 library benötogt in der Regel nicht einmal die Angabe der I2C Adresse. Weswegen die Konstruktoren ohne auskommen. 
// Nur bei Problemem kann man diese auch mahnuell setzen. Das macht ins besondere Sinn, wenn man Mehrere Displays verwenden möchte. 

// Für bestimmte boards habe ich die Konstruktoren mit den entsprechenden Pins und I2C Adressen versehen. 

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
#elif defined(BOARD_ESP8266_OLED_Black_128x64_SDA_D1__SDC_D2)
    u8g2 = new U8G2_SSD1306_128X64_NONAME_F_SW_I2C(U8G2_R0, /* clock=*/ 2, /* data=*/ 1, /* reset=*/ U8X8_PIN_NONE);
#else
    u8g2 = new U8G2_SSD1306_128X64_NONAME_F_SW_I2C(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);
#endif
}



// Datentyp mit dem die Daten für eine Aktion übergeben werden
struct ActionData {
  String name;
  String details;
  String params[3]; // Bis zu 3 Informationszeilen
  String result;
  String resultDetails;
};


void DisplayManager::displayAction(const ActionData& action) {
    int yPositionBottomLine = SCREEN_HEIGHT - 1;    // Berechnet die Y-Position der untersten Zeile
    String actionStr(action.name.c_str());          // Konvertiert den Namen der Aktion in einen String
    u8g2->clearBuffer();                            // Löscht den Display-Buffer
    u8g2->setFont(displayActionFontHeader);         // Setzt den Font für die Überschrift
    // Berechnet die Breite des Aktion-Namens
    int actionNameWidth = u8g2->getStrWidth(action.name.c_str()); 
    int startX = (SCREEN_WIDTH - actionNameWidth) / 2; // Berechnet den Startpunkt für zentrierten Text
    u8g2->drawStr(startX, lowerY_FirstLine, action.name.c_str()); // Zeichnet den Namen der Aktion zentriert
    u8g2->setFont(displayActionFontBody);           // Setzt den Font für den Detailtext
    u8g2->drawStr(0, lowerY_SecondLine, action.details.c_str()); // Zeichnet die Details der Aktion
    // Durchläuft alle Parameter und zeichnet sie, falls vorhanden
    for (int i = 0; i < 3 && i < sizeof(action.params) / sizeof(action.params[0]); ++i) {
        if (action.params[i].length() > 0) {
            u8g2->drawStr(0, lowerY_ThirdLine + (i * 9), action.params[i].c_str()); // Zeichnet jeden Parameter
        }
    }
    u8g2->setFont(displayActionFontBodySmaller);    // Setzt einen kleineren Font für den Ergebnistext
    int resultWidth = u8g2->getStrWidth(action.result.c_str()) + 8; // Berechnet die Breite des Ergebnistexts inkl. Rand
    int resultStartX = 0;                           // Startposition für das Ergebnis festlegen
    u8g2->setDrawColor(1);                          // Setzt die Zeichenfarbe für das Hintergrund-Rechteck
    u8g2->drawBox(resultStartX, yPositionBottomLine - 8, resultWidth, 10); // Zeichnet ein Hintergrund-Rechteck
    u8g2->setDrawColor(0);                          // Invertiert die Farbe für den Ergebnistext
    u8g2->drawStr(resultStartX + 4, yPositionBottomLine, action.result.c_str()); // Zeichnet den Ergebnistext
    u8g2->setDrawColor(1);                           // Setzt die Zeichenfarbe zurück
    int detailsWidth = u8g2->getStrWidth(action.resultDetails.c_str()); // Berechnet die Breite des Detailtexts des Ergebnisses
    int detailsStartX = SCREEN_WIDTH - detailsWidth; // Berechnet den Startpunkt für rechtsbündigen Text
    u8g2->drawStr(detailsStartX, yPositionBottomLine, action.resultDetails.c_str()); // Zeichnet die Detailinfos des Ergebnisses
    u8g2->sendBuffer();                             // Sendet das gezeichnete Bild zum Display
}


void DisplayManager::drawBigNumberNoHeader(float number, String unit, String annotation, String formattingStr) {
    const uint8_t * currentNumberFont = numberFont; // Setzt den Standardfont für Zahlen
    const uint8_t* currentUnitFont = unitFont;  // Setzt den Standardfont für Einheiten
    char numberBuffer[20];                      // Puffer für die formatierte Zahl als String
    // Formatieren der Zahl als String gemäß dem Formatierungsmuster
    snprintf(numberBuffer, sizeof(numberBuffer), formattingStr.c_str(), number); 
    u8g2->setFont(currentNumberFont);           // Setzt den Font für die Darstellung der Zahl
    int numberWidth = u8g2->getStrWidth(numberBuffer); // Berechnet die Breite des Zahlenstrings
    u8g2->setFont(currentUnitFont);             // Setzt den Font für die Darstellung der Einheit
    int unitWidth = u8g2->getStrWidth(unit.c_str()); // Berechnet die Breite des Einheitenstrings
    // Überprüft, ob die Breite von Zahl und Einheit größer als das Display ist
    if ((numberWidth + unitWidth + 4) > 128) {  
        currentNumberFont = smallNumberFont;    // Wechselt zu einem kleineren Font für die Zahl
        currentUnitFont = smallUnitFont;        // Wechselt zu einem kleineren Font für die Einheit
        u8g2->setFont(currentNumberFont);       // Setzt den neuen Font für die Zahl
        numberWidth = u8g2->getStrWidth(numberBuffer); // Berechnet erneut die Breite des Zahlenstrings
        u8g2->setFont(currentUnitFont);         // Setzt den neuen Font für die Einheit
        unitWidth = u8g2->getStrWidth(unit.c_str()); // Berechnet erneut die Breite des Einheitenstrings
    }
    int unitX = 128 - unitWidth;                // Position der Einheit am rechten Rand
    int unitY = SCREEN_HEIGHT - 1;              // Vertikale Position für die Einheit, bedarf ggf. einer Anpassung
    int numberX = unitX - numberWidth - 4;      // Positioniert die Zahl links von der Einheit mit 4 Pixel Abstand
    int numberY = unitY;                        // Richtet die Basislinie der Zahl mit der der Einheit aus
    u8g2->setFont(bigNumberAnnotationFont);     // Setzt den Font für die Überschrift
    u8g2->drawStr(0, 28, annotation.c_str());   // Zeichnet die Überschrift
    u8g2->setFont(currentUnitFont);             // Setzt den (eventuell aktualisierten) Font für die Einheit
    u8g2->drawStr(unitX, unitY, unit.c_str());  // Zeichnet die Einheit
    u8g2->setFont(currentNumberFont);           // Setzt den (eventuell aktualisierten) Font für die Zahl
    u8g2->drawStr(numberX, numberY, numberBuffer); // Zeichnet die Zahl
    u8g2->sendBuffer();                         // Sendet das Update an das Display
}


// Initiiere neues Action Display 
ActionData action;
action.name = "MQTT Sync";
action.details = "Sende Daten";
action.params[0] = "Broker: " + String(_broker);
action.params[1] = "Port:   " + String(_port);
action.result = "In Arbeit";

displayManager.displayAction(action); // Sende die Daten zur Anzeige

// Dinge werden getan...
// ...
// 

action.result = "Done";                // überschreibt das Result und ResultDetails
action.resultDetails = "Published";

displayManager.displayAction(action);   // Sende die Daten zur Anzeige





