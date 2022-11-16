// Load config
#include "key.h"
#include "config.h"
#include "networking.h"
#include "nfc.h"
#include "crypto.h"
#include "logic.h"
#include "Wifi.h"

void setup(void) {
  Serial.begin(9600);
  Serial.println("Doora | po/iw hackerspace | https://github.com/poiw-org/doora");

  nfc.begin();

  Serial.print("Trying to connect to ");
  Serial.println(ssid);

	Wifi wifi(ssid, password, NTPServer);
	Nfc nfc();

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


  wifi.connect();

  if(forceOpen) forceOpenTheDoor();
  else if(nfc.readCard()){
     digitalWrite(BUSY_LED, HIGH);
    // Successfuly read the card, proceeding to parse/validate the JWT token.
		 if(key.setFields(nfc.getCard(),nfc.getSnFromTag()){
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
