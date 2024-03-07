## Boards 
### ESP8266 Support
//Add url to the boardmanager:
https://arduino.esp8266.com/stable/package_esp8266com_index.json

Install boards "esp8266" by ESP8266 Community V3.1.2

### ESP32 Support


## Software dependencies

Current (07.03.2024) working list of used libraries 
* [To Install] PubSubClient in Version 2.8 / PubSubClient by Nick O'Leary <nick.oleary@gmail.com> Version 2.8.0 INSTALLED
* [To Install] U8g2 in Version 2.34.22 / U8g2 by oliver <olikraus@gmail.com> Version 2.34.22 INSTALLED

* [To Install] Time in Version 1.6.1 / Time by Michael Margolis Version 1.6.1 INSTALLED
* [To Install] RTClib in Version 2.1.1 / RTClib by Adafruit Version 2.1.1 INSTALLED
* [To Install] Adafruit_BusIO in Version 1.14.4 / Adafruit BusIO by Adafruit Version 1.14.4 INSTALLED


* [Comes with board software] SPI in Version 1.0
* [Comes with board software] Wire in Version 1.0
* [Comes with board software] ESP8266WiFi in Version 1.0


### Testing libs (other branches)
 To be checked if still needed: HttpClient by Adrian McEwen <adrianm@mcqn.com> Version 2.2.0 INSTALLED
 Ticker by Stefan Staub Version 4.4.0 INSTALLED
 ArduinoOTA(esp32) Built-In by Ivan Grokhotkov and Hristo Gochkov Version 2.0.0 INSTALLED
 EEPROM Built-In by Ivan Grokhotkov Version 2.0.0 INSTALLED

# Changes
## Graphics Library
Adfruit Graphics are not used anymore as the Lib was not capable of talking to some of my tested oled Displays! --> Swiched to u8g2 lib!
Adafruit SSD1306 by Adafruit V2.5.7
Adafruit "GFX Library" by Adafruit Version 1.11.8 INSTALLED

Removed NTP CLient to use internal functions of ESP
* [not needed] NTPClient in Version 3.2.1 / Fabrice Weinberg Version 3.2.1 INSTALLED

