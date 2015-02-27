#include "Arduino.h"
#include "ESP8266_client.h"

#define WAIT_INIT_MS	5000
#define AP_WAIT_MS		10000
#define FIND_PEEK_DELAY_MS	5
#define WAIT_CONNECTION_MS	10000
#define NORMAL_COMMAND_RESP_MS	100



ESP8266ClientClass::ESP8266ClientClass(void)
{
	conState = WL_UNINIT;
}

bool ESP8266ClientClass::safePrint(char * buffer, bool line)
{
	if (line)
		mySerial.println(buffer);
	else 
		mySerial.print(buffer);
	
	mySerial.flushOutput();

	return find(buffer, line);	
}

bool ESP8266ClientClass::safePrint(const __FlashStringHelper *ifsh, bool line)
{
	if (line)
		mySerial.println(ifsh);
	else 
		mySerial.print(ifsh);
	
	mySerial.flushOutput();
	
	return find(ifsh, line);	
}

bool ESP8266ClientClass::find(char * buffer, bool line)
{
	char peekValue;
	
	if (!mySerial.find(buffer))
		return false;
	
	if (!line)
		return true;
	
	delay(FIND_PEEK_DELAY_MS);
	while (mySerial.peek() == '\n' || mySerial.peek() == '\r') {
		char c;
		mySerial.readBytes(&c, 1);
		delay(FIND_PEEK_DELAY_MS);
	}
	
	return true;
}

bool ESP8266ClientClass::find(const __FlashStringHelper *ifsh, bool line)
{
	const char PROGMEM *p = (const char PROGMEM *) ifsh;
	char peekValue;
	byte i;
	
	i = 0;
	while(true) {
		unsigned char inChar;
		unsigned char myChar;
		myChar = pgm_read_byte_near(p + i);
		if (myChar == 0)
			break;
		byte readBytes = mySerial.readBytes(&inChar, 1);
		
		Serial.println(myChar);
		Serial.println(inChar);
		
		if (!readBytes || myChar != inChar) {
			return false;
		}
		
		i++;
	}
	
	if (!line)
		return true;
	
	delay(FIND_PEEK_DELAY_MS);
	while (mySerial.peek() == '\n' || mySerial.peek() == '\r') {
		char c;
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
	if (!find(F("OK"), true)) {
		return false;
	}
	return true;
}

bool ESP8266ClientClass::waitInit(void)
{
	char * readyString = "ready\r\n";
	char * readyChar;
	
	mySerial.setTimeout(WAIT_INIT_MS);
	readyChar = readyString;
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
			readyChar = readyString;
		}
	}
	
	delay(50);
	
	return ping();
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
	
	trial = 0;
	while(conState == WL_UNINIT) {
		mySerial.println(F("AT+RST"));
		if (waitInit()) {
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
	
	mySerial.setTimeout(500);
	
	/* set mode to STA */
	mySerial.println(F("AT+CWMODE=1"));
	delay(500);
	mySerial.flushInput();
		
	/* connect to the AP */
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
	mySerial.setTimeout(AP_WAIT_MS);
	if (!find(F("\r\nOK"), true))
		return conState;
		
	conState = WL_CONNECTED;
	
	return conState;
}

wl_status_t ESP8266ClientClass::status(void)
{
	if (conState == WL_UNINIT)
		return conState;
	
	mySerial.flushInput();
	safePrint(F("AT+CWJAP?"), true);
	if (!find(F("+CWJAP:"))) {
		delay(10);
		mySerial.flushInput();
		conState = WL_DISCONNECTED;
	} else {
		delay(10);
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
		return false;
	if (!safePrint(host))
		return false;
	if (!safePrint(F("\",")))
		return false;
	sprintf(port_buffer, "%d", port);
	if (!safePrint(port_buffer, true))
		return false;
	
	mySerial.setTimeout(WAIT_CONNECTION_MS);
	
	if (!find(F("\r\nOK"), true))
		return false;
	
	if (!find(F("Linked"), true))
		return false;
	
	return true;
}

ESP8266ClientClass wifi;