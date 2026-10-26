// mqtt_washes.ino
// Tests WiFi + MQTT by cycling all 72 LEDs through red, green, blue washes.
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

String lightId    = "20";                                       // <-- your allocated number
String mqtt_topic = "student/CASA0014/luminaire/" + lightId;
String clientId   = "";

const int num_leds     = 72;
const int payload_size = num_leds * 3;
byte RGBpayload[payload_size];

void setup() {
  Serial.begin(115200);
  Serial.println(F("Test 7: MQTT washes to Vespera (light 20)"));

  WiFiDrv::pinMode(25, OUTPUT);
  WiFiDrv::pinMode(26, OUTPUT);
  WiFiDrv::pinMode(27, OUTPUT);
  LedRed();

  byte mac[6];
  WiFi.macAddress(mac);
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02X%02X%02X%02X%02X%02X", mac[5], mac[4], mac[3], mac[2], mac[1], mac[0]);
  clientId = "MKR1010_Sender_" + String(macStr);

  Serial.print(F("Publishing to topic: "));
  Serial.println(mqtt_topic);

  startWifi();
  mqttClient.setServer(mqtt_server, mqtt_port);
  mqttClient.setBufferSize(512);
  mqttClient.setKeepAlive(30);
  Serial.println(F("Setup complete - starting colour cycle"));
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) startWifi();
  if (!mqttClient.connected()) reconnectMQTT();
  mqttClient.loop();

  Serial.println(F("Red..."));
  for (int n = 0; n < 50; n++) { send_colour_to_all((n * 4) + 50, 0, 0); mqttClient.loop(); delay(50); }

  Serial.println(F("Green..."));
  for (int n = 0; n < 50; n++) { send_colour_to_all(0, (n * 4) + 50, 0); mqttClient.loop(); delay(50); }

  Serial.println(F("Blue..."));
  for (int n = 0; n < 50; n++) { send_colour_to_all(0, 0, (n * 4) + 50); mqttClient.loop(); delay(50); }

  delay(1000);
}

// Set all 72 LEDs to one colour and publish the full picture.
void send_colour_to_all(int r, int g, int b) {
  if (mqttClient.connected()) {
    for (int pixel = 0; pixel < num_leds; pixel++) {
      RGBpayload[pixel * 3 + 0] = (byte)constrain(r, 0, 255);
      RGBpayload[pixel * 3 + 1] = (byte)constrain(g, 0, 255);
      RGBpayload[pixel * 3 + 2] = (byte)constrain(b, 0, 255);
    }
    mqttClient.publish(mqtt_topic.c_str(), RGBpayload, payload_size);
  } else {
    Serial.println(F("MQTT not connected, cannot publish."));
  }
}
