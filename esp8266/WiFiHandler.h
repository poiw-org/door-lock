#ifndef WIFI_HANDLER_H
#define WIFI_HANDLER_H

#include <ESP8266WiFi.h>
#include "FirebaseHandler.h"

class WiFiHandler {
public:
    WiFiHandler(const FirebaseHandler firebaseHandler);
    void begin(const char* ssid, const char* password);
    bool connect();


private:
    const char* ssid;
    const char* password;
    FirebaseHandler firebaseHandler;

};

#endif // WIFI_HANDLER_H
