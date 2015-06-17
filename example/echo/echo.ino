#include <SoftwareSerial.h>
#include <espduino.h>
#include <mqtt.h>
#include "BorgnixAPIv2.h"

BorgnixClient client("voyager.orientsoft.cn", 11883, "12345", "67890", "HiWiFi_orientsoft", "welcome1");

void borgDevCb(char* payload)
{
  debugPort->print(F("borgDevCb: "));
  debugPort->println(payload);
}

void borgIntervalCb()
{
  debugPort->println("Hello!");
  client.BorgDevSend("Hello!");
  client.BorgSimpleSend("Hello", String(12345));
}

void setup() {
  client.BorgDevConnect(borgDevCb);
  client.setInterval(borgIntervalCb, 10000);
}

void loop() {
  client.process();
}

