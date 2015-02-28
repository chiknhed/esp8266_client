# Arduino library for Sharp GP2Y1010AU sensor

This is a very simple library for ESP8266 module from Ai-Thinker

## precondition

- Unstable yet
- Dependent on AltSoftSerial library. I had to patch AltSoftSerial library to modify peek function and increase RX buffer somewhat.


## Hardware configuration tested

Arduino Pro Mini 3.3V
ESP8266mod module from Ai-Thinker (http://forum.hobbycomponents.com/viewtopic.php?f=74&t=1777)

## Current function

- AP connection
- TCP connection (single)
- AP Scan

## ToDo

- detailed HW configuration guide (reference)
- more stable connection
- corner-case handling
- ~~add modified AltSoftSerial library patch~~
- ~~Scan functionality~~
- ~~Ap connection~~
- ~~TCP connection (single)~~


##Reference

- on the module itself and HW configuration : http://forum.hobbycomponents.com/viewtopic.php?f=74&t=1777
