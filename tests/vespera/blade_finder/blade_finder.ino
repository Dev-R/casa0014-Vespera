// blade_finder.ino
// Lights one LED at a time (0-71) to map LED numbers to blade positions.
// Requires: connections.ino, RGBLED.ino, arduino_secrets.h

#include <SPI.h>
#include <WiFiNINA.h>
#include <PubSubClient.h>
#include "arduino_secrets.h"
#include <utility/wifi_drv.h>

const char* ssid          = SECRET_SSID;
const char* password      = SECRET_PASS;
const char* ssid1         = SECRET_SSID1;
const char* password1     = SECRET_PASS1;
const char* mqtt_username = SECRET_MQTTUSER;
const char* mqtt_password = SECRET_MQTTPASS;
const char* mqtt_server   = "mqtt.cetools.org";
const int mqtt_port       = 1884;

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

String lightId    = "20";
String mqtt_topic = "student/CASA0014/luminaire/" + lightId;
String clientId   = "";

const int num_leds     = 72;
const int payload_size = num_leds * 3;
byte RGBpayload[payload_size];

const int STEP_MS   = 2500;   // how long each LED stays lit (ms) - raise if you need longer to note it
const int BRIGHT    = 120;    // white brightness (0-255)
int current = 0;              // which LED is lit right now

void setup() {
  Serial.begin(115200);
  Serial.println(F("Test 8: blade finder (light 20) - one white LED at a time"));

  WiFiDrv::pinMode(25, OUTPUT);
  WiFiDrv::pinMode(26, OUTPUT);
  WiFiDrv::pinMode(27, OUTPUT);
  LedRed();

  byte mac[6];
  WiFi.macAddress(mac);
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02X%02X%02X%02X%02X%02X", mac[5], mac[4], mac[3], mac[2], mac[1], mac[0]);
  clientId = "MKR1010_Sender_" + String(macStr);

  startWifi();
  mqttClient.setServer(mqtt_server, mqtt_port);
  mqttClient.setBufferSize(512);
  mqttClient.setKeepAlive(30);
  Serial.println(F("LED number now lighting:"));
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) startWifi();
  if (!mqttClient.connected()) reconnectMQTT();
  mqttClient.loop();

  light_one(current);
  Serial.println(current);              // write down where this one lands on the dome

  current++;
  if (current >= num_leds) {
    current = 0;
    Serial.println(F("--- wrapped back to 0 ---"));
  }
  delay(STEP_MS);
}

// Turn everything off, then set LED number `idx` to white, and publish.
void light_one(int idx) {
  for (int i = 0; i < payload_size; i++) RGBpayload[i] = 0;   // all off
  RGBpayload[idx * 3 + 0] = BRIGHT;
  RGBpayload[idx * 3 + 1] = BRIGHT;
  RGBpayload[idx * 3 + 2] = BRIGHT;
  if (mqttClient.connected()) {
    mqttClient.publish(mqtt_topic.c_str(), RGBpayload, payload_size);
  }
}
