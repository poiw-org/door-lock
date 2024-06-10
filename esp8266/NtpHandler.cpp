#include "NtpHandler.h"

NtpHandler::NtpHandler(const char* server) : ntpServer(server), timeClient(ntpUDP, ntpServer) {}

void NtpHandler::begin() {
    timeClient.begin();
}

unsigned long NtpHandler::getTime() {
    try {
        timeClient.update();
        lastKnownTime = timeClient.getEpochTime();

        if(lastKnownTime == 0){
            lastKnownTime = eepromHandler.getLastKnownTime() || 0;
        }
        else{
            eepromHandler.saveLastKnownTime(lastKnownTime);  
        }

        return lastKnownTime;
    } catch (const std::exception &e) {
        Serial.print("Exception getting NTP time: ");
        Serial.println(e.what());
        lastKnownTime = eepromHandler.getLastKnownTime();  // Load from EEPROM if sync fails
        return lastKnownTime;
    }
}
