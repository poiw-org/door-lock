#include "FirebaseHandler.h"
#include "Config.h"

void FirebaseHandler::begin() {
    config.api_key = API_KEY;
    auth.user.email = USER_EMAIL;
    auth.user.password = USER_PASSWORD;
    config.database_url = DATABASE_URL;
    fbdo.setResponseSize(4096);
    Firebase.reconnectWiFi(true);
    Firebase.begin(&config, &auth);
    isBlacklisted = false; // Initialize isBlacklisted
}

void FirebaseHandler::syncBlacklistedKeys() {
    try {
        if (!Firebase.RTDB.get(&fbdo, "/admin/blacklistedKeys")) {
            Serial.println("Failed to download blacklisted keys.");
            return;
        }

        if (fbdo.dataType() == "json") {
            FirebaseJsonArray arr = fbdo.jsonArray();
            for (int i = 0; i < arr.size(); i++) {
                FirebaseJsonData data;
                arr.get(data, i);
                String key = data.to<String>();
                blacklistedKeys.push_back(key);
            }
            Serial.println("Blacklisted keys downloaded.");
        }
    } catch (const std::exception &e) {
        Serial.print("Exception syncing blacklisted keys: ");
        Serial.println(e.what());
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

        unsigned long epochTime = jwtHandler.getEpochTime();
        String sn = jwtHandler.getSn();  // Convert to String for comparison

        if (sn == nfcHandler.getSnFromTag() && 
            jwtHandler.getExpiresAt() > epochTime && 
            jwtHandler.getNotBefore() < epochTime && 
            !isBlacklisted) {
            return true;
        }
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
