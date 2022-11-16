#include <ESP8266WiFi.h>
#include <Firebase_ESP_Client.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <addons/RTDBHelper.h>
#include <addons/TokenHelper.h>

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

const char *ssid = STASSID;
const char *password = STAPSK;

// This function returns the UNIX epoch time.
// It is used to validate the keys based on their expiresAtiry/issue date.
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, NTPServer);
unsigned long epochTime;
unsigned long getTime() {
  timeClient.update();
  unsigned long now = timeClient.getEpochTime();
  return now;
}

FirebaseJsonArray blacklistedKeys;
bool connected = false;
bool forceOpen = false;
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

    timeClient.begin();

    connected = true;
  }

  epochTime = getTime();
  String forceOpenURI = "/admin/forceOpen/";
  forceOpenURI.concat(doorId);

  Firebase.RTDB.getBool(&fbdo, forceOpenURI, &forceOpen);
}

volatile bool dataChanged = false;

void streamCallback(FirebaseStream data) {
  Serial.printf(
      "sream path, %s\nevent path, %s\ndata type, %s\nevent type, %s\n\n",
      data.streamPath().c_str(), data.dataPath().c_str(),
      data.dataType().c_str(), data.eventType().c_str());
  printResult(data); // see addons/RTDBHelper.h
  Serial.println();

  // This is the size of stream payload received (current and max value)
  // Max payload size is the payload size under the stream path since the stream
  // connected and read once and will not update until stream reconnection takes
  // place. This max value will be zero as no payload received in case of
  // ESP8266 which BearSSL reserved Rx buffer size is less than the actual
  // stream payload.
  Serial.printf("Received stream payload size: %d (Max. %d)\n\n",
                data.payloadLength(), data.maxPayloadLength());

  // Due to limited of stack memory, do not perform any task that used large
  // memory here especially starting connect to server. Just set this flag and
  // check it status later.
  dataChanged = true;
}
