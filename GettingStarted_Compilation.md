# Prepare Arduino

> [!NOTE]
> I use Arduino IDE 1.8.16 as you can make it portable and it seems to not be bloated too much. I had perfomance issues with the 2.* Versions.

> [!NOTE]
> For Code editing I used VScode with Arduino and C++ Extensions. Unfortunately the flashing did not really work out of VScode, so I also used the IDE to compile and flash. 
> To have a similar setup, I would encourage you to set the flag in the Arduino Preferences that you use an external IDE.


## Adding Boards to Arduino

Add the following URLs (comma separated) to you boardmanager if you are missing several boards: 
- https://adafruit.github.io/arduino-board-index/package_adafruit_index.json, 
- https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json,
- http://arduino.esp8266.com/stable/package_esp8266com_index.json

### ESP32
- Install "esp32" by Espressiv Systems from Board Manager

### ESP8266 
- Install boards "esp8266" by ESP8266 Community V3.1.2


## Installing needed libraries

### Libraries you need to install 
* PubSubClient in Version 2.8 / PubSubClient by Nick O'Leary <nick.oleary@gmail.com> Version 2.8.0 INSTALLED
* U8g2 in Version 2.34.22 / U8g2 by oliver <olikraus@gmail.com> Version 2.34.22 INSTALLED
* Time in Version 1.6.1 / Time by Michael Margolis Version 1.6.1 INSTALLED
* RTClib in Version 2.1.1 / RTClib by Adafruit Version 2.1.1 INSTALLED
* Adafruit_BusIO in Version 1.14.4 / Adafruit BusIO by Adafruit Version 1.14.4 INSTALLED
* Preferences by Volodymyr Shymanskyy Version 2.1.0 --  Preferences library for ESP8266 and Particle Gen3 devices, with ESP32-compatible API


### Libraries that come with installing the boards or Adruino IDE

* SPI in Version 1.0
* Wire in Version 1.0
* ESP8266WiFi in Version 1.0 (only for ESP8266)

### Optional libraries (not used in main)
* Adafruit NeoPixel by Adafruit Version 1.12.0

# Prepare the code and settings

## Creating the Secrets File
> [!IMPORTANT]
> To compile, SPIDR curremtly requires a **arduino-secrets.h** file that is ignored by GIT for security reasons. 

Create a copy of the Secrets template and name it 
```
cp arduino_secrets-template.h arduino_secrets.h
```
You can hardcode you credentials into it, so after compilation SPIDR will connect automatically connect, or you can also leave the default settings and configure it later via the SPIDR web frontend and it's AP mnode. 

## Configuring your board
You need to adapt you Arduino Board and the **config.h** file in order that everything can be compiled.

The config settings for several tested boards are explained under [Supported Hardware](GettingStarted_SupportedHardware.md)