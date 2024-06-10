#include "EepromHandler.h"

void EepromHandler::begin() {
    EEPROM.begin(EEPROM_SIZE);
}

void EepromHandler::saveBlacklistedKeys(const std::vector<String>& keys) {
    int addr = KEYS_ADDR;
    for (const auto& key : keys) {
        for (int i = 0; i < key.length(); i++) {
            EEPROM.write(addr++, key[i]);
            if (addr >= EEPROM_SIZE) break;
        }
        EEPROM.write(addr++, '\0'); // Null terminator
        if (addr >= EEPROM_SIZE) break;
    }
    EEPROM.commit();
}

std::vector<String> EepromHandler::getBlacklistedKeys() {
    Serial.println("Reading blacklisted keys from EEPROM");

    std::vector<String> keys;
    int addr = KEYS_ADDR;
    while (addr < EEPROM_SIZE) {
        String key;
        char c = EEPROM.read(addr++);
        while (c != '\0' && addr < EEPROM_SIZE) {
            key += c;
            c = EEPROM.read(addr++);
        }
        if (key.length() > 0) keys.push_back(key);
        if (addr >= EEPROM_SIZE) break;
    }
    return keys;
}

void EepromHandler::saveLastKnownTime(unsigned long time) {
    EEPROM.put(TIME_ADDR, time);
    EEPROM.commit();
}

unsigned long EepromHandler::getLastKnownTime() {
    Serial.println("Reading last known time from EEPROM");
    unsigned long time;
    EEPROM.get(TIME_ADDR, time);
    return time;
}
