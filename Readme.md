BorgnixAPI-ArduinoV2  
--------------------  
BorgnixAPI-ArduinoV2 provides APIs to help Arduino to talk to Borgnix.com.  
Borgnix ESPShield is suggestted for fast & stable connection between Arduino and ESP module. Bare ESP module could also be used.  
  
API  
---  
```javascript
BorgnixClient(char* host, uint16_t port, char* uuid, char* token, char* wifiName, char* wifiPass);  
```
Constructor  
```javascript
boolean BorgDevConnect(BorgDevCB borgDevCb);
```
  void BorgDevSend(char* payload);
  void BorgSimpleSend(String dataType, String payload);
  void BorgTopicSend(String dataType, String payload);
  
  void process();
  
  int setInterval(BorgIntervalCB callback, int ms);
  void clearInterval(int intervalNo);
  int setTimeout(BorgIntervalCB callback, int ms, int lifeCycle);

  void safeDelay(int ms);
  void showFreeMemory();
