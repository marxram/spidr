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
