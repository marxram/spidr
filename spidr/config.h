#define SW_VER "1.0"
#define COMMIT "746ec207"


///////////////////////////////////////////////////////////////////////
// HARDWARE SPECIFIC ADAPTIONS 
// DISPLAY ------------------------------------------------------------

// Only define Screen Address if display is not working by default
// The display library tries to detect the address automatically
//#define SCREEN_ADDRESS 0x3C   // Address Examples: 0x3C, 0x3D, 0x78

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)

///////////////////////////////////////////////////////////////////////
// HARDWARE SPECIFIC ADAPTIONS 
// Board Settings (I2C and Pins) -------------------------------------
// You don't need to do something here, if the board uses the normal I2C pins!

//#define BOARD_WEMOS_OLED_128x64_ESP32
//#define BOARD_HELTEC_OLED_128x32_ESP8266
#define BOARD_HELTEC_WiFiKit_32_V3_OLED_128x64_ESP32
//#define BOARD_WEMOS_OLED_128x32_ESP32_S2
//#define BOARD_ESP8266_OLED_Black_128x64_SDA_D1__SDC_D2

#ifdef BOARD_WEMOS_OLED_128x64_ESP32
    #define HWTYPE "WEMOS-OLED-128x64"
#elif defined(BOARD_HELTEC_OLED_128x32_ESP8266)
   #define HWTYPE "HELTEC-OLED-128x32"
#elif defined(BOARD_HELTEC_WiFiKit_32_V3_OLED_128x64_ESP32)
    #define HWTYPE "HELTEC-OLED-128x64-WiFiKitV3"
#elif defined(BOARD_WEMOS_OLED_128x32_ESP32_S2)
    #define HWTYPE "WEMOS-OLED-128x32-S2"
#elif defined(BOARD_ESP8266_OLED_Black_128x64_SDA_D1__SDC_D2)
    #define HWTYPE "NodeMCU-OLED-128x64"
#else
    #ifdef ESP8266
        #define HWTYPE "ESP-8266-GENERIC"
    #endif
    #ifdef ESP32
        #define HWTYPE "ESP-32-GENERIC"
    #endif
#endif


#define USE_SECRETS_FILE_AS_DEFAULT


//#define USE_NEOPIXEL_LED


// Configurations that have been moved to the Preferences Manager and can be updated during Runtime
// Only Defaults below

#define DEF_WIFI_AP_NAME "SPIDR-net"
#define DEF_WIFI_AP_PASSWORD "SPIDR-pass"


// TimeSynchronization and handling
#define DEF_USE_NTP_SYNC true
#define DEF_GMT_OFFSET_SECONDS 3600 // Offset of your timezone to GMT in seconds (3600 = 1h)
#define DEF_DST_OFFSET_SECONDS 3600 // Offset of your Daylight Saving Time in seconds (3600 = 1h)
#define DEF_NTP_SERVER "ptbtime1.ptb.de" // Server to use for time sync
#define DEF_NTP_FALLBACK_SERVER "ptbtime2.ptb.de" // Fallback server in case the first one fails

///////////////////////////////////////////////////////////////////////
// Timing behavior
#define DEF_DURATION_STAY_IN_HOME_NETWORK_MS   180000  
#define DEF_DURATION_STAY_IN_HOME_NETWORK_MS_FIRST_BOOT 10000
#define DEF_DURATION_STAY_IN_AP_NETWORK_MS     180000  

// Timeout how long wifiConnect should try
#define DEF_WIFI_CONNECT_TIME_WINDOW_S 10

// Inverter Considered to be offline Timeout
#define DEF_INVERTER_OFFLINE_TIMEOUT_SECONDS 360 // 6 Minutes
                                                 // Last known power will be reset to 0 W after this time

#define DEF_DISP_POWER_MS 5000 // Default display duration for power, in milliseconds
#define DEF_DISP_ENERGY_TODAY_MS 5000 // Default display duration for energy today, in milliseconds
#define DEF_DISP_ENERGY_TOTAL_MS 5000 // Default display duration for energy total, in milliseconds
#define DEF_DISP_ENERGY_TIME_MS  4000 // Default display duration for energy time, in milliseconds
#define DEF_DISP_ENERGY_GRAPH_MS 10000 // Default display duration for energy graph, in milliseconds
