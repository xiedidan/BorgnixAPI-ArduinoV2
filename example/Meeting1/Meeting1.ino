#include <avr/wdt.h>
#include <MemoryFree.h>
#include <SoftwareSerial.h>
#include <espduino.h>
#include <mqtt.h>
#include <WifiLink.h>
#include <BorgnixAPIv2.h>

BorgnixClient client("z.borgnix.com", 1883, "16799b30-4ae6-11e5-9137-45959ddc0973", "87e09d33665a0543d521a8a69c6233898a3c83d5", "HiWiFi_orientsoft", "welcome1");

int lightStat = 0;

void buttomISR()
{ 
  debugPort->println(F("Buttom Pushed!"));
  client.BorgSimpleSend("buttom", "1");
  client.showFreeMemory();
}

void borgDevCb(char* payload)
{
  lightStat++;
  if (lightStat == 3)
    lightStat = 0;

  switch(lightStat)
  {
    case 0:
    analogWrite(3, 255);
    analogWrite(5, 0);
    analogWrite(6, 0);
    break;
    case 1:
    analogWrite(3, 0);
    analogWrite(5, 255);
    analogWrite(6, 0);
    break;
    case 2:
    analogWrite(3, 0);
    analogWrite(5, 0);
    analogWrite(6, 255);
    break;
    default:
    analogWrite(3, 255);
    analogWrite(5, 0);
    analogWrite(6, 0);
    break;
  }
  
  debugPort->print(F("borgDevCb: "));
  debugPort->println(payload);
  client.showFreeMemory();
}

void borgIntervalCb()
{
  debugPort->println(F("Interval 1"));
  // client.BorgDevSend("Hello!");
  // client.BorgSimpleSend("Hello", String(12345));
  // client.BorgTopicSend("myTopic", String(54321));
}

void setup() {
  pinMode(3, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);

  digitalWrite(3, 255);
  digitalWrite(5, 255);
  digitalWrite(6, 255);
  
  client.BorgDevConnect(borgDevCb);
  client.setInterval(borgIntervalCb, 10000);

  attachInterrupt(0, buttomISR, RISING);
}

void loop() {
  client.process();
}

