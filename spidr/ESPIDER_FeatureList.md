# S|P|I|D|R

S| mart Home and
P| rivacy focused
I| oT
D| ata
R| relay


## Table of contents Contents
- [Features](#features)
- [Networking](#networking)
- [Inverter](#inverter)
- [Smart Home Integration](#smart-home-integration)
- [Display](#display)
- [Webinterface](#webinterface)
- [Hardware Compatibility](#hardware-compatibility)
- [ToDo](#todo)
- [Bugfixing](#bugfixing)

## Features

### Networking
* WiFi Accesspoint Mode if Home Network is not available or set incorrectly
* Retry if Networks were not reachable for the first time (necessary debugging)
* Option: NTP Time Sync (needed for special things like resetting the daily counter, etc.)

### Inverter
* Retrieve Data via Screen Scraping
* Energy Today Counter is reset via UDP commands to synchronize the time
* UDP Commands can be extended in the Future

### Smart Home Integration
* Inverter Data is sent to a local MQTT Broker
* If Broker is Configured with HomeAssistant the Inverter Should be automatically Detected by HA
* Icons and topics can be set in the MQTTManager

## Display
* Current Power
* Energy Production today
* Energy Production total
* Power Graph (just PoC, using generated values at ESP startup)
* Time Display
* Show different Action Results

### Webinterface
* Webserver available in Home-Network and AP-Mode **There might be issues if the system is switching for Inverter Mode** --> Just wait
* Home Page: showing Inverter Status
* Config Page: for most important Settings (Handling Configuration Changes with Preferences Manager)
* Config Options Page: For more detailed setting of behavior (Handling Configuration Changes with Preferences Manager)
* Serial Page: See Debug Information, Serial Log
* WiKi Page: Information and Links

## Hardware Compatibility
* Generally compatible and tested with some ESP8266
* Generally compatible and tested with some ESP32
* Different I2C OLED Display types with 128x64 and 128x32 were tested (Using U8g2)
* Special Boards were tested and have Config Defines
  * BOARD_WEMOS_OLED_128x64_ESP32
  * BOARD_HELTEC_OLED_128x32_ESP8266
  * BOARD_HELTEC_WiFiKit_32_V3_OLED_128x64_ESP32
  * BOARD_WEMOS_OLED_128x32_ESP32_S2
  * Use Generic ESP32 or ESP8266 and set the I2C Address (SCREEN_ADDRESS 0x3C) or change a U8g2 Constructor in the DisplayManager::DisplayManager Constructor

## ToDo

### Known Bugs
* Check if Relay gets disconnected because of UDP commands
* Check NTPisActive Parameter
* Bugfix NTP behavior
* AP - ssid not what is set in Pr
* MQTT Topic using from Preferences -> Currently Hardcoded: "state_topic": "SolarInverterBridge/inverter
* Saving AP Net Parameters seems not to work

## Feature Improvements

### OLED Handling Screensaver
* Turn OLED Off during Night
* Enable by pressing User Button
* Disable after a certain amount of time
* Better Graph Display

### Convenience
* Use User LED to display errors

### Inverter and Data Handling
* Reset Daily counter early morning (currently done with the invOffTimeoutS Preference)

## Bugfixing

### Issue: Display stays blank
Before you change anything in this code, I advise you to create a small Demo project with your Controller and LED Setup using U8g2 lib.
If this is working, check if the Constructor used in this project is set correctly. 
Hint: The different specific Board DEFINES also set the U8g2 Constructor. You might need to uncomment them out!
