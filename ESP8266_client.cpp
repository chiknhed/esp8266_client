#include "Arduino.h"
#include "ESP8266_client.h"
#include <avr/pgmspace.h>

#define WAIT_INIT_MS	5000
#define AP_WAIT_MS		20000
#define FIND_PEEK_DELAY_MS	5
#define WAIT_CONNECTION_MS	10000
#define NORMAL_COMMAND_RESP_MS	100
#define WAIT_SEND_MS	3000


ESP8266ClientClass::ESP8266ClientClass(void)
{
	conState = WL_UNINIT;
	remainingBytes = 0;
	timeout = 1000;
}

bool ESP8266ClientClass::safePrint(char * buffer, bool line)
{
	if (line)
		mySerial.println(buffer);
	else 
		mySerial.print(buffer);
	
	mySerial.flushOutput();

	return find(buffer);	
}

bool ESP8266ClientClass::safePrint(const __FlashStringHelper *ifsh, bool line)
{
	if (line)
		mySerial.println(ifsh);
	else 
		mySerial.print(ifsh);
	
	mySerial.flushOutput();
	
	return find(ifsh);	
}

bool ESP8266ClientClass::find(char * buffer)
{
	char peekValue;
	byte i;
	
	i = 0;
	while(true) {
		unsigned char inChar;
		unsigned char myChar;
		
		myChar = buffer[i];
		while (myChar == '\n' || myChar == '\r') {
			i += 1;
			myChar = buffer[i];
		}
		if (myChar == 0)
			break;
		byte readBytes = mySerial.readBytes(&inChar, 1);
		while (inChar == '\n' || inChar == '\r') {
			readBytes = mySerial.readBytes(&inChar, 1);
		}
#if 0
		Serial.print(myChar, HEX);Serial.print(" ");
		Serial.print(inChar, HEX);Serial.print(" ");
#endif
			
		if (!readBytes || myChar != inChar) {
			Serial.println("readbytes:");
			Serial.println(readBytes);
			return false;
		}
		
		i++;
	}
	
	while (true) {
		char c = mySerial.peek();
		if (c != '\n' && c != '\r')
			break;
		mySerial.readBytes(&c, 1);
		delay(FIND_PEEK_DELAY_MS);
	}	
		
	return true;
}

bool ESP8266ClientClass::find(const __FlashStringHelper *ifsh)
{
	const char PROGMEM *p = (const char PROGMEM *) ifsh;
	char peekValue;
	byte i;
	
	i = 0;
	while(true) {
		unsigned char inChar;
		unsigned char myChar;
		myChar = pgm_read_byte_near(p + i);
		
		while (myChar == '\n' || myChar == '\r') {
			i += 1;
			myChar = pgm_read_byte_near(p + i);
		}
		if (myChar == 0)
			break;
		byte readBytes = mySerial.readBytes(&inChar, 1);
		while (inChar == '\n' || inChar == '\r') {
			readBytes = mySerial.readBytes(&inChar, 1);
		}
#if 0
		Serial.print(myChar, HEX);Serial.print(" ");
		Serial.print(inChar, HEX);Serial.print(" ");
#endif
		if (!readBytes || myChar != inChar) {
			return false;
		}
		
		i++;
	}
	
	while (true) {
		char c = mySerial.peek();
		if (c != '\n' && c != '\r')
			break;
		mySerial.readBytes(&c, 1);
		delay(FIND_PEEK_DELAY_MS);
	}	
			
	return true;
}

bool ESP8266ClientClass::ping(void)
{
	mySerial.flushInput();
	mySerial.setTimeout(1000);
	
	safePrint("AT", true);
	if (!find(F("OK"))) {
		return false;
	}
	return true;
}

bool ESP8266ClientClass::waitString(char *buffer)
{
	char * readyChar;
	
	mySerial.setTimeout(WAIT_INIT_MS);
	readyChar = buffer;
	while (true) {
		char inChar;
		byte readBytes = mySerial.readBytes(&inChar, 1);
		
		if (!readBytes) {
			return false;
		}
		
		if (inChar == *readyChar) {
			readyChar ++;
			if (*readyChar == 0)
				break;
		} else {
			readyChar = buffer;
		}
	}
	
	delay(50);
	
	return true;
}

