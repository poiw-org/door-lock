#include "Door.h"
#include "Key.h"
#include <Firebase_ESP_Client.h>

class Controller {

private:
  FirebaseData fbdo;
  String snFromTag;
  unsigned long epochTime;
  bool isBlacklisted;
  bool connected;
  Door door = nullptr;

public:
  Controller(bool connected, FirebaseData fbdo, String snFromTag,
             unsigned long epochTime, Door door) {
    this->fbdo = fbdo;
    this->epochTime = epochTime;
    this->connected = connected;
    this->door = door;
  };

  void setDoor(Door door) { this->door = door; };

  void validate(Key key) {
    if (this->door == nullptr) {
      Serial.printf("No door controlled by firebase");
      return;
    }

    if (this->allowedToEnter(key)) {
      this->door.open();
      this->logEntry(key);
    } else {
      this->logUnauthorizedAttempt(key);
      this->door.error();
    }

    return;
  }

  void connect() {
    Serial.println("Got new IP address: ");
    Serial.println(WiFi.localIP());

    config.api_key = API_KEY;
    auth.user.email = USER_EMAIL;
    auth.user.password = USER_PASSWORD;
    config.database_url = DATABASE_URL;
    fbdo.setResponseSize(4096);

    Firebase.reconnectWiFi(true);
    config.token_status_callback =
        tokenStatusCallback; // see addons/TokenHelper.h
    // config.cert.data = rootCACert;

    Firebase.begin(&config, &auth);

    time.startTime();

    connected = true;
    // time.setEpochTime();

    // String forceOpenURI = "/admin/forceOpen/";
    // forceOpenURI.concat(doorId);

    // Firebase.RTDB.getBool(&fbdo, forceOpenURI, &forceOpen);
  }

private:
  void setSnFromTag(String snFromTag) { this->snFromTag = snFromTag; };
  bool allowedToEnter(Key key) {
    if (connected) {
      String blackListedURI = "/admin/blacklistedKeys/";
      blackListedURI.concat(key.id);
      Firebase.RTDB.getBool(&fbdo, blackListedURI, &isBlacklisted);

      if (key.sn == snFromTag && key.expiresAt > epochTime &&
          key.notBefore < epochTime &&
          (isBlacklisted == NULL || isBlacklisted == false))
        return true;
      else
        return false;
    } else {
      if (key.sn == snFromTag)
        return true;
      else
        return false;
    }
  }

  void logEntry(Key key) {
    if (!connected)
      return;

    String entryURI = "/entries/";
    entryURI.concat(this->door.id);

    FirebaseJson entry;

    entry.set("user", key.sub);
    entry.set("id", key.id);
    entry.set("serialNumber", key.sn);
    entry.set("issuedAt", key.issuedAt);
    entry.set("expiresAt", key.expiresAt);
    entry.set("notBefore", key.notBefore);
    entry.set("timestamp", epochTime);

    Serial.printf("Writing entry to DB... %s\n",
                  Firebase.RTDB.pushJSON(&fbdo, entryURI, &entry)
                      ? "ok"
                      : fbdo.errorReason().c_str());
  }

  void logForceOpenDoor() {
    String forceOpenURI = "/admin/forceOpen/";
    forceOpenURI.concat(Key.id);
    Firebase.RTDB.setBool(&fbdo, forceOpenURI, false);
  }

  void logUnauthorizedAttempt(Key key) {
    if (!connected)
      return;

    String failedAuthURI = "/failedAuthorizations/";
    failedAuthURI.concat(this->door.id);

    FirebaseJson failedAuthorization;

    failedAuthorization.set("user", key.sub);
    failedAuthorization.set("id", key.id);
    failedAuthorization.set("serialNumber", key.sn);
    failedAuthorization.set("issuedAt", key.issuedAt);
    failedAuthorization.set("notBefore", key.notBefore);
    failedAuthorization.set("expiresAt", key.expiresAt);
    failedAuthorization.set("timestamp", epochTime);

    if (key.sn != snFromTag) {
      failedAuthorization.set("serialNumberFromTag", snFromTag);
      failedAuthorization.set("reason", "SerialNumberMismatch");
    } else if (isBlacklisted == true) {
      failedAuthorization.set("reason", "KeyBlacklisted");
    } else if (key.expiresAt <= key.epochTime) {
      failedAuthorization.set("reason", "KeyExpired");
    } else if (key.notBefore > epochTime) {
      failedAuthorization.set("reason", "KeyNotYetActivated");
    } else if (key.notBefore > epochTime) {
      failedAuthorization.set("reason", "IssuingDateInTheFuture");
    } else {
      failedAuthorization.set("reason", "Unknown");
    }

    Serial.printf(
        "Writing failed auth to DB... %s\n",
        Firebase.RTDB.pushJSON(&fbdo, failedAuthURI, &failedAuthorization)
            ? "ok"
            : fbdo.errorReason().c_str());
  }
};
