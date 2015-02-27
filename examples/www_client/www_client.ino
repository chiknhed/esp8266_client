#include "AltSoftSerial.h"
#include "ESP8266_client.h"


void setup() {
  wl_status_t wl_state;
  
  Serial.begin(9600);
  
  wl_state = wifi.begin("leekwon2G", "xnvkdlqm");
  Serial.println(wl_state);
  Serial.println(wifi.status());
  
  wl_state = wifi.begin("leekwon2G", "11111111");
  Serial.println(wl_state);
  Serial.println(wifi.status());
  
  Serial.setTimeout(0);
}

void loop() {
}
