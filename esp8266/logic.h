bool isBlacklisted;
bool allowedToEnter(){
      if(connected){        
        String blackListedURI = "/admin/blacklistedKeys/";
        blackListedURI.concat(id);
        Firebase.RTDB.getBool(&fbdo, blackListedURI, &isBlacklisted);

        if(sn == snFromTag && expiresAt > epochTime && notBefore < epochTime && (isBlacklisted == NULL || isBlacklisted == false)) return true;
        else return false;
      }
      else{
        if(sn == snFromTag) return true;
        else return false;
      }
}


void logEntry(){
  if(!connected) return;

  String entryURI = "/entries/";
  entryURI.concat(doorId);

  FirebaseJson entry;

  entry.set("user", sub);
  entry.set("id", id);
  entry.set("serialNumber", sn);
  entry.set("issuedAt", issuedAt);
  entry.set("expiresAt", expiresAt);
  entry.set("notBefore", notBefore);
  entry.set("timestamp", epochTime);

  Serial.printf("Writing entry to DB... %s\n", Firebase.RTDB.pushJSON(&fbdo, entryURI, &entry) ? "ok" : fbdo.errorReason().c_str());
}

void logUnauthorizedAttempt(){
  if(!connected) return;

  String failedAuthURI = "/failedAuthorizations/";
  failedAuthURI.concat(doorId);

  FirebaseJson failedAuthorization;

  failedAuthorization.set("user", sub);
  failedAuthorization.set("id", id);
  failedAuthorization.set("serialNumber", sn);
  failedAuthorization.set("issuedAt", issuedAt);
  failedAuthorization.set("notBefore", notBefore);
  failedAuthorization.set("expiresAt", expiresAt);
  failedAuthorization.set("timestamp", epochTime);

  if(sn != snFromTag){
      failedAuthorization.set("serialNumberFromTag", snFromTag);
      failedAuthorization.set("reason", "SerialNumberMismatch");
  }else if(isBlacklisted == true){
      failedAuthorization.set("reason", "KeyBlacklisted");
  }else if(expiresAt <= epochTime){
      failedAuthorization.set("reason", "KeyExpired");
  }else if(notBefore > epochTime){
      failedAuthorization.set("reason", "KeyNotYetActivated");
  }else if(notBefore > epochTime){
      failedAuthorization.set("reason", "IssuingDateInTheFuture");
  }else{
      failedAuthorization.set("reason", "Unknown");
  }

  Serial.printf("Writing failed auth to DB... %s\n", Firebase.RTDB.pushJSON(&fbdo, failedAuthURI, &failedAuthorization) ? "ok" : fbdo.errorReason().c_str());
}

void openTheDoor(){
  digitalWrite(BUSY_LED, LOW);
  Serial.println("Door Opened");
  digitalWrite(SUCCESS_LED, HIGH);
  digitalWrite(D0, HIGH);
  digitalWrite(SPEAKER, HIGH);
  delay(300);
  digitalWrite(SPEAKER, LOW);
  delay(1000);
  digitalWrite(D0, LOW);
  digitalWrite(SUCCESS_LED, LOW);
}

void forceOpenTheDoor(){
  forceOpen = false;
  
  String forceOpenURI = "/admin/forceOpen/";
  forceOpenURI.concat(doorId);
  Firebase.RTDB.setBool(&fbdo, forceOpenURI, false);

  digitalWrite(BUSY_LED, LOW);
  Serial.println("Door Opened");
  digitalWrite(SUCCESS_LED, HIGH);
  digitalWrite(D0, HIGH);
  digitalWrite(SPEAKER, HIGH);
  delay(1000);
  digitalWrite(SPEAKER, LOW);
  delay(5000);
  digitalWrite(D0, LOW);
  digitalWrite(SUCCESS_LED, LOW);
}

void denyEntry(){
  digitalWrite(BUSY_LED, LOW);
  Serial.println("Authorization failed.");
  digitalWrite(ERROR_LED, HIGH);

  for(int i=0; i<3; i++){
    digitalWrite(SPEAKER, HIGH);
    delay(300);
    digitalWrite(SPEAKER, LOW);
    delay(200);
  }

  delay(3000);
  digitalWrite(ERROR_LED, LOW);
}
