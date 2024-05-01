#ifndef SystemHealth_h
#define SystemHealth_h

#include <Arduino.h>

class SystemHealth {
public:
  SystemHealth();
  void update();
  String getFormattedUptime()  const;
  unsigned long getFreeRAM()  const;
  String getResetReason()  const;
  float getTemperature()  const; // Only valid for ESP32
  unsigned long getUptimeSeconds()  const;

private:
  unsigned long freeRAM;
  String resetReason;
  unsigned long uptimeSeconds;
  float temperature;  // Only for ESP32

  String formatUptime(unsigned long seconds) const;
  String verbose_reset_reasonESP32();

};

#endif
