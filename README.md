# Arduino library for ESP8266 (Client only)

This is a very simple library for ESP8266 module from Ai-Thinker

## precondition

- ~~Quite stable now~~ -> There's unknown reset after days of test. May be current issue? or a bug?
- Dependent on AltSoftSerial library. I had to patch AltSoftSerial library to modify peek function and increase RX buffer somewhat.
- You'd better to use 115200 for HW Serial to don't become bottleneck for SoftSerial (I tested with 115200 usually)
- AltSoftSerial (connected to ESP8266) works as 9600bps

## Some Tips

- Firmware version is very important. I tried with older version and moved to newer version, and the AT response is quite different.
- There is timing issue due to limited UART RX buffer. For example, you should handle the scan entry as fast as possible to prevent RX buffer overflow.

## Hardware configuration tested

Arduino Pro Mini 3.3V

ESP8266mod module from Ai-Thinker (http://forum.hobbycomponents.com/viewtopic.php?f=74&t=1777)

Firmware version :
AT version:0.21.0.0
SDK version:0.9.5

## Current function

- AP connection
- TCP connection (single)
- AP Scan

## ToDo

- ~~detailed HW configuration guide (reference)~~ mark this done for now :)
- ~~more stable connection~~
- corner-case handling
- ~~add modified AltSoftSerial library patch~~
- ~~Scan functionality~~
- ~~Ap connection~~
- ~~TCP connection (single)~~


##Reference

- on the module itself and HW configuration : http://forum.hobbycomponents.com/viewtopic.php?f=74&t=1777
- Espressif page : https://espressif.com/

## public API list

```
ESP8266ClientClass();
wl_status_t begin(char * ssid = NULL, char * password = NULL, byte * mac = NULL);
wl_status_t status(void);
bool connect(char * host, unsigned int port);
bool isConnected(void);
void disconnect(void);
void print(char * buffer);
void print(const __FlashStringHelper *ifsh);
void readLoop(void);
void setTimeout(long timeout);
byte readBytes(char* buffer, byte buffer_size);
bool startScan(void);
bool scanEntry(char * ssid, char * rssi, byte *security);
void getMac(byte mac[6]);
```
