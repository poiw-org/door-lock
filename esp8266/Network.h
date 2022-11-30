#include "Time.h"
#include <ESP8266WiFi.h>

class Wifi {
private:
  char *ssid;
  char *password;
  bool connected = false;
  // Time time;

public:
  Wifi(char *STASSID, char *STAPSK, String NTPServer) {
    ssid = STASSID;
    password = STAPSK;
    time = Time(NTPServer);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
  };

  bool checkConnectivity() {
    if (WiFi.status() != WL_CONNECTED) {
      connected = false;
      digitalWrite(ERROR_LED, HIGH);
      return;
    }

    // time.setEpochTime();

    // String forceOpenURI = "/admin/forceOpen/";
    // forceOpenURI.concat(doorId);

    // Firebase.RTDB.getBool(&fbdo, forceOpenURI, &forceOpen);
  }
}
