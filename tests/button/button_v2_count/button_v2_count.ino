// button_v2_count.ino
// Button test v2: counts presses with edge detection + debounce.
// Wiring: button between D4 and GND.

const int BUTTON_PIN = 4;
const unsigned long DEBOUNCE_MS = 25;   // ignore changes faster than this

bool lastStable = false;        // last debounced state (true = pressed)
bool lastReading = false;       // most recent raw reading
unsigned long lastChange = 0;   // when the raw reading last changed
unsigned long presses = 0;      // total clean presses

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  Serial.println("Press the button. Count prints on each new press.");
}

void loop() {
  bool reading = !digitalRead(BUTTON_PIN);   // pressed = true

  // If the raw reading changed, start the debounce timer.
  if (reading != lastReading) {
    lastChange = millis();
    lastReading = reading;
  }

  // Once the reading has held steady past DEBOUNCE_MS, accept it as the new stable state.
  if (millis() - lastChange > DEBOUNCE_MS) {
    if (reading != lastStable) {
      lastStable = reading;
      if (lastStable) {            // count only on the press, not the release
        presses++;
        Serial.print("Press ");
        Serial.println(presses);
      }
    }
  }
}
