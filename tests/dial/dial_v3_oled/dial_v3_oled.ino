// dial_v3_oled.ino
// Dial test v3: displays mood on OLED screen via I2C.
// Wiring: pot to A1, OLED SDA/SCL to pins 11/12.
// Libraries: Adafruit SSD1306, Adafruit GFX.

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

const int POT_PIN = A1;
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

int binOf(int raw) {
  return constrain(raw * N_MOODS / ADC_MAX, 0, N_MOODS - 1);
}

// Draw the current mood to the screen. m/3 picks the colour zone (moods 0-2 = zone 0, etc).
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
  display.display();          // nothing appears until this line sends it to the screen
}

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000);
  analogReadResolution(12);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {   // try 0x3D if nothing appears
    Serial.println("OLED not found. Check the wiring and the I2C address.");
  }
  Serial.println("raw\tmood");
}

void loop() {
  int raw = analogRead(POT_PIN);
  int mood = binOf(raw);

  showMood(mood, raw);

  Serial.print(raw);
  Serial.print('\t');
  Serial.println(MOODS[mood]);

  delay(100);
}
