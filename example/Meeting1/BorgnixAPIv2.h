/*
 BorgnixAPIv2.h - Borgnix Arduino API for ESP8266 Adaptor Shield
 Xie Di, 2015.05.21
 http://www.borgnix.com
*/

#ifndef BORGNIX_API_V2_H
#define BORGNIX_API_V2_H

#define SERIAL_BAUD 19200

#ifndef MAX_JOB_COUNT
#define MAX_JOB_COUNT 8
#endif

#define TOPIC_PREFIX "/devices/"

extern void wifiCb(void* response);
extern void mqttConnected(void* response);
extern void mqttDisconnected(void* response);
extern void mqttData(void* response);
extern void mqttPublished(void* response);
extern void feedWatchdog();

extern SoftwareSerial* debugPort;
extern ESP* esp;
extern MQTT* mqtt;
extern boolean wifiConnectFlag;
extern boolean mqttConnectFlag;

typedef void (*BorgDevCB)(char* payload);
typedef void (*BorgIntervalCB)();

typedef struct Interval_Item
{
  BorgIntervalCB callback;
  unsigned long interval;
  unsigned long lastMilli;
  boolean switchOn;
} Interval;

class BorgnixClient
{
  public:
  BorgnixClient(char* host, uint16_t port, char* uuid, char* token, char* wifiName, char* wifiPass);
  ~BorgnixClient();
  
  boolean BorgDevConnect(BorgDevCB borgDevCb);
  void BorgDevSend(char* payload);
  void BorgSimpleSend(String dataType, String payload);
  void BorgTopicSend(String dataType, String payload);
  
  void process();
  
  int setInterval(BorgIntervalCB callback, int ms);
  void clearInterval(int intervalNo);

  void showFreeMemory();
  
  static char* host;
  static uint16_t port;
  static char* uuid;
  static char* token;
  static char* inTopic;
  static char* outTopic;
  static char* wifiName;
  static char* wifiPass;
  static BorgDevCB borgDevCb;
  
  private:
  // helper
  void disconnect();
  Interval* jobList[MAX_JOB_COUNT];
};

#endif

