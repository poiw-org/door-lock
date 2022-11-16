#include <ArduinoJson.h>
#include <CustomJWT.h>

class Key {

private:
  CustomJWT jwt;

  String sub;
  String id;
  String sn;
  String snFromTag;
  unsigned long issuedAt;
  unsigned long expiresAt;
  unsigned long notBefore;

public:
  Key() { jwt = jwt(JWTDecryptioPass, 256); };
  ~Key() {
    sub = nullptr;
    id = nullptr;
    sn = nullptr;
    snFromTag = nullptr;
    issuedAt = 0;
    expiresAt = 0;
    notBefore = 0;
  };

  bool setFields(char *card, String snFromTag) {
    this->snFromTag = snFromTag;
    return decodeJWT(card);
  }

  bool decodeJWT(char *card) {
    jwt.allocateJWTMemory();

    int result = jwt.decodeJWT(card);

    if (result == 0) {
      DynamicJsonDocument doc(cardPayloadLength);
      // Deserialize the JSON document
      DeserializationError error = deserializeJson(doc, jwt.payload);

      // Test if parsing succeeds.
      if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return false;
      }

      String _sub(doc["sub"]);
      String _key(doc["jti"]);
      String _sn(doc["aud"]);
      _sn.replace("0x", "");
      _sn.replace(" ", ":");
      issuedAt = doc["iat"];
      notBefore = doc["nbf"];
      expiresAt = doc["exp"];

      sub = _sub;
      id = _key;
      sn = _sn;

      return true;
    } else
      return false;
  }
}
