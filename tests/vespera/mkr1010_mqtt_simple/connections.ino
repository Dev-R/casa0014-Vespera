/*
 * connections.ino
 * Purpose:
 * Handles WiFi and MQTT connections.
 * Taken from the CASA0014 workshop sketch by Duncan Wilson (CASA, UCL).
 * Source: https://github.com/ucl-casa-ce/casa0014/blob/c900cddb/vespera/workshop-sketch/mkr1010_mqtt_simple/connections.ino
 * Accessed:  26 October 2026
 */

// Connect wifi and print setup info
void startWifi() {
  LedBlue();
  Serial.print(F("Connecting to WiFi: "));
  Serial.println(ssid);

  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println(F("Communication with WiFi module failed!"));
    while (true)
      ;
  }

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 5) {
    WiFi.begin(ssid, password);
    Serial.print(F("."));
    delay(3000);
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println(F("\nWiFi connected!"));
    Serial.print(F("IP address: "));
    Serial.println(WiFi.localIP());
    LedGreen();
  } else {
    Serial.println(F("\nWiFi connection failed, will retry in loop."));
    LedRed();
  }
}

// Handle reconnects to MQTT broker
void reconnectMQTT() {
  LedBlue();
  wifiClient.stop();

  if (!mqttClient.connected()) {
    Serial.println(F("Connecting to MQTT..."));
    if (mqttClient.connect(clientId.c_str(), mqtt_username, mqtt_password)) {
      Serial.println(F("connected"));
      LedGreen();
    } else {
      Serial.print(F("failed, rc="));
      Serial.print(mqttClient.state());
      Serial.println(F(" try again in 5 seconds"));
      LedRed();
      delay(5000);
    }
  }
}