#include "NfcHandler.h"
#include "Config.h" // Include your pin definitions from a config file if available

NfcHandler::NfcHandler() : pn532_i2c(Wire), nfc(pn532_i2c) {
}

void NfcHandler::begin() {
    nfc.begin();
}

bool NfcHandler::readCard() {
    try {
        if (nfc.tagPresent()) {
            digitalWrite(BUSY_LED, HIGH);

            NfcTag tag = nfc.read();

            if (tag.hasNdefMessage()) {
                NdefMessage message = tag.getNdefMessage();
                NdefRecord record = message.getRecord(0);

                int cardPayloadLength = record.getPayloadLength();
                byte payload[cardPayloadLength];
                record.getPayload(payload);

                String payloadAsString = "";
                for (int c = 3; c < cardPayloadLength; c++) {
                    payloadAsString += (char)payload[c];
                }
                payloadAsString.toCharArray(card, cardPayloadLength - 2);

                snFromTag = tag.getUidString();
                snFromTag.replace("0x", "");
                snFromTag.replace(" ", ":");
                snFromTag.toLowerCase();
                return true;
            }
        }
    } catch (const std::exception &e) {
        Serial.print("Exception reading card: ");
        Serial.println(e.what());
    }
    return false;
}

const char* NfcHandler::getCard() {
    return card;
}

void NfcHandler::openTheDoor() {
    digitalWrite(BUSY_LED, LOW);
    Serial.println("Door Opened");
    digitalWrite(SUCCESS_LED, HIGH);
    digitalWrite(DOOR_PIN, HIGH);
    digitalWrite(SPEAKER, HIGH);
    delay(300);
    digitalWrite(SPEAKER, LOW);
    delay(1000);
    digitalWrite(DOOR_PIN, LOW);
    digitalWrite(SUCCESS_LED, LOW);
}

void NfcHandler::denyEntry() {
    digitalWrite(BUSY_LED, LOW);
    Serial.println("Authorization failed.");
    digitalWrite(ERROR_LED, HIGH);

    for (int i = 0; i < 3; i++) {
        digitalWrite(SPEAKER, HIGH);
        delay(300);
        digitalWrite(SPEAKER, LOW);
        delay(200);
    }

    delay(3000);
    digitalWrite(ERROR_LED, LOW);
}

String NfcHandler::getSnFromTag() const {
    return snFromTag;
}
