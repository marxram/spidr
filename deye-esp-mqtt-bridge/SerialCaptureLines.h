#ifndef SerialCaptureLines_h
#define SerialCaptureLines_h

#include <Arduino.h>

class SerialCaptureLines : public Print {
private:
    String* lines; // Dynamic array of lines
    size_t maxLines; // Maximum number of lines in the buffer
    size_t currentLine; // Index of the current line being written to
    size_t lineCount; // Total number of lines currently stored

public:
    SerialCaptureLines(size_t _maxLines = 100);
    virtual ~SerialCaptureLines();
    size_t write(uint8_t character) override;
    void clearBuffer();
    String getBuffer() const;
};

#endif
