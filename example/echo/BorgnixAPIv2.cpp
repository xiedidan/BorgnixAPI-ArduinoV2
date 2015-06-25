/*
 BorgnixAPIv2.cpp - Borgnix Arduino API for ESP8266 Adaptor Shield
 Xie Di, 2015.05.21
 http://www.borgnix.com
*/

#include <avr/wdt.h>
#include <Arduino.h>
#include <MemoryFree.h>
#include <SoftwareSerial.h>
#include <espduino.h>
#include <mqtt.h>
#include "BorgnixAPIv2.h"

SoftwareSerial* debugPort;
ESP* esp;
MQTT* mqtt;
boolean wifiConnectFlag;
boolean mqttConnectFlag;

char* BorgnixClient::host = NULL;
uint16_t BorgnixClient::port = 1883;
char* BorgnixClient::uuid = NULL;
char* BorgnixClient::token = NULL;
char* BorgnixClient::inTopic = NULL;
char* BorgnixClient::outTopic = NULL;
char* BorgnixClient::wifiName = NULL;
char* BorgnixClient::wifiPass = NULL;
BorgDevCB BorgnixClient::borgDevCb = NULL;

BorgnixClient::BorgnixClient(char* host, uint16_t port, char* uuid, char* token, char* wifiName, char* wifiPass)
{
  debugPort = new SoftwareSerial(7, 8); // RX, TX
  esp = new ESP(&Serial, debugPort, 4);
  mqtt = new MQTT(esp);
  
  BorgnixClient::host = host;
  BorgnixClient::port = port;
  BorgnixClient::uuid = uuid;
  BorgnixClient::token = token;
  BorgnixClient::wifiName = wifiName;
  BorgnixClient::wifiPass = wifiPass;
  
  for (int i = 0; i < MAX_JOB_COUNT; i++)
  {
    Interval* item = (Interval*)malloc(sizeof(Interval));
    item->switchOn = false;
    item->callback = NULL;
    
    jobList[i] = item;
  }
  
  wdt_enable(WDTO_8S);
  wdt_reset();
  this->setInterval((BorgIntervalCB)feedWatchdog, 500);
}

BorgnixClient::~BorgnixClient()
{
  wdt_reset();
  
  this->disconnect();
  
  for (int i = 0; i < MAX_JOB_COUNT; i++)
  {
    free(this->jobList[i]);
  }
}

int BorgnixClient::setInterval(BorgIntervalCB callback, int ms)
{
  Interval* item;
  int i = 0;
  for (i = 0; i < MAX_JOB_COUNT; i++)
  {
    if ((jobList[i])->switchOn == false)
      break;
  }
  
  if (i == MAX_JOB_COUNT - 1 && (jobList[i])->switchOn == true)
    return -1;
  
  item = jobList[i];
  item->callback = callback;
  item->interval = ms;
  item->lastMilli = millis();
  item->switchOn = true;
  
  return i;
}

void BorgnixClient::clearInterval(int intervalNo)
{
  if (intervalNo < MAX_JOB_COUNT && intervalNo >= 0)
    (jobList[intervalNo])->switchOn = false;
}

void feedWatchdog()
{
  wdt_reset();
}

void wifiCb(void* response)
{
  uint32_t status;
  RESPONSE res(response);

  if (res.getArgc() == 1)
  {
    res.popArgs((uint8_t*)&status, 4);
    if (status == STATION_GOT_IP)
    {
      debugPort->println(F("WIFI Connected"));
      mqtt->connect(BorgnixClient::host, BorgnixClient::port);
      wifiConnectFlag = true;
    }
    else
    {
      wifiConnectFlag = false;
      mqtt->disconnect();
    }
  }
}

void mqttConnected(void* response)
{
  debugPort->println(F("MQTT connected"));
  mqttConnectFlag = true;

  // subscribe dev topic
  mqtt->subscribe(BorgnixClient::inTopic);
}

void mqttPublished(void* response)
{

}

void mqttDisconnected(void* response)
{

}

void mqttData(void* response)
{
  RESPONSE res(response);
  res.popString();
  String str = res.popString();
  int len = str.length() + 1;
  char buf[len + 1];
  buf[len] = '\0';
  str.toCharArray(buf, len);
  
  // call user data callback
  BorgnixClient::borgDevCb(buf);
}

