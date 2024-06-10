#ifndef NTP_HANDLER_H
#define NTP_HANDLER_H

#include <NTPClient.h>
#include <WiFiUdp.h>
#include "EepromHandler.h"

class NtpHandler {
public:
    NtpHandler(const char* server);
    void begin();
    unsigned long getTime();

private:
    const char* ntpServer;
    WiFiUDP ntpUDP;
    NTPClient timeClient;
    EepromHandler eepromHandler; 
    unsigned long lastKnownTime;
};

#endif // NTP_HANDLER_H
