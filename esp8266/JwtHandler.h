#ifndef JWT_HANDLER_H
#define JWT_HANDLER_H

#include <Arduino.h>
#include <CustomJWT.h>
#include <ArduinoJson.h>

class JwtHandler {
public:
    JwtHandler(const char* decryptionPass);
    bool decodeJWT(const char* jwt);
    const char* getSub() const;
    const char* getId() const;
    const char* getSn() const;
    unsigned long getIssuedAt() const;
    unsigned long getExpiresAt() const;
    unsigned long getNotBefore() const;

private:
    CustomJWT jwt;
    String sub;
    String id;
    String sn;
    unsigned long issuedAt;
    unsigned long expiresAt;
    unsigned long notBefore;
};

#endif // JWT_HANDLER_H
