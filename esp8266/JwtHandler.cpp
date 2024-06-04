#include "JwtHandler.h"

JwtHandler::JwtHandler(const char* decryptionPass) : jwt((char*)decryptionPass, 256) {}

bool JwtHandler::decodeJWT(const char* token) {
    try {
        jwt.allocateJWTMemory();

        int result = jwt.decodeJWT((char*)token);

        if (result == 0) {
            DynamicJsonDocument doc(jwt.payloadLength);
            DeserializationError error = deserializeJson(doc, jwt.payload);

            if (error) {
                Serial.print(F("deserializeJson() failed: "));
                Serial.println(error.f_str());
                return false;
            }

            sub = doc["sub"].as<String>();
            id = doc["jti"].as<String>();
            sn = doc["aud"].as<String>();
            sn.replace("0x", "");
            sn.replace(" ", ":");
            issuedAt = doc["iat"];
            notBefore = doc["nbf"];
            expiresAt = doc["exp"];
            return true;
        }
    } catch (const std::exception &e) {
        Serial.print("Exception decoding JWT: ");
        Serial.println(e.what());
    }
    return false;
}

const char* JwtHandler::getSub() const {
    return sub.c_str();
}

const char* JwtHandler::getId() const {
    return id.c_str();
}

const char* JwtHandler::getSn() const {
    return sn.c_str();
}

unsigned long JwtHandler::getIssuedAt() const {
    return issuedAt;
}

unsigned long JwtHandler::getExpiresAt() const {
    return expiresAt;
}

unsigned long JwtHandler::getNotBefore() const {
    return notBefore;
}
