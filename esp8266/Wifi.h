#include "Time.h"
#include <ESP8266WiFi.h>

class Wifi {
private:
  char *ssid;
  char *password;
  bool connected = false;
  Time time;

public:
  Wifi(char *STASSID, char *STAPSK, String NTPServer) {
    ssid = STASSID;
    password = STAPSK;
    time = Time(NTPServer);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
  };

  void connect() {
    if (WiFi.status() != WL_CONNECTED) {
      connected = false;
      digitalWrite(ERROR_LED, HIGH);
      return;
    } else if (!connected) {
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
    }

    time.setEpochTime();

    String forceOpenURI = "/admin/forceOpen/";
    forceOpenURI.concat(doorId);

    Firebase.RTDB.getBool(&fbdo, forceOpenURI, &forceOpen);
  }
}
