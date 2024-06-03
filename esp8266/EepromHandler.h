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

private:
    static const int EEPROM_SIZE = 512;
};

#endif // EEPROM_HANDLER_H
