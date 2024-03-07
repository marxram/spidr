//#define BOARD_WEMOS_OLED_128x64_ESP32
//#define BOARD_HELTEC_OLED_128x32_ESP8266


///////////////////////////////////////////////////////////////////////
// HARDWARE SPECIFIC ADAPTIONS 
// DISPLAY ------------------------------------------------------------
//#define SCREEN_WIDTH 128 // OLED display width, in pixels


#ifdef BOARD_HELTEC_OLED_128x32_ESP8266
  #define SCREEN_HEIGHT 32 // OLED display height, in pixels
  #define HEADER_FONT       u8g2_font_spleen6x12_me
  #define NORMAL_FONT       u8g2_font_spleen6x12_me
  #define ANNOTATION_FONT   u8g2_font_5x7_tf 
#else 
  #define SCREEN_HEIGHT 64 // OLED display height, in pixels
  #define HEADER_FONT       u8g2_font_spleen8x16_me 
  #define NORMAL_FONT       u8g2_font_spleen6x12_me 
  #define ANNOTATION_FONT   u8g2_font_spleen5x8_me 
#endif 

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)


#define GMT_OFFSET_SECONDS 3600 // Offset of your timezone to GMT in seconds (3600 = 1h)
#define DST_OFFSET_SECONDS 3600 // Offset of your Daylight Saving Time in seconds (3600 = 1h)
#define NTP_SERVER "pool.ntp.org" // Server to use for time sync
#define NTP_FALLBACK_SERVER "time.nist.gov" // Fallback server in case the first one fails

// Only define Screen Address if display is not working by default
// The display library tries to detect the address automatically
//#define SCREEN_ADDRESS 0x3C 
// Address Examples: 0x3C, 0x3D, 0x78

///////////////////////////////////////////////////////////////////////
// Timing behavior
#define DURATION_STAY_IN_HOME_NETWORK 200  // Seconds


