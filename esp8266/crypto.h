#include <CustomJWT.h>
#include <ArduinoJson.h>

CustomJWT jwt(JWTDecryptioPass, 256);

String sub; 
String id;
String sn;
unsigned long issuedAt;
unsigned long expiresAt;
unsigned long notBefore;
// Key *key;
bool decodeJWT(){
  jwt.allocateJWTMemory();

  int result = jwt.decodeJWT(card);
  
  if(result == 0){
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
          _sn.replace("0x","");
          _sn.replace(" ",":");
      issuedAt = doc["iat"];
      notBefore = doc["nbf"];
      expiresAt = doc["exp"];

      sub = _sub;
      id = _key;
      sn = _sn;

      return true;
  }
  else return false;
}