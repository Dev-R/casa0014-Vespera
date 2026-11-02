/*
 * mood_dial.ino
 *
 * Purpose:
 * Main program for the Vespera Mood Dial interface.
 *
 * Functionality:
 * - Initializes the hardware, WiFi, MQTT connection, OLED display,
 *   and LED system.
 * - Coordinates mood selection, vote submission, vote fading,
 *   screen updates, and LED updates.
 * - Stores the shared variables and settings used across all tabs.
 *
 * Project Structure:
 * - dial.ino         Reads the dial and selects a mood.
 * - votes.ino        Stores votes and handles vote fading.
 * - lights.ino       Builds and publishes the LED display.
 * - screen.ino       Controls the OLED screen.
 * - connections.ino Handles WiFi and MQTT connections.
 * - RGBLED.ino       Controls the onboard status LED.
 *
 * User Flow:
 * 1. User turns the dial to select a mood.
 * 2. User presses the button to check in.
 * 3. The vote is stored if the cooldown period has passed.
 * 4. LEDs update to reflect the new vote.
 * 5. The OLED confirms the action and shows live vote counts.
 * 6. Votes automatically fade after 30 minutes.
 *
 * Notes:
 * - This file acts as the system controller.
 * - Other tabs handle specific tasks and are compiled together
 *   as a single Arduino program.
 * 
 * Base WiFi/MQTT code: Duncan Wilson (CASA)
 * https://github.com/ucl-casa-ce/casa0014/tree/c900cddbd06d88c26d0962312b30b779e8cc97f2/vespera
 */

#include <SPI.h>
#include <WiFiNINA.h>
#include <PubSubClient.h>
#include "arduino_secrets.h"
#include <utility/wifi_drv.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Forward declaration for display object defined in screen.ino
extern Adafruit_SSD1306 display;

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
String lightId = "20";

// MQTT topic to publish color data to: "student/CASA0014/luminaire/<lightId>"
String mqtt_topic = "student/CASA0014/luminaire/" + lightId;            
String clientId = ""; // Unique client ID based on MAC address

// NeoPixel Configuration (72 LEDs * 3 bytes for RGB = 216 bytes)
const int num_leds = 72;
const int payload_size = num_leds * 3; // x3 for RGB

// Create the byte array to send in MQTT payload
byte RGBpayload[payload_size];


// Tunable settings
const unsigned long FADE_MS     = 30UL * 60UL * 1000UL;  // vote lifetime: 30 min
const unsigned long COOLDOWN_MS = 5UL * 1000UL;         // min gap between check-ins: 5 s
const int HYST    = 40;                                  // dial anti-flicker dead band
const int BRIGHT  = 120;                                 // LED brightness (0-255)

// Pin configuration
const int POT_PIN    = A1;
const int BUTTON_PIN = 4;

// Mood model configuration
const int N_MOODS   = 12;
const int VOTES_PER = 6;      // LEDs per blade
const int ADC_MAX   = 4096;

const char* MOODS[N_MOODS] = {
  "Tired", "Bored", "Sad",
  "Frustrated", "Stressed", "Anxious",
  "Energised", "Excited", "Happy",
  "Content", "Relaxed", "Calm"
};
// Zone colour per mood (groups of 3): blue / red / yellow / green
const byte ZONE_RGB[4][3] = {
  {  40,  90, 200 },   // 0-2  blue   (low / down)
  { 210,  50,  40 },   // 3-5  red    (tense)
  { 230, 180,  30 },   // 6-8  yellow (lively)
  {  60, 190,  90 }    // 9-11 green  (at ease)
};

// Shared state
unsigned long voteTime[N_MOODS][VOTES_PER];   // when each vote slot was cast (0 = empty)
int mood = -1;                                // current dial mood
unsigned long lastPress = 0;                  // for cooldown

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  analogReadResolution(12);

  // Status LEDs on MKR1010 NINA module
  WiFiDrv::pinMode(25, OUTPUT); // R
  WiFiDrv::pinMode(26, OUTPUT); // G
  WiFiDrv::pinMode(27, OUTPUT); // B
  LedRed();

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Initialize the OLED display with I2C address 0x3C

  // Generate a unique client ID using the device's MAC address
  byte mac[6];
  WiFi.macAddress(mac);
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02X%02X%02X%02X%02X%02X", mac[5], mac[4], mac[3], mac[2], mac[1], mac[0]);
  clientId = "MKR1010_Sender_" + String(macStr);

  // Print device and MQTT info to serial (Debug)
  Serial.print(F("Device Client ID: ")) ;
  Serial.println(clientId);
  Serial.print(F("Publishing to topic: "));
  Serial.println(mqtt_topic);

  
  // Connect to WiFi
  startWifi();

  // Connect to MQTT broker
  mqttClient.setServer(mqtt_server, mqtt_port);
  mqttClient.setBufferSize(512);
  mqttClient.setKeepAlive(30);


  reconnectMQTT();     // connect BEFORE drawing, so the sweep/clear actually lands
  startupSweep();      // sweep one LED round the dome, then clear to blank
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

  // 1. dial -> mood
  mood = applyHysteresis(readPot(), mood);

  // 2. button (active low) with cooldown
  static bool lastBtn = false;
  bool btn = !digitalRead(BUTTON_PIN);
  bool justPressed = btn && !lastBtn;
  lastBtn = btn;

  const char* flash = NULL;
  if (justPressed) {
    if (millis() - lastPress < COOLDOWN_MS) {
      flash = "wait...";
    } else {
      lastPress = millis();
      if (addVote(mood)) { flash = "Saved"; publishLights(); }
      else               { flash = "blade full"; }
    }
  }

  // 3. fade: redraw if any vote expired
  if (expireOld()) publishLights();

  showScreen(flash);
  if (flash) delay(600);
  delay(40);
}
