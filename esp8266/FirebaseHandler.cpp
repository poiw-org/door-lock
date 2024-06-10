#include "FirebaseHandler.h"
#include "Config.h"
#include <addons/TokenHelper.h>

FirebaseHandler::FirebaseHandler(NtpHandler ntpHandler) : ntpHandler(ntpHandler) {}

void FirebaseHandler::begin() {
    config.api_key = API_KEY;
    auth.user.email = USER_EMAIL;
    auth.user.password = USER_PASSWORD;
    config.database_url = DATABASE_URL;
    fbdo.setResponseSize(4096);
    Firebase.reconnectNetwork(false);
    config.token_status_callback = tokenStatusCallback;
    Firebase.begin(&config, &auth);
    isBlacklisted = false; // Initialize isBlacklisted
}

void FirebaseHandler::syncBlacklistedKeys() {
    if (Firebase.RTDB.getString(&fbdo, F("/blacklisted_keys"))) {
        String keys = fbdo.stringData();
        blacklistedKeys.clear();
        int start = 0;
        int end = keys.indexOf(',');

        while (end != -1) {
            blacklistedKeys.push_back(keys.substring(start, end));
            start = end + 1;
            end = keys.indexOf(',', start);
        }
        blacklistedKeys.push_back(keys.substring(start));
        eepromHandler.saveBlacklistedKeys(blacklistedKeys);  // Save to EEPROM
    } else {
        blacklistedKeys = eepromHandler.getBlacklistedKeys();  // Load from EEPROM if sync fails
    }
}

const std::vector<String>& FirebaseHandler::getBlacklistedKeys() {
    return blacklistedKeys;
}

bool FirebaseHandler::getForceOpen() {
    bool forceOpen = false;
    try {
        String forceOpenURI = "/admin/forceOpen/";
        forceOpenURI.concat(doorId);
        Firebase.RTDB.getBool(&fbdo, forceOpenURI, &forceOpen);
    } catch (const std::exception &e) {
        Serial.print("Exception getting force open: ");
        Serial.println(e.what());
    }
    return forceOpen;
}

void FirebaseHandler::resetForceOpen() {
    try {
        String forceOpenURI = "/admin/forceOpen/";
        forceOpenURI.concat(doorId);
        Firebase.RTDB.setBool(&fbdo, forceOpenURI, false);
    } catch (const std::exception &e) {
        Serial.print("Exception resetting force open: ");
        Serial.println(e.what());
    }
}

bool FirebaseHandler::allowedToEnter(const JwtHandler& jwtHandler, const NfcHandler& nfcHandler) {
    try {
        String blackListedURI = "/admin/blacklistedKeys/";
        blackListedURI.concat(jwtHandler.getId());
        Firebase.RTDB.getBool(&fbdo, blackListedURI, &isBlacklisted);

        if(isBlacklisted) {
            Serial.println("Key is blacklisted.");
            return false;
        }

        unsigned long epochTime = ntpHandler.getTime();

        if(jwtHandler.getExpiresAt() <= epochTime || jwtHandler.getNotBefore() > epochTime || isBlacklisted) {
            Serial.println("Key has expired or is not yet activated.");
            return false;
        }



        String sn = jwtHandler.getSn();  // Convert to String for comparison

        if (sn != nfcHandler.getSnFromTag()) {
            Serial.println("Serial number mismatch.");
            return false;
        }

        return true;
    
    } catch (const std::exception &e) {
        Serial.print("Exception checking entry permission: ");
        Serial.println(e.what());
    }
    return false;
}

void FirebaseHandler::logEntry(const JwtHandler& jwtHandler, const NfcHandler& nfcHandler, unsigned long epochTime) {
    try {
        String entryURI = "/entries/";
        entryURI.concat(doorId);

        FirebaseJson entry;
        entry.set("user", jwtHandler.getSub());
        entry.set("id", jwtHandler.getId());
        entry.set("serialNumber", jwtHandler.getSn());
        entry.set("issuedAt", jwtHandler.getIssuedAt());
        entry.set("expiresAt", jwtHandler.getExpiresAt());
        entry.set("notBefore", jwtHandler.getNotBefore());
        entry.set("timestamp", epochTime);

        Serial.printf("Writing entry to DB... %s\n", Firebase.RTDB.pushJSON(&fbdo, entryURI, &entry) ? "ok" : fbdo.errorReason().c_str());
    } catch (const std::exception &e) {
        Serial.print("Exception logging entry: ");
        Serial.println(e.what());
    }
}

void FirebaseHandler::logUnauthorizedAttempt(const JwtHandler& jwtHandler, const NfcHandler& nfcHandler, unsigned long epochTime) {
    try {
        String failedAuthURI = "/failedAuthorizations/";
        failedAuthURI.concat(doorId);

        FirebaseJson failedAuthorization;
        failedAuthorization.set("user", jwtHandler.getSub());
        failedAuthorization.set("id", jwtHandler.getId());
        failedAuthorization.set("serialNumber", jwtHandler.getSn());
        failedAuthorization.set("issuedAt", jwtHandler.getIssuedAt());
        failedAuthorization.set("notBefore", jwtHandler.getNotBefore());
        failedAuthorization.set("expiresAt", jwtHandler.getExpiresAt());
        failedAuthorization.set("timestamp", epochTime);

        String sn = jwtHandler.getSn();  // Convert to String for comparison

        if (sn != nfcHandler.getSnFromTag()) {
            failedAuthorization.set("serialNumberFromTag", nfcHandler.getSnFromTag());
            failedAuthorization.set("reason", "SerialNumberMismatch");
        } else if (isBlacklisted) {
            failedAuthorization.set("reason", "KeyBlacklisted");
        } else if (jwtHandler.getExpiresAt() <= epochTime) {
            failedAuthorization.set("reason", "KeyExpired");
        } else if (jwtHandler.getNotBefore() > epochTime) {
            failedAuthorization.set("reason", "KeyNotYetActivated");
        } else {
            failedAuthorization.set("reason", "Unknown");
        }

        Serial.printf("Writing failed auth to DB... %s\n", Firebase.RTDB.pushJSON(&fbdo, failedAuthURI, &failedAuthorization) ? "ok" : fbdo.errorReason().c_str());
    } catch (const std::exception &e) {
        Serial.print("Exception logging unauthorized attempt: ");
        Serial.println(e.what());
    }
}
