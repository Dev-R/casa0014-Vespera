// dial_v1_raw.ino
// Dial test v1: prints raw potentiometer value (0-4095) to Serial.
// Wiring: pot outer legs to 3.3V/GND, middle leg to A1.

const int POT_PIN = A1;

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000);   // wait briefly for the Serial Monitor
  analogReadResolution(12);             // readings run 0 to 4095 instead of 0 to 1023
}

void loop() {
  int raw = analogRead(POT_PIN);
  Serial.println(raw);
  delay(100);
}
