// dial_v4_hysteresis.ino
// Dial test v4: adds hysteresis to prevent mood flickering at boundaries.
// Wiring: same as v3. Set HYST to 0 to see flickering, 40 to prevent it.

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

const int POT_PIN = A1;
const int HYST = 40;        // dead band around each boundary, in ADC counts (0 = off)
const int N_MOODS = 12;
const int ADC_MAX = 4096;

const char* MOODS[N_MOODS] = {
  "Tired", "Bored", "Sad",
  "Frustrated", "Stressed", "Anxious",
  "Energised", "Excited", "Happy",
  "Content", "Relaxed", "Calm"
};
const char* ZONES[4] = { "Blue zone", "Red zone", "Yellow zone", "Green zone" };

Adafruit_SSD1306 display(128, 64, &Wire, -1);

int mood = -1;              // -1 until the first reading
unsigned long changes = 0;  // counts mood changes, for the flicker test

// Average 8 readings to smooth out electrical noise (new in v4).
int readPot() {
  long sum = 0;
  for (int i = 0; i < 8; i++) sum += analogRead(POT_PIN);
  return sum / 8;
}

int binOf(int raw) {
  return constrain(raw * N_MOODS / ADC_MAX, 0, N_MOODS - 1);
}

// Only leave the current mood once the reading passes its edge plus HYST.
// With no current mood yet (-1), just take the plain slice.
int applyHysteresis(int raw, int current) {
  if (current < 0) return binOf(raw);
  int lower = current * ADC_MAX / N_MOODS - HYST;
  int upper = (current + 1) * ADC_MAX / N_MOODS + HYST;
  if (raw < lower || raw >= upper) return binOf(raw);
  return current;
}

void showMood(int m, int raw) {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("How are you feeling?");
  display.setTextSize(2);
  display.setCursor(0, 22);
  display.print(MOODS[m]);
  display.setTextSize(1);
  display.setCursor(0, 54);
  display.print(ZONES[m / 3]);
  display.setCursor(100, 54);
  display.print(raw);
  display.display();
}

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000);
  analogReadResolution(12);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {   // try 0x3D if nothing appears
    Serial.println("OLED not found. Check the wiring and the I2C address.");
  }
  Serial.println("raw\tmood\tchanges");
}

void loop() {
  int raw = readPot();
  int next = applyHysteresis(raw, mood);
  if (next != mood) {
    if (mood >= 0) changes++;   // don't count the very first mood as a change
    mood = next;
  }

  showMood(mood, raw);

  Serial.print(raw);
  Serial.print('\t');
  Serial.print(MOODS[mood]);
  Serial.print('\t');
  Serial.println(changes);

  delay(50);
}
