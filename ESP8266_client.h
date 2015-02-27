#ifndef __ESP8266_CLIENT_H
#define __ESP8266_CLIENT_H

#include "Arduino.h"
#include <avr/pgmspace.h>
#include <inttypes.h>
#include "AltSoftSerial.h"

#define WIFI_SSID_LENGTH 33
#define WIFI_PASSWORD_LENGTH 20

typedef enum {
	WIFI_SECURITY_OPEN = 0,
	WIFI_SECURITY_WEP = 1,
	WIFI_SECURITY_WPA1 = 2,
	WIFI_SECURITY_WPA2 = 3,
} security_t;

typedef enum {
	WL_UNINIT = 0,
	WL_DISCONNECTED = 1,
	WL_CONNECTED = 2,
} wl_status_t;	

class ESP8266ClientClass
{
	private:
		AltSoftSerial mySerial;
		bool ping(void);
		bool find(const __FlashStringHelper *ifsh, bool line = false);
		bool find(char * buffer, bool line = false);
		bool waitInit(void);
		wl_status_t conState;
		void normalizePasswd(char * input, char * normalized);
		bool safePrint(char * buffer, bool line = false);
		bool safePrint(const __FlashStringHelper *ifsh, bool line = false);
		
	public:
		ESP8266ClientClass();
		wl_status_t begin(char * ssid, char * password = NULL);
		wl_status_t status(void);
		bool connect(char * host, unsigned int port);
		bool isConnected(void);
		void println(char * buffer);
		void print(char * buffer);
		void println(const __FlashStringHelper *ifsh);
		void print(const __FlashStringHelper *ifsh);
		void startScan(void);
		bool scanEntry(char * ssid, char * rssi, int *security);
		size_t read(char * buffer, size_t bufferSize);
};

extern ESP8266ClientClass wifi;
#endif	//__ESP8266_CLIENT_H

