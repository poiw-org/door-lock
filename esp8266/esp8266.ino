#include "Config.h"
#include "WiFiHandler.h"
#include "NfcHandler.h"
#include "FirebaseHandler.h"
#include "JwtHandler.h"
#include "NtpHandler.h"
#include "EepromHandler.h"

WiFiHandler wifiHandler;
NfcHandler nfcHandler;
FirebaseHandler firebaseHandler;
JwtHandler jwtHandler(JWTDecryptioPass);
NtpHandler ntpHandler(NTPServer);
EepromHandler eepromHandler;

bool forceOpen = false;
unsigned long epochTime;
bool connected = false;

void setup() {
    Serial.begin(9600);
    Serial.println("Doora | po/iw hackerspace | https://github.com/poiw-org/doora");

    delay(1000);

    nfcHandler.begin();
    wifiHandler.begin(STASSID, STAPSK);
    firebaseHandler.begin();
    ntpHandler.begin();
    eepromHandler.begin();

    pinMode(DOOR_PIN, OUTPUT);
    pinMode(SPEAKER, OUTPUT);
    pinMode(BUSY_LED, OUTPUT);
    pinMode(ERROR_LED, OUTPUT);
    pinMode(SUCCESS_LED, OUTPUT);

    digitalWrite(DOOR_PIN, LOW);
    digitalWrite(SPEAKER, LOW);
    digitalWrite(BUSY_LED, LOW);
    digitalWrite(ERROR_LED, LOW);
    digitalWrite(SUCCESS_LED, LOW);
}

void loop() {
    digitalWrite(BUSY_LED, LOW);
    digitalWrite(ERROR_LED, LOW);
    digitalWrite(SUCCESS_LED, LOW);

    wifiHandler.connect();

    if (forceOpen) {
        firebaseHandler.resetForceOpen();
        digitalWrite(DOOR_PIN, HIGH);
        delay(5000);  // Keep door open for 5 seconds
        digitalWrite(DOOR_PIN, LOW);
        forceOpen = false;
    } else if (nfcHandler.readCard()) {
        digitalWrite(BUSY_LED, HIGH);
        if (jwtHandler.decodeJWT(nfcHandler.getCard())) {
            if (firebaseHandler.allowedToEnter(jwtHandler, nfcHandler)) {
                digitalWrite(DOOR_PIN, HIGH);
                digitalWrite(SUCCESS_LED, HIGH);
                delay(5000);  // Keep door open for 5 seconds
                digitalWrite(DOOR_PIN, LOW);
                digitalWrite(SUCCESS_LED, LOW);
                firebaseHandler.logEntry(jwtHandler, nfcHandler, ntpHandler.getTime());
            } else {
                digitalWrite(ERROR_LED, HIGH);
                delay(1000);  // Indicate error for 1 second
                digitalWrite(ERROR_LED, LOW);
                firebaseHandler.logUnauthorizedAttempt(jwtHandler, nfcHandler, ntpHandler.getTime());
            }
        } else {
            digitalWrite(ERROR_LED, HIGH);
            delay(1000);  // Indicate error for 1 second
            digitalWrite(ERROR_LED, LOW);
            Serial.println("Failed to decode JWT.");
        }
        digitalWrite(BUSY_LED, LOW);
    }

    delay(150);
    digitalWrite(IDLE_LED, LOW);
}
