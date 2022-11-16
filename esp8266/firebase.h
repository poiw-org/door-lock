#include <Firebase_ESP_Client.h>
#include "Key.h"

class Firebase {

private:
  FirebaseData fbdo;
  String snFromTag;
  unsigned long epochTime;
  bool isBlacklisted;
  bool connected;
  String doorId;
  Key key;

public:
  Firebase(bool connected, FirebaseData fbdo, String snFromTag,
           unsigned long epochTime, String doorId, Key key) {
    this->fbdo = fbdo;
    this->snFromTag = snFromTag;
    this->epochTime = epochTime;
    this->connected = connected;
    this->key = key;
  };

  bool allowedToEnter() {
    if (connected) {
      String blackListedURI = "/admin/blacklistedKeys/";
      blackListedURI.concat(id);
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

  void logEntry() {
    if (!connected)
      return;

    String entryURI = "/entries/";
    entryURI.concat(doorId);

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

  void logUnauthorizedAttempt() {
    if (!connected)
      return;

    String failedAuthURI = "/failedAuthorizations/";
    failedAuthURI.concat(doorId);

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
}
