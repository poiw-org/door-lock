#include "EepromHandler.h"

void EepromHandler::begin() {
    EEPROM.begin(EEPROM_SIZE);
}

void EepromHandler::saveBlacklistedKeys(const std::vector<String>& keys) {
    try {
        int addr = 0;
        for (const auto& key : keys) {
            for (int i = 0; i < key.length(); i++) {
                EEPROM.write(addr++, key[i]);
                if (addr >= EEPROM_SIZE) break;
            }
            EEPROM.write(addr++, '\0'); // Null terminator
            if (addr >= EEPROM_SIZE) break;
        }
        EEPROM.commit();
    } catch (const std::exception &e) {
        Serial.print("Exception saving blacklisted keys to EEPROM: ");
        Serial.println(e.what());
    }
}

std::vector<String> EepromHandler::getBlacklistedKeys() {
    std::vector<String> keys;
    try {
        int addr = 0;
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
    } catch (const std::exception &e) {
        Serial.print("Exception reading blacklisted keys from EEPROM: ");
        Serial.println(e.what());
    }
    return keys;
}
