// button_v1_read.ino
// Button test v1: prints 1 when pressed, 0 when not.
// Wiring: button between D4 and GND (uses INPUT_PULLUP).

const int BUTTON_PIN = 4;

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
}

void loop() {
  bool pressed = !digitalRead(BUTTON_PIN);  // ! flips it so pressed = 1
  Serial.println(pressed);
  delay(100);
}
