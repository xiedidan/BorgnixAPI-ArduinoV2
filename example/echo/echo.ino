#include <avr/wdt.h>
#include <MemoryFree.h>
#include <SoftwareSerial.h>
#include <espduino.h>
#include <mqtt.h>
#include <WifiLink.h>
#include "BorgnixAPIv2.h"

BorgnixClient client("voyager.orientsoft.cn", 11883, "UUID", "TOKEN", "HiWiFi_orientsoft", "welcome1");
int bFlag = 1;

void buttonISR(void)
{
  if (bFlag == 0)
  {
    wifiLink->setLed(1);
    bFlag = 1;
  }
  else
  {
    wifiLink->setLed(0);
    bFlag = 0;
  }
}

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

void borgTimeoutCb()
{
  debugPort->println(F("Hello Timeout!"));
}

void setup() {
  attachInterrupt(0, buttonISR, RISING);
  client.BorgDevConnect(borgDevCb);
  client.setInterval(borgIntervalCb, 10000);
  client.setTimeout(borgTimeoutCb, 5000, 3);
}

void loop() {
  client.process();
}