void BorgnixClient::BorgDevSend(char* payload)
{
  showFreeMemory();
  
  mqtt->publish(BorgnixClient::outTopic, payload);
}

void BorgnixClient::BorgSimpleSend(String dataType, String payload)
{
  unsigned long ts = millis();
  String data = String(ts);
  data = "ts=" + data;
  data += ",type=";
  data += dataType;
  data += ",data=";
  data += payload;
  int len = data.length() + 1;
  char buf[len];
  buf[len - 1] = '\0';
  data.toCharArray(buf, len);

  showFreeMemory();
  
  mqtt->publish(BorgnixClient::outTopic, buf);
}

void BorgnixClient::BorgTopicSend(String dataType, String payload)
{
  String outTopic = BorgnixClient::outTopic;
  outTopic += "/";
  outTopic += dataType;
  int len = outTopic.length() + 1;
  char topicBuf[len];
  topicBuf[len - 1] = '\0';
  outTopic.toCharArray(topicBuf, len);
  
  unsigned long ts = millis();
  String data = String(ts);
  data = "ts=" + data;
  data += ",data=";
  data += payload;
  len = data.length() + 1;
  char buf[len];
  buf[len - 1] = '\0';
  data.toCharArray(buf, len);

  showFreeMemory();
  
  mqtt->publish(topicBuf, buf);
}

boolean BorgnixClient::BorgDevConnect(BorgDevCB borgDevCb)
{
  BorgnixClient::borgDevCb = borgDevCb;
  
  String inTopicString = String(TOPIC_PREFIX);
  inTopicString += BorgnixClient::uuid;
  inTopicString += "/in";

  String outTopicString = String(TOPIC_PREFIX);
  outTopicString += BorgnixClient::uuid;
  outTopicString += "/out";

  BorgnixClient::inTopic = (char*)malloc(inTopicString.length() + 1);
  BorgnixClient::inTopic[inTopicString.length()] = '\0';
  inTopicString.toCharArray(BorgnixClient::inTopic, inTopicString.length() + 1);
  
  BorgnixClient::outTopic = (char*)malloc(outTopicString.length() + 1);
  BorgnixClient::outTopic[outTopicString.length()] = '\0';
  outTopicString.toCharArray(BorgnixClient::outTopic, outTopicString.length() + 1);

  // connect MQTT
  Serial.begin(SERIAL_BAUD);
  debugPort->begin(SERIAL_BAUD);
  esp->enable();
  delay(500);
  esp->reset();
  delay(500);
  while (!esp->ready());

  debugPort->println(F("ARDUINO: setup mqtt client"));
  if (!mqtt->begin(BorgnixClient::uuid, BorgnixClient::uuid, BorgnixClient::token, 120, 1))
  {
    debugPort->println(F("ARDUINO: failed to setup mqtt client"));
    while (1);
  }

  // setup lwt
  mqtt->lwt("/lwt", "offline", 0, 0);

  // setup mqtt
  mqtt->connectedCb.attach(mqttConnected);
  mqtt->disconnectedCb.attach(mqttDisconnected);
  mqtt->publishedCb.attach(mqttPublished);
  mqtt->dataCb.attach(mqttData);

  // setup & start wifi
  debugPort->println(F("ARDUINO: setup wifi"));
  esp->wifiCb.attach(wifiCb);
  esp->wifiConnect(BorgnixClient::wifiName, BorgnixClient::wifiPass);

  debugPort->println(F("ARDUINO: system started"));

  return true;
}

void BorgnixClient::disconnect()
{
  mqtt->disconnect();
}

void BorgnixClient::showFreeMemory()
{
  debugPort->print(F("RAM: "));
  debugPort->println(freeMemory(), DEC);
}

void BorgnixClient::process()
{
  esp->process();
  if (wifiConnectFlag)
  {
    for (int i = 0; i < MAX_JOB_COUNT; i++)
    {
      Interval* item = jobList[i];
      if (item->switchOn == true)
      {
        unsigned long now = millis();
        if (now - item->lastMilli > item->interval)
        {
          BorgIntervalCB callback = item->callback;
          callback();
          item->lastMilli = now;
        } // if timeout
      } // if switchOn
    } // i loop
  } // if wifiConnectFlag
}

