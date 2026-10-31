/*
 * lights.ino
 *
 * Purpose:
 * Generates the 72-LED visual state of the Vespera dome and publishes
 * the LED data to the lighting controller.
 *
 * Functionality:
 * - Converts the current vote grid into an RGB LED representation.
 * - Maps votes to LED positions on the dome.
 * - Builds the complete 72-LED colour payload.
 * - Publishes the payload to Vespera for display.
 *
 * LED Mapping:
 * - LED index = (mood × 6) + slot
 * - Each mood occupies 6 LED positions.
 * - Inner slots are filled before outer slots.
 *
 * Colour Logic:
 * - Each mood belongs to one of four mood zones:
 *     • Zone 0: Moods 0-2
 *     • Zone 1: Moods 3-5
 *     • Zone 2: Moods 6-8
 *     • Zone 3: Moods 9-11
 * - The RGB colour for each zone is defined in ZONE_RGB[zone].
 */

// Build the whole dome from live votes and publish it.
void publishLights() {
  for (int i = 0; i < payload_size; i++) RGBpayload[i] = 0;   // start all off
  unsigned long now = millis();

  for (int m = 0; m < N_MOODS; m++) {
    const byte* c = ZONE_RGB[m / 3];
    int slot = 0;                                   // live votes pack from the centre out
    for (int s = 0; s < VOTES_PER; s++) {
      if (voteTime[m][s] != 0 && now - voteTime[m][s] < FADE_MS) {
        int led = m * 6 + slot;
        RGBpayload[led * 3 + 0] = (byte)(c[0] * BRIGHT / 128);
        RGBpayload[led * 3 + 1] = (byte)(c[1] * BRIGHT / 128);
        RGBpayload[led * 3 + 2] = (byte)(c[2] * BRIGHT / 128);
        slot++;
      }
    }
  }
  if (mqttClient.connected())
    mqttClient.publish(mqtt_topic.c_str(), RGBpayload, payload_size);
}

// Startup: sweep one white LED around the dome, then clear to blank.
// Proves the system controls all 72 LEDs and wipes whatever the previous
// user left lit. Also a quick dead-pixel check.
void startupSweep() {
  for (int i = 0; i < num_leds; i++) {
    for (int j = 0; j < payload_size; j++) RGBpayload[j] = 0;
    RGBpayload[i * 3 + 0] = BRIGHT;
    RGBpayload[i * 3 + 1] = BRIGHT;
    RGBpayload[i * 3 + 2] = BRIGHT;
    if (mqttClient.connected())
      mqttClient.publish(mqtt_topic.c_str(), RGBpayload, payload_size);
    mqttClient.loop();
    delay(25);
  }
  for (int j = 0; j < payload_size; j++) RGBpayload[j] = 0;   // clear to blank
  if (mqttClient.connected())
    mqttClient.publish(mqtt_topic.c_str(), RGBpayload, payload_size);
}
