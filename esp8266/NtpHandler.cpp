#include "NtpHandler.h"

NtpHandler::NtpHandler(const char* server) : ntpServer(server), timeClient(ntpUDP, ntpServer) {}

void NtpHandler::begin() {
    timeClient.begin();
}

unsigned long NtpHandler::getTime() {
    try {
        timeClient.update();
        return timeClient.getEpochTime();
    } catch (const std::exception &e) {
        Serial.print("Exception getting NTP time: ");
        Serial.println(e.what());
    }
    return 0;
}
