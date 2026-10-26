// dial_v2_mood.ino
// Dial test v2: maps potentiometer range to 12 moods, prints to Serial.
// Wiring: pot outer legs to 3.3V/GND, middle leg to A1.

const int POT_PIN = A1;
const int N_MOODS = 12;
const int ADC_MAX = 4096;

const char* MOODS[N_MOODS] = {
  "Tired", "Bored", "Sad",
  "Frustrated", "Stressed", "Anxious",
  "Energised", "Excited", "Happy",
  "Content", "Relaxed", "Calm"
};

// Turn a raw reading (0-4095) into a mood number (0-11).
// raw * 12 / 4096 lands in 0..11; constrain guards the very top value.
int binOf(int raw) {
  return constrain(raw * N_MOODS / ADC_MAX, 0, N_MOODS - 1);
}

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000);
  analogReadResolution(12);
  Serial.println("raw\tmood");
}

void loop() {
  int raw = analogRead(POT_PIN);
  int mood = binOf(raw);

  Serial.print(raw);
  Serial.print('\t');
  Serial.println(MOODS[mood]);

  delay(100);
}
