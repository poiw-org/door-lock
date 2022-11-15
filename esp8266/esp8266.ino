// Load config
#include "key.h"
#include "config.h"
#include "networking.h"
#include "nfc.h"
#include "crypto.h"
#include "logic.h"


void setup(void) {
  Serial.begin(9600);
  Serial.println("Doora | po/iw hackerspace | https://github.com/poiw-org/doora");

  nfc.begin();

  Serial.print("The device will try to connect to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  pinMode(D0, OUTPUT);
  pinMode(SPEAKER, OUTPUT);
  pinMode(IDLE_LED, OUTPUT);
  pinMode(BUSY_LED, OUTPUT);
  pinMode(ERROR_LED, OUTPUT);
  pinMode(SUCCESS_LED, OUTPUT);
}


void loop(void) {
  digitalWrite(BUSY_LED, LOW);
  digitalWrite(ERROR_LED, LOW);
  digitalWrite(SUCCESS_LED, LOW);
  digitalWrite(IDLE_LED, LOW);


  connect();

  if(forceOpen) forceOpenTheDoor();
  else if(readCard()){
     digitalWrite(BUSY_LED, HIGH);
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

  digitalWrite(IDLE_LED, HIGH);
  delay(150);
}
