
// Benutzung von u8Gg2
//
// Die wichtigsten u8g2 Funktionen sind:

    // Es gibt unzählige Fonts in verschiedenen Größe und Stilen.
    setFont(font); 

    // Zeichnet einen String an der Position x, y = untere Kante des Textes
    drawStr(startX, lowerY, "Hallo Display");

    // Gibt die Breite eines Strings in abhängigkeit
    // aktuellen Font zurück ohne den String zu zeichnen. 
    getStrWidth("Hallo Welt");

    // Löscht den Inhalt des Displays
    clearBuffer();

    // Sendet das Update an das Display
    sendBuffer();


// Zu den Konstruktoren:







// Datentyp mit dem die Daten für eine Aktion übergeben werden
struct ActionData {
  String name;
  String details;
  String params[3]; // Bis zu 3 Informationszeilen
  String result;
  String resultDetails;
};


// Klasse für die Ansteuerung des Displays
// ActionData wird als Parameter übergeben und damit 
// alle Informationen für die Anzeige
void DisplayManager::displayAction(const ActionData& action) {
    int yPositionBottomLine = SCREEN_HEIGHT - 1;
    String actionStr(action.name.c_str());
    u8g2->clearBuffer();

    // Zentrieren der Headline
    u8g2->setFont(displayActionFontHeader); // Assuming the font is okay
    int actionNameWidth = u8g2->getStrWidth(action.name.c_str());
    int startX = (SCREEN_WIDTH - actionNameWidth) / 2;
    u8g2->drawStr(startX, lowerY_FirstLine, action.name.c_str());

    // Schreiben des Detail String 
    u8g2->setFont(displayActionFontBody);
    u8g2->drawStr(0, lowerY_SecondLine, action.details.c_str());

    // Schreiben aller Parameter Zeilen
    for (int i = 0; i < 3 && i < sizeof(action.params) / sizeof(action.params[0]); ++i) {
        if (action.params[i].length() > 0) {
        u8g2->drawStr(0, lowerY_ThirdLine + (i * 9), action.params[i].c_str());
        }
    }
    // Nutze einen kleineren Font
    u8g2->setFont(displayActionFontBodySmaller); // Using a small font

    // Berechne die Breite des Result Strings und füge 8 Pixel Rand hinzu
    int resultWidth = u8g2->getStrWidth(action.result.c_str()) + 8; // 4 pixels padding on each side
    int resultStartX = 0; // Starting X position for result box

    // Zeichne ein Hintergrund-Rechteck für den Result String
    u8g2->setDrawColor(1); // Set color for background
    u8g2->drawBox(resultStartX, yPositionBottomLine - 8, resultWidth, 10); // Height is approximate, adjust as needed

    // Schreibe den Result String in invertierter Farbe
    u8g2->setDrawColor(0); // Set color for text
    u8g2->drawStr(resultStartX + 4, yPositionBottomLine, action.result.c_str());

    // Setze die Farbe zurück auf normal
    u8g2->setDrawColor(1);

    // Berechne die Breite des Details Strings um ihn rechtsbündig zu setzen
    int detailsWidth = u8g2->getStrWidth(action.resultDetails.c_str());
    int detailsStartX = SCREEN_WIDTH - detailsWidth; // Calculate X position for details to align right

    // Schreiben des Result Details Strings
    u8g2->drawStr(detailsStartX, yPositionBottomLine, action.resultDetails.c_str());

    // Update an das Display senden
    u8g2->sendBuffer();
}


void DisplayManager::drawBigNumberNoHeader(float number, String unit, String annotation, String formattingStr) {

    // Initialisiere die Fonts
    const uint8_t * currentNumberFont = numberFont; // Assuming default larger font
    const uint8_t* currentUnitFont = unitFont; // Assuming default larger font

    // Umwandeln der Zahl in einen String
    char numberBuffer[20]; // Buffer to hold the formatted number as a string
    snprintf(numberBuffer, sizeof(numberBuffer), formattingStr.c_str(), number);

    // Berechne die Breite des Zahlen-Strings
    u8g2->setFont(currentNumberFont);
    int numberWidth = u8g2->getStrWidth(numberBuffer);

    // Berechne die Breite des Einheiten-Strings
    u8g2->setFont(currentUnitFont);
    int unitWidth = u8g2->getStrWidth(unit.c_str());

    // Sind beide Strings breiter als das Display (inkl. 4 Pixel Abstand)
    if ((numberWidth + unitWidth + 4) > 128) {
        currentNumberFont = smallNumberFont; // Choose a suitable font for the action name
        currentUnitFont = smallUnitFont;
        // Setze kleinere Fonts ein und berechne die Breiten neu
        u8g2->setFont(currentNumberFont);
        numberWidth = u8g2->getStrWidth(numberBuffer);
        u8g2->setFont(currentUnitFont);
        unitWidth = u8g2->getStrWidth(unit.c_str());
    }

    // Berechne die Positionen
    int unitX = 128 - unitWidth; // Position unit at the right edge
    int unitY = SCREEN_HEIGHT -1; // Vertical position for the unit, adjust as needed
    int numberX = unitX - numberWidth - 4; // Position number to the left of the unit, 4 pixels apart
    int numberY = unitY; // Align the baseline of the number with the unit

    // Schreibe die Zahlenüberschrift
    u8g2->setFont(bigNumberAnnotationFont);
    u8g2->drawStr(0, 28, annotation.c_str());

    // Schreibe die Einheit mit dem (möglicherweise aktualisierten) Font
    u8g2->setFont(currentUnitFont);
    u8g2->drawStr(unitX, unitY, unit.c_str());

    // Schreibe die Zahl mit dem (möglicherweise aktualisierten) Font
    u8g2->setFont(currentNumberFont);
    u8g2->drawStr(numberX, numberY, numberBuffer);

    // Update an das Display senden
    u8g2->sendBuffer();
}