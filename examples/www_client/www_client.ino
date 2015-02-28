#include "AltSoftSerial.h"
#include "ESP8266_client.h"


void setup() {
  wl_status_t wl_state;
  char ssid[WIFI_SSID_LENGTH];
  byte security;
  char rssi;
  
  Serial.begin(115200);
  
  wl_state = wifi.begin();
  Serial.println(wl_state);
  
  Serial.println(F("Scanning"));
  wifi.startScan();
  while (wifi.scanEntry(ssid, &rssi, &security)) {
    Serial.print(ssid);
    Serial.print(F(":"));
    Serial.print((int)rssi);
    Serial.print(F(":"));
    Serial.println(security);
  }
  
  Serial.println(F("Connecting to AP"));
  wl_state = wifi.begin("leekwon2G", "xnvkdlqm");
  Serial.println(wl_state);
  Serial.println(wifi.status());
  
#if 0
  wl_state = wifi.begin("leekwon2G", "11111111");
  Serial.println(wl_state);
  Serial.println(wifi.status());
#endif
  
  Serial.setTimeout(0);
}

void loop() {
  char buffer[21];
  byte rb;
  
  Serial.println(F("connecting to the server"));
  wifi.connect("www.google.com", 80);
  if (wifi.isConnected()) {
    Serial.println(F("connected"));
  } else  {
    Serial.println(F("not connected"));
    while(1);
  }
  
  Serial.println(F("Sending request"));
  wifi.print(F("GET / HTTP/1.1\r\n"));
  wifi.print("Host: www.google.com\r\n");
  wifi.print(F("Connection: close\r\n"));
  wifi.print(F("\r\n"));
  
  Serial.println(F("Printing response"));
  wifi.setTimeout(2000);
  do {
    rb = wifi.readBytes(buffer, 21);
    if (rb) {
      Serial.print(buffer);
    }
  } while (rb);
  
  delay(50000);
  wifi.disconnect();
  if (wifi.isConnected())
    Serial.println(F("connected"));
  else 
    Serial.println(F("not connected"));
  delay(50000);
}