void ESP8266ClientClass::normalizePasswd(char * input, char * normalized)
{
	byte i;
	byte o;
	
	o = 0;
	for (i = 0; i < strlen(input); i++) {
		if (input[i] == '\"') {
			normalized[o] = '\\';
			o++;
		}
		normalized[o] = input[i];
		o++;
	}
	normalized[o] = 0;
}

wl_status_t ESP8266ClientClass::begin(char * ssid, char * passwd)
{
	byte trial;
	char buffer[WIFI_PASSWORD_LENGTH + 10];
	
	mySerial.begin(9600);
	
	delay(1000);
	mySerial.setTimeout(WAIT_INIT_MS);
	mySerial.flushInput();
	
	trial = 0;
	while(conState == WL_UNINIT) {
		mySerial.println(F("AT+RST"));
		if (waitString("ready\r\n") && ping()) {
			conState = WL_DISCONNECTED;
			break;
		}
		
		trial ++;
		if (trial == 3) {
			conState = WL_UNINIT;
			break;
		}
	}
	
	if (conState == WL_UNINIT) {
		return conState;
	}
	
	conState = WL_DISCONNECTED;
		
	delay(500);
	if (!ssid)
		return conState;
		
	/* set mode to STA */
	mySerial.println(F("AT+CWMODE=1"));
	delay(500);
	mySerial.flushInput();
		
	/* connect to the AP */
	mySerial.setTimeout(AP_WAIT_MS);
	if (!safePrint(F("AT+CWJAP=\"")))
		return conState;
	if (!safePrint(ssid))
		return conState;
	if (!safePrint(F("\",\"")))
		return conState;
	if (passwd) {
		normalizePasswd(passwd, buffer);
		
		if (!safePrint(buffer))
			return conState;
	}
	if (!safePrint(F("\""), true))
		return conState;
	mySerial.flushInput();
	if (!find(F("OK")))
		return conState;
		
	conState = WL_CONNECTED;
	
	return conState;
}

wl_status_t ESP8266ClientClass::status(void)
{
	if (conState == WL_UNINIT)
		return conState;
	
	mySerial.flushInput();
	mySerial.setTimeout(NORMAL_COMMAND_RESP_MS);
	safePrint(F("AT+CWJAP?"), true);
	if (!find(F("+CWJAP:"))) {
		delay(NORMAL_COMMAND_RESP_MS);
		mySerial.flushInput();
		conState = WL_DISCONNECTED;
	} else {
		delay(NORMAL_COMMAND_RESP_MS);
		mySerial.flushInput();
		conState = WL_CONNECTED;
	}
	
	return conState;
}

bool ESP8266ClientClass::connect(char * host, unsigned int port)
{
	char port_buffer[20];
	
	if (conState == WL_UNINIT || conState == WL_DISCONNECTED)
		return false;
	
	mySerial.flushInput();
	mySerial.setTimeout(NORMAL_COMMAND_RESP_MS);
	if (!safePrint(F("AT+CIPSTART=\"TCP\",\"")))
		goto out;
	if (!safePrint(host))
		goto out;
	if (!safePrint(F("\",")))
		goto out;
	sprintf(port_buffer, "%d", port);
	if (!safePrint(port_buffer, true))
		goto out;
	
	mySerial.setTimeout(WAIT_CONNECTION_MS);
	
	if (!find(F("OK")))
		goto out;
	
	if (!find(F("Linked")))
		goto out;
	
	return true;
	
out:
	delay(100);
	mySerial.flushInput();
	return false;
}

bool ESP8266ClientClass::isConnected(void)
{
	if (conState == WL_UNINIT || conState == WL_DISCONNECTED)
		return false;
	
	mySerial.flushInput();
	mySerial.setTimeout(NORMAL_COMMAND_RESP_MS);
	if (!safePrint(F("AT+CIPSTART=?"), true))
		goto out;
	if (!find(F("+CIPSTART:")))
		goto out;
	
	delay(500);
	mySerial.flushInput();
	
	return true;
	
out:
	delay(500);
	mySerial.flushInput();
}

void ESP8266ClientClass::disconnect(void)
{
	if (conState == WL_UNINIT || conState == WL_DISCONNECTED)
		goto out;
	
	mySerial.flushInput();
	mySerial.setTimeout(NORMAL_COMMAND_RESP_MS);
	
	if (!safePrint(F("AT+CIPCLOSE"), true))
		goto out;
	if (!find(F("OK")))
		goto out;
	find(F("UNLINK"));
	
out:
	delay(10);
	mySerial.flushInput();
}

