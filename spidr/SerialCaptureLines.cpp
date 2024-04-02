#include "SerialCaptureLines.h"

SerialCaptureLines::SerialCaptureLines(size_t _maxLines) : maxLines(_maxLines), currentLine(0), lineCount(0) {
    lines = new String[maxLines];
}

SerialCaptureLines::~SerialCaptureLines() {
    delete[] lines;
}

size_t SerialCaptureLines::write(uint8_t character) {
    Serial.write(character); // Echo the character to the standard serial output
    if (character == '\n') {

        time_t now = time(nullptr); // Get the current time
        struct tm *timeinfo = localtime(&now); // Convert to local time structure

        char timeStr[34];
        strftime(timeStr, sizeof(timeStr), "%d.%m.%Y %Hh%Mm%S", timeinfo);


        // Get current time and format it as a timestamp
        char timestamp[20]; // Assuming timestamp length will be less than 20 characters
        snprintf(timestamp, sizeof(timestamp), "%7lu", millis());
        
        // Add the timestamp to the current line
        lines[currentLine] = String(timestamp) + " | "+ String(timeStr) + " | " + lines[currentLine];

        currentLine = (currentLine + 1) % maxLines;
        if (lineCount < maxLines) lineCount++;
        lines[currentLine] = ""; // Prepare the new line
    } else {
        lines[currentLine] += (char)character;
    }
    return 1; // Indicate 1 character was written
}

void SerialCaptureLines::clearBuffer() {
    for (size_t i = 0; i < maxLines; ++i) {
        lines[i] = "";
    }
    currentLine = 0;
    lineCount = 0;
}

String SerialCaptureLines::getBuffer() const {
    String buffer;
    for (size_t i = 0; i < lineCount; ++i) {
        // Calculate the index of the line considering the circular buffer logic
        size_t lineIndex = (currentLine + maxLines - lineCount + i + 1) % maxLines;
        buffer += lines[lineIndex] + '\n';
    }
    return buffer;
}
