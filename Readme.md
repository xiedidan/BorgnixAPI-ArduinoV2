BorgnixAPI-ArduinoV2  
--------------------  
BorgnixAPI-ArduinoV2 provides APIs to help Arduino to talk to Borgnix.com.  
Borgnix ESPShield is suggestted for fast & stable connection between Arduino and ESP module. Bare ESP module could also be used.  
  
API  
---  
###Constructor  
```javascript
BorgnixClient(char* host, uint16_t port, char* uuid, char* token, char* wifiName, char* wifiPass);  
```
Setup for MQTT broker / MQTT verification / WIFI
<hr>  
###Connect  
```javascript
boolean BorgDevConnect(BorgDevCB borgDevCb);  
```  
Connect to Borgnix.com through MQTT.  
borgDevCb is provided by user, BorgnixAPI will callback when data arrives. Data parsing and handling should go to borgDevCb:  
```javascript
typedef void (*BorgDevCB)(char* payload);  
```
<hr>
###Senders
```javascript
void BorgDevSend(char* payload);  
```
Send plain text (payload) to Borgnix.com  
```javascript
void BorgSimpleSend(String dataType, String payload);  
```
BorgDevSend wrapper. BorgSimpleSend will format inputs and call BorgDevSend. eg.:  
```javascript
ts=12345678,type=YOUR_DATA_TYPE,payload=YOUR_PAYLOAD
```
ts(timestamp) is the result of millis() function.
```javascript
void BorgTopicSend(String dataType, String payload);
```
Another BorgDevSend wrapper. BorgTopicSend uses dataType to format MQTT topic. eg.:
```javascript
MQTT topic: /devices/YOUR_UUID/in/YOUR_DATA_TYPE
Message: ts=12345678,type=YOUR_DATA_TYPE,payload=YOUR_PAYLOAD
```
<hr>
###Job System
BorgnixAPI provides a bundle of APIs to control its job system.  
Job system automatically manages user and internal jobs.  
**Some of the internal jobs couldn't be blocked, so user code (not only user job) should use safeDelay() instead of delay().**  
```javascript
void process();
```
process() should be called in loop().  
Parses and processes received MQTT raw data in internal buffer. Maintains MQTT connection.  
```javascript
int setInterval(BorgIntervalCB callback, int ms);
```
Register periodic job. BorgnixAPI will automatically callback per interval.
IntervalNo will be returned and could be stored for further uses.
```javascript
void clearInterval(int intervalNo);
```
Unregister periodic job with speicified intervalNo.  
```javascript
int setTimeout(BorgIntervalCB callback, int ms, int lifeCycle);
```
Register periodic job with limited call count.  
If job is called [lifeCycle] times, it will be removed by job system.
<hr>
###Tools
```javascript
void safeDelay(int ms);
```
An Arduino stock delay() wrapper.  
Delay specified ms and ensure job system work properly.  
```javascript
void showFreeMemory();
```
Check run-time free memory.  
Especially useful for ATMega328p based Arduinos.  
