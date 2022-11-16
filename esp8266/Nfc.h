#include <NfcAdapter.h>
#include <PN532.h>
#include <PN532_I2C.h>
#include <Wire.h>

class Nfc {

private:
  PN532_I2C pn532_i2c;
  NfcAdapter nfc;

  char card[1024];
  int cardPayloadLength;
  String snFromTag;

public:
  Nfc() {
    pn532_i2c = PN532_I2C(Wire);
    nfc = NfcAdapter(pn532_i2c);
  };

  char *getCard() { return card; }
  String getSnFromTag() { return snFromTag; }

  bool readCard() {
    if (nfc.tagPresent()) {
      digitalWrite(IDLE_LED, LOW);
      digitalWrite(BUSY_LED, HIGH);

      NfcTag tag = nfc.read();

      if (tag.hasNdefMessage()) // every tag won't have a message
      {

        NdefMessage message = tag.getNdefMessage();

        NdefRecord record = message.getRecord(0);

        cardPayloadLength = record.getPayloadLength();
        byte payload[cardPayloadLength];
        record.getPayload(payload);

        // Force the data into a String (might work depending on the content)
        // Real code should use smarter processing
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

      return false;
    }

    return false;
  }
};
