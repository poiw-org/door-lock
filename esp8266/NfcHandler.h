#ifndef NFC_HANDLER_H
#define NFC_HANDLER_H

#include <Wire.h>
#include <PN532_I2C.h>
#include <NfcAdapter.h>
#include <Arduino.h> // Include Arduino for pin definitions

class NfcHandler {
public:
    NfcHandler();
    void begin();
    bool readCard();
    const char* getCard();
    void openTheDoor();
    void denyEntry();
    String getSnFromTag() const;

private:
    PN532_I2C pn532_i2c;
    NfcAdapter nfc;
    char card[1024];
    String snFromTag;
};

#endif // NFC_HANDLER_H
