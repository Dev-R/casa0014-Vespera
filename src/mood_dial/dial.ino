/*
 * dial.ino
 *
 * Purpose:
 * Handles mood selection using the rotary dial (potentiometer).
 *
 * Functionality:
 * - Reads the analog value from the potentiometer connected to POT_PIN.
 * - Converts the dial position into one of 12 mood categories.
 * - Applies signal smoothing to reduce electrical noise.
 * - Uses hysteresis to prevent rapid switching between adjacent moods
 *   when the dial is positioned near a boundary.
 *
 * Input Processing:
 * - Samples the potentiometer 8 times and averages the readings.
 * - Maps the ADC range (0–4095) into 12 equal mood segments.
 * - Each segment corresponds to a single mood on the mood wheel.
 *
 * Hysteresis Logic:
 * - Maintains the current mood while the dial reading remains within
 *   a tolerance range around the mood boundaries.
 * - Only changes mood when the reading moves beyond the hysteresis
 *   threshold (HYST).
 * - Prevents flickering and unstable mood selection caused by small
 *   fluctuations in analog readings.
 *
 * Notes:
 * - Responsible only for mood selection and stability.
 * - Does not handle display updates, vote storage, or LED rendering.
 */

// Average 8 readings to smooth out electrical noise.
int readPot() {
  long sum = 0;
  for (int i = 0; i < 8; i++) sum += analogRead(POT_PIN);
  return sum / 8;
}

// Split the pot's 0-4095 range into 12 equal slices, one per mood.
int binOf(int raw) {
  return constrain(raw * N_MOODS / ADC_MAX, 0, N_MOODS - 1);
}

// Only leave the current mood once the reading passes its edge plus HYST,
// so a reading sitting on a boundary can't bounce between two moods.
int applyHysteresis(int raw, int current) {
  if (current < 0) return binOf(raw);
  int lower = current * ADC_MAX / N_MOODS - HYST;
  int upper = (current + 1) * ADC_MAX / N_MOODS + HYST;
  if (raw < lower || raw > upper) return binOf(raw);
  return current;
}
