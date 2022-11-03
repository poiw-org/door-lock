// Load config
#include "config.h"

// Libraries
#include <Wire.h>
#include <PN532_I2C.h>
#include <PN532.h>
#include <NfcAdapter.h>
#include <CustomJWT.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <Firebase_ESP_Client.h>
#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>
#include <NTPClient.h>
#include <WiFiUdp.h>


FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

const char* ssid     = STASSID;
const char* password = STAPSK;


PN532_I2C pn532_i2c(Wire);
NfcAdapter nfc = NfcAdapter(pn532_i2c);
CustomJWT jwt(JWTDecryptioPass, 256);

// This function returns the UNIX epoch time. 
// It is used to validate the keys based on their expiry/issue date.
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, NTPServer);
unsigned long epochTime; 
unsigned long getTime() {
  timeClient.update();
  unsigned long now = timeClient.getEpochTime();
  return now;
}

bool connected = false;
void connect(){
  if(WiFi.status() != WL_CONNECTED){
    connected = false;
    return;
  }else if(!connected){
    Serial.println("Got new IP address: ");
    Serial.println(WiFi.localIP());

    config.api_key = API_KEY;
    auth.user.email = USER_EMAIL;
    auth.user.password = USER_PASSWORD;
    config.database_url = DATABASE_URL;
    fbdo.setResponseSize(4096);

    Firebase.reconnectWiFi(true);
    Firebase.begin(&config, &auth);

    timeClient.begin();

    connected = true;
  }

  epochTime = getTime();
}

void setup(void) {
  Serial.begin(9600);
  Serial.println("Doora | po/iw hackerspace | https://github.com/poiw-org/doora");

  nfc.begin();

  Serial.print("The device will try to connect to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  pinMode(IDLE_LED, OUTPUT);
  pinMode(BUSY_LED, OUTPUT);
  pinMode(ERROR_LED, OUTPUT);
  pinMode(SUCCESS_LED, OUTPUT);
}


char card[1024];
int cardPayloadLength;
String snFromCard;
bool readCard(){
  if (nfc.tagPresent())
    {
      digitalWrite(IDLE_LED, LOW);
      digitalWrite(BUSY_LED, HIGH);

      NfcTag tag = nfc.read();

      if (tag.hasNdefMessage()) // every tag won't have a message
      {

        NdefMessage message = tag.getNdefMessage();

        NdefRecord record = message.getRecord(0);

        cardPayloadLength = record.getPayloadLength();
        byte payload[cardPayloadLength];
        record.getPayload(payload);

        // Force the data into a String (might work depending on the content)
        // Real code should use smarter processing
        String payloadAsString = "";
        for (int c = 3; c < cardPayloadLength; c++) {
          payloadAsString += (char)payload[c];
        }

        payloadAsString.toCharArray(card, cardPayloadLength-2);
        snFromCard = tag.getUidString();
          snFromCard.replace("0x","");
          snFromCard.replace(" ",":");
        return true;
      }

      return false;
    }

    return false;
}

String sub; 
String key;
String sn;
int issuedAt;
int expires;
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
      String _key(doc["key"]);
      String _sn(doc["sn"]);
          _sn.replace("0x","");
          _sn.replace(" ",":");
      issuedAt = doc["iat"];
      expires = doc["exp"];

      sub = _sub;
      key = _key;
      sn = _sn;

      return true;
  }
  else return false;
}

bool allowedToEnter(){
      if(connected){
        Serial.println(epochTime);
        if(sn == snFromCard && expires > epochTime && issuedAt < epochTime) return true;
        else return false;
      }
      else{
        if(sn == snFromCard) return true;
        else return false;
      }
}

void logEntry(){
  if(!connected) return;

  String entryURI = "/entries/";
  entryURI.concat(doorId);

  FirebaseJson entry;

  entry.set("sub", sub);
  entry.set("key", key);
  entry.set("sn", sn);
  entry.set("iat", issuedAt);
  entry.set("exp", expires);
  entry.set("timestamp/.sv", "timestamp");

  Serial.printf("Writing entry to DB... %s\n", Firebase.RTDB.pushJSON(&fbdo, entryURI, &entry) ? "ok" : fbdo.errorReason().c_str());
}

void logUnauthorizedAttempt(){
  if(!connected) return;

  String failedAuthURI = "/failedAuthorizations/";
  failedAuthURI.concat(doorId);

  FirebaseJson failedAuthorization;

  failedAuthorization.set("sub", sub);
  failedAuthorization.set("key", key);
  failedAuthorization.set("sn", sn);
  failedAuthorization.set("iat", issuedAt);
  failedAuthorization.set("exp", expires);
  failedAuthorization.set("timestamp/.sv", "timestamp");

  if(sn != snFromCard){
      failedAuthorization.set("snFromCard", snFromCard);
      failedAuthorization.set("reason", "SerialNumberMismatch");
  }else if(expires <= epochTime){
      failedAuthorization.set("reason", "KeyExpired");
  }else if(issuedAt > epochTime){
      failedAuthorization.set("reason", "KeyNotYetActivated");
  }else{
      failedAuthorization.set("reason", "Unknown");
  }

  Serial.printf("Writing failed auth to DB... %s\n", Firebase.RTDB.pushJSON(&fbdo, failedAuthURI, &failedAuthorization) ? "ok" : fbdo.errorReason().c_str());
}

void openTheDoor(){
  digitalWrite(BUSY_LED, LOW);
  Serial.println("Door Opened");
  digitalWrite(SUCCESS_LED, HIGH);
  delay(3000);
  digitalWrite(SUCCESS_LED, LOW);
}

void denyEntry(){
  digitalWrite(BUSY_LED, LOW);
  Serial.println("Authorization failed.");
  digitalWrite(ERROR_LED, HIGH);
  delay(3000);
  digitalWrite(ERROR_LED, LOW);
}

void loop(void) {
  digitalWrite(BUSY_LED, LOW);
  digitalWrite(ERROR_LED, LOW);
  digitalWrite(SUCCESS_LED, LOW);

  digitalWrite(IDLE_LED, HIGH);

  connect();

  if(readCard()){
    // Successfuly read the card, proceeding to parse/validate the JWT token.
    if(decodeJWT()){
      // Successfuly decoded the JWT token, proceeding to determine if the door can open with the given key.
      if(allowedToEnter()){
          // The door can open.
          openTheDoor();
          logEntry();
      }
      else{
        // Door opening was blocked due to the rules specified in allowedToEnter()
        denyEntry();
        logUnauthorizedAttempt();
      }
    }
    else{
      // The card was read successfully, but the token could not be authenticated. That probably means that the card did not have a JWT token written on it
      // or that the JWT token has been forged.
      denyEntry();
      Serial.println("Failed to decode JWT.");
    }
  }

  delay(1000);
}



