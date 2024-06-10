#ifndef FIREBASE_HANDLER_H
#define FIREBASE_HANDLER_H

#include <Arduino.h>
#include <Firebase_ESP_Client.h>
#include <ArduinoJson.h>
#include <vector>
#include "JwtHandler.h"
#include "NfcHandler.h"
#include "NtpHandler.h"
#include "EepromHandler.h"

class FirebaseHandler {
public:
    FirebaseHandler(const NtpHandler ntpHandler);
    void begin();
    void syncBlacklistedKeys();
    const std::vector<String>& getBlacklistedKeys();
    bool getForceOpen();
    void resetForceOpen();
    bool allowedToEnter(const JwtHandler& jwtHandler, const NfcHandler& nfcHandler);
    void logEntry(const JwtHandler& jwtHandler, const NfcHandler& nfcHandler, unsigned long epochTime);
    void logUnauthorizedAttempt(const JwtHandler& jwtHandler, const NfcHandler& nfcHandler, unsigned long epochTime);

private:
    NtpHandler ntpHandler;
    EepromHandler eepromHandler;
    FirebaseData fbdo;
    FirebaseAuth auth;
    FirebaseConfig config;
    std::vector<String> blacklistedKeys;
    bool isBlacklisted;
};

#endif // FIREBASE_HANDLER_H