void ESP8266ClientClass::print(char * buffer)
{
	char len_buffer[10];
	
	delay(30);
	
	if (conState == WL_UNINIT || conState == WL_DISCONNECTED)
		return;
	
	mySerial.flushInput();
	mySerial.setTimeout(WAIT_SEND_MS);
	
	if(!safePrint(F("AT+CIPSEND=")))
		goto out;
	
	sprintf(len_buffer, "%d", strlen(buffer));
	if(!safePrint((char*)len_buffer, true))
		goto out;
	if(!find(F(">")))
		goto out;
	
	if(!find(F(" ")))		/* I don't know why do we need this */
		goto out;
	
	if(!safePrint(buffer)) {
		goto out;
	}
	
	if(!find(F("SEND OK")))
		goto out;
	
	return;

out:
	//readLoop(); //okkwon debug
	delay(500);
	mySerial.flushInput();
}

void ESP8266ClientClass::print(const __FlashStringHelper *ifsh)
{
	char cbuffer[21];
	const char PROGMEM *p = (const char PROGMEM *) ifsh;
	int i;
	
	if (conState == WL_UNINIT || conState == WL_DISCONNECTED)
		return;
	
	mySerial.flushInput();
	mySerial.setTimeout(WAIT_SEND_MS);
	
	cbuffer[1] = 0;
	i = 0;
	while(true) {
		char c = pgm_read_byte_near(p + i);
		cbuffer[i%20] = c;
		cbuffer[i%20+1] = 0;
		if (c == 0) {
			if (i%20)
				print(cbuffer);
			break;
		}
		if (i%20 == 19) {
			print(cbuffer);
		}
		i++;
	}
}

void ESP8266ClientClass::readLoop(void)
{
	while (true) {
		char c;
		byte rb = mySerial.readBytes(&c, 1);

		if (rb) {
			//Serial.write(c);
		}
	}
}

void ESP8266ClientClass::setTimeout(long to)
{
	timeout = to;
}

byte ESP8266ClientClass::readBytes(char* buffer, byte buffer_size)
{
	char c;
	byte rb;
	
	mySerial.setTimeout(timeout);
	
	if (remainingBytes == 0) {
		if(!waitString("+IPD,")) {
			return 0;
		}
		
		remainingBytes = 0;
		while(mySerial.readBytes(&c, 1)) {
			if (c == ':') {
				break;
			}
			remainingBytes *= 10;
			remainingBytes += c - '0';			
		}
	}
	
	rb = mySerial.readBytes(buffer, buffer_size - 1);
	if (rb >= remainingBytes) {
		rb = remainingBytes;
		mySerial.flushInput();
	}
	remainingBytes -= rb;
	buffer[rb] = 0;
	
	return rb;
}

bool ESP8266ClientClass::startScan(void)
{
	delay(100);
	mySerial.setTimeout(NORMAL_COMMAND_RESP_MS);
	mySerial.flushInput();
	
	if (!safePrint(F("AT+CWLAP=\"\",\"\""), true))
		return false;
	
	while (!mySerial.available()) {
		delay(100);
	}
	
	return true;
}

bool ESP8266ClientClass::scanEntry(char * ssid, char * rssi, byte *security)
{
	char c;
	byte rb;
	char buffer[30];
	
	mySerial.setTimeout(NORMAL_COMMAND_RESP_MS);
	if (!find(F("+CWLAP:(")))
		goto out;
	
	rb = mySerial.readBytes(&c, 1);
	*security = c - '0';
	if (*security == 4) *security = 3;
	
	if (!find(F(",\"")))
		goto out;
	
	rb = mySerial.readBytesUntil('\"', ssid, WIFI_SSID_LENGTH);
	ssid[rb] = 0;
	
	if (!find(F(",-")))
		goto out;
	
	*rssi = 0;
	while (true) {
		rb = mySerial.readBytes(&c, 1);
		if (c <= '9' && c >= '0') {
			*rssi *= 10;
			*rssi += c - '0';
		} else {
			break;
		}
	}
	*rssi = -*rssi;
	
	mySerial.readBytesUntil('\n', buffer, 30);
	
	return true;
	
out:
	conState = WL_UNINIT;	/* reset after scanning -_-; */
	return false;
}
		
ESP8266ClientClass wifi;