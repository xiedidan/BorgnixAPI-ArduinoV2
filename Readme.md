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
<hr>
```javascript
boolean BorgDevConnect(BorgDevCB borgDevCb);  
```  
Connect to Borgnix.com through MQTT.  
borgDevCb is provided by user, BorgnixAPI will callback when data arrives. Data parsing and handling should go to borgDevCb:  
```javascript
typedef void (*BorgDevCB)(char* payload);  
```
<hr>
```javascript
void BorgDevSend(char* payload);  
```
Send plain text (payload) to Borgnix.com  
<hr>
```javascript
void BorgSimpleSend(String dataType, String payload);  
```
BorgDevSend wrapper. BorgSimpleSend will format inputs and call BorgDevSend. eg.:  
```javascript
ts=12345678 type=YOUR_DATA_TYPE payload=YOUR_PAYLOAD  
```
ts(timestamp) is the result of millis() function.
<hr>
```javascript
void BorgTopicSend(String dataType, String payload);
```

  
  void process();
  
  int setInterval(BorgIntervalCB callback, int ms);
  void clearInterval(int intervalNo);
  int setTimeout(BorgIntervalCB callback, int ms, int lifeCycle);

  void safeDelay(int ms);
  void showFreeMemory();
