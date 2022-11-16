#include <NTPClient.h>
#include <WiFiUdp.h>
class Time {

private:
  unsigned long epochTime;
  WiFiUDP ntpUDP;
  NTPClient timeClient;

public:
  Time(String NTPServer) { timeClient = timeClient(ntpUDP, NTPServer); };
  void setEpochTime() {
    timeClient.update();
    this->epochTime = timeClient.getEpochTime();
  };
  void startTime() { this->timeClient.begin(); }
};
