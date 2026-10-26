// Duncan Wilson Sept 2026 - Simple RGB MQTT Messenger to Vespera
// Works with Arduino MKR1010
//
// This is the starting sketch for the CASA0014 workshop.
// It connects to WiFi and the MQTT broker, and cycles all 72 LEDs
// through Red, Green, and Blue washes.

#include <SPI.h>
#include <WiFiNINA.h>
#include <PubSubClient.h>
#include "arduino_secrets.h" 
#include <utility/wifi_drv.h> // Library to drive the onboard RGB LED on the MKR1010

/*
**** please enter your sensitive data in the Secret tab/arduino_secrets.h
**** using format below
#define SECRET_SSID "ssid name"
#define SECRET_PASS "ssid password"
#define SECRET_MQTTUSER "user name - eg student"
#define SECRET_MQTTPASS "password";
*/
const char* ssid          = SECRET_SSID;
const char* password      = SECRET_PASS;
const char* ssid1         = SECRET_SSID1;
const char* password1     = SECRET_PASS1;
const char* mqtt_username = SECRET_MQTTUSER;
const char* mqtt_password = SECRET_MQTTPASS;
const char* mqtt_server   = "mqtt.cetools.org";
const int mqtt_port       = 1884;

// Create WiFi and MQTT client objects
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

// Update lightId with your allocated student number (10–40)
String lightId = "99";

// MQTT topic to publish color data to: "student/CASA0014/luminaire/<lightId>"
String mqtt_topic = "student/CASA0014/luminaire/" + lightId;            
String clientId = ""; // Unique client ID based on MAC address

// NeoPixel Configuration (72 LEDs * 3 bytes for RGB = 216 bytes)
const int num_leds = 72;
const int payload_size = num_leds * 3; // x3 for RGB

// Create the byte array to send in MQTT payload
byte RGBpayload[payload_size];

void setup() {
  Serial.begin(115200);
  Serial.println(F("Starting Vespera Simple RGB Sender..."));

  // Status LEDs on MKR1010 NINA module
  WiFiDrv::pinMode(25, OUTPUT); // R
  WiFiDrv::pinMode(26, OUTPUT); // G
  WiFiDrv::pinMode(27, OUTPUT); // B
  LedRed();

  // Generate a unique client ID using the device's MAC address
  byte mac[6];
  WiFi.macAddress(mac);
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02X%02X%02X%02X%02X%02X", mac[5], mac[4], mac[3], mac[2], mac[1], mac[0]);
  clientId = "MKR1010_Sender_" + String(macStr);

  Serial.print(F("Device Client ID: "));
  Serial.println(clientId);
  Serial.print(F("Publishing to topic: "));
  Serial.println(mqtt_topic);

  // Connect to WiFi
  startWifi();

  // Connect to MQTT broker
  mqttClient.setServer(mqtt_server, mqtt_port);
  mqttClient.setBufferSize(512);
  mqttClient.setKeepAlive(30);
  
  Serial.println(F("Setup complete - starting color cycle!"));
}
 
void loop() {
  // Check Wi-Fi and MQTT connections
  if (WiFi.status() != WL_CONNECTED) {
    startWifi();
  }
  if (!mqttClient.connected()) {
    reconnectMQTT();
  }
  mqttClient.loop();

  // 1. Red fade-in animation
  Serial.println(F("Fading Red..."));
  for (int n = 0; n < 50; n++) {
    send_colour_to_all((n * 4) + 50, 0, 0);
    mqttClient.loop(); // Keep MQTT connection alive
    delay(50);
  }

  // 2. Green fade-in animation
  Serial.println(F("Fading Green..."));
  for (int n = 0; n < 50; n++) {
    send_colour_to_all(0, (n * 4) + 50, 0);
    mqttClient.loop();
    delay(50);
  }

  // 3. Blue fade-in animation
  Serial.println(F("Fading Blue..."));
  for (int n = 0; n < 50; n++) {
    send_colour_to_all(0, 0, (n * 4) + 50);
    mqttClient.loop();
    delay(50);
  }

  // Pause at the end of the cycle
  delay(1000);
}

// Sets all 72 LEDs to the given (r, g, b) color and publishes to MQTT
void send_colour_to_all(int r, int g, int b) {
  if (mqttClient.connected()) {
    for (int pixel = 0; pixel < num_leds; pixel++) {
      RGBpayload[pixel * 3 + 0] = (byte)constrain(r, 0, 255);
      RGBpayload[pixel * 3 + 1] = (byte)constrain(g, 0, 255);
      RGBpayload[pixel * 3 + 2] = (byte)constrain(b, 0, 255);
    }
    mqttClient.publish(mqtt_topic.c_str(), RGBpayload, payload_size);
  } else {
    Serial.println(F("MQTT client not connected, cannot publish."));
  }
}

// Turns all 72 LEDs off (black)
void send_all_off() {
  send_colour_to_all(0, 0, 0);
}
