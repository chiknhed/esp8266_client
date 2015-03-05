# Arduino library for ESP8266 (Client only)

This is a very simple library for ESP8266 module from Ai-Thinker

## precondition

- ~~Quite stable now~~ -> There's unknown reset after days of test. May be current issue? or a bug? (I used 3.3v output from FTDI with Arduino Pro Mini 3.3V, so current issue is strongly suspected)
- Dependent on AltSoftSerial library. I had to patch AltSoftSerial library to modify peek function and increase RX buffer somewhat.
- You'd better to use 115200 for HW Serial to don't become bottleneck for SoftSerial (I tested with 115200 usually)
- AltSoftSerial (connected to ESP8266) works as 9600bps

## My Current Hardware Configuration

- I think I really need a power supply for 3.3v
- My ESP8266 module looks like ESP-07
- ESP8266 GPIO0 -> VCC
- ESP8266 GPIO2 -> HIGH
- ESP8266 GPIO15 -> GND
- ESP8266 CH\_PD -> Pull-up (Only this pin should be pull-up for me.. Why?)
- ESP8266 TXD -> Pro Mini D8
- ESP8266 RXD -> Pro Mini D9

## How did I flash my Module

- Download SDK from Espressif page : http://bbs.espressif.com/viewtopic.php?f=5&t=154
- Download Flash program for Espressif page : http://bbs.espressif.com/viewtopic.php?f=7&t=25&p=70&hilit=FLASH_DOWNLOAD_TOOLS_v0.9.3.1_141118#p70
- Flash by "new firmware" instruction at esp\_iot\_sdk\_v0.9.5\_15\_01\_23/esp\_iot\_sdk\_v0.9.5/bin/at/readme.txt
- Run the following AT commands with FTDI connection to ESP8266
```
AT+CWMODE=1
AT+RST
AT+CWJAP="leekwon2G","xnvkdlqm"
AT+CIUPDATE
AT+UART=9600,8,1,0,0
```

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
