#include "SystemHealth.h"
// Detecting Reset Reason in ESP32 as described in 
// https://espressif-docs.readthedocs-hosted.com/projects/arduino-esp32/en/latest/api/reset_reason.html

#if CONFIG_IDF_TARGET_ESP32 // ESP32/PICO-D4
#include "esp32/rom/rtc.h"
#elif CONFIG_IDF_TARGET_ESP32S2
#include "esp32s2/rom/rtc.h"
#elif CONFIG_IDF_TARGET_ESP32C2
#include "esp32c2/rom/rtc.h"
#elif CONFIG_IDF_TARGET_ESP32C3
#include "esp32c3/rom/rtc.h"
#elif CONFIG_IDF_TARGET_ESP32S3
#include "esp32s3/rom/rtc.h"
#elif CONFIG_IDF_TARGET_ESP32C6
#include "esp32c6/rom/rtc.h"
#elif CONFIG_IDF_TARGET_ESP32H2
#include "esp32h2/rom/rtc.h"
#endif


SystemHealth::SystemHealth() {
  // Constructor
  update(); // Initialize all readings
}

void SystemHealth::update() {
  freeRAM = ESP.getFreeHeap() / 1000;
  uptimeSeconds = millis() / 1000;


  #ifdef ESP32
  resetReason = verbose_reset_reasonESP32();
  #else
  resetReason = ESP.getResetReason();
  #endif

    /*
  #ifdef ESP32
  resetReason = verbose_reset_reasonESP32();
  temperature = (temperature_sens_read() - 32) / 1.8;
  #else
  temperature = 0.0;  // Use an invalid value to denote unavailable temperature
  resetReason = ESP.getResetReason();
  #endif
    */

}

String SystemHealth::getFormattedUptime() const {
  return formatUptime(uptimeSeconds);
}

unsigned long SystemHealth::getUptimeSeconds() const {
  return uptimeSeconds;
}

unsigned long SystemHealth::getFreeRAM() const {
  return freeRAM;
}

String SystemHealth::getResetReason() const {
  return resetReason;
}

float SystemHealth::getTemperature() const {
  return temperature;
}

String SystemHealth::formatUptime(unsigned long seconds) const {
  unsigned long days = seconds / 86400;
  seconds %= 86400;
  unsigned long hours = seconds / 3600;
  seconds %= 3600;
  unsigned long minutes = seconds / 60;
  seconds %= 60;

  return String(days) + "d " + String(hours) + "h " + String(minutes) + "m " + String(seconds) + "s";
}


String SystemHealth::verbose_reset_reasonESP32()
{
  int reset_reason = rtc_get_reset_reason(0);
  switch ( reset_reason)
  {
    case 1  : return "Vbat power on reset";break;
    case 3  : return "Software reset digital core";break;
    case 4  : return "Legacy watch dog reset digital core";break;
    case 5  : return "Deep Sleep reset digital core";break;
    case 6  : return "Reset by SLC module, reset digital core";break;
    case 7  : return "Timer Group0 Watch dog reset digital core";break;
    case 8  : return "Timer Group1 Watch dog reset digital core";break;
    case 9  : return "RTC Watch dog Reset digital core";break;
    case 10 : return "Instrusion tested to reset CPU";break;
    case 11 : return "Time Group reset CPU";break;
    case 12 : return "Software reset CPU";break;
    case 13 : return "RTC Watch dog Reset CPU";break;
    case 14 : return "for APP CPU, reseted by PRO CPU";break;
    case 15 : return "Reset when the vdd voltage is not stable";break;
    case 16 : return "RTC Watch dog reset digital core and rtc module";break;
    default : return "NO_MEAN";
  }
}