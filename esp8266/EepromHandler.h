#ifndef EEPROM_HANDLER_H
#define EEPROM_HANDLER_H

#include <Arduino.h>
#include <EEPROM.h>
#include <vector>

class EepromHandler {
public:
    void begin();
    void saveBlacklistedKeys(const std::vector<String>& keys);
    std::vector<String> getBlacklistedKeys();
    void saveLastKnownTime(unsigned long time);
    unsigned long getLastKnownTime();

private:
    static const int EEPROM_SIZE = 512;
    static const int TIME_ADDR = 0;
    static const int KEYS_ADDR = sizeof(unsigned long);
};

#endif // EEPROM_HANDLER_H
