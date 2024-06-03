#ifndef WIFI_HANDLER_H
#define WIFI_HANDLER_H

#include <ESP8266WiFi.h>

class WiFiHandler {
public:
    void begin(const char* ssid, const char* password);
    bool connect();

private:
    const char* ssid;
    const char* password;
};

#endif // WIFI_HANDLER_H
