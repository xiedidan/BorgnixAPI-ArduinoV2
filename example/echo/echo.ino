#include <avr/wdt.h>
#include <MemoryFree.h>
#include <SoftwareSerial.h>
#include <espduino.h>
#include <mqtt.h>
#include "BorgnixAPIv2.h"

BorgnixClient client("voyager.orientsoft.cn", 11883, "UUID", "TOKEN", "HiWiFi_orientsoft", "welcome1");

void borgDevCb(char* payload)
{
  debugPort->print(F("borgDevCb: "));
  debugPort->println(payload);
  client.showFreeMemory();
}

void borgIntervalCb()
{
  debugPort->println(F("Hello!"));
  // client.BorgDevSend("Hello!");
  client.BorgSimpleSend("Hello", String(12345));
  // client.BorgTopicSend("myTopic", String(54321));
}

void setup() {
  client.BorgDevConnect(borgDevCb);
  client.setInterval(borgIntervalCb, 10000);
}

void loop() {
  client.process();
}

