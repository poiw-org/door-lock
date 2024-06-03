#include "WiFiHandler.h"

void WiFiHandler::begin(const char* ssid, const char* password) {
    this->ssid = ssid;
    this->password = password;
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
}

bool WiFiHandler::connect() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("Connecting to WiFi...");
        unsigned long startAttemptTime = millis();
        while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 5000) {
            delay(100);
            Serial.print(".");
        }
        Serial.println();

        if (WiFi.status() == WL_CONNECTED) {
            Serial.println("Connected to WiFi");
            Serial.print("IP Address: ");
            Serial.println(WiFi.localIP());
            return true;
        } else {
            Serial.println("Failed to connect to WiFi");
            return false;
        }
    }
    return true;
}
