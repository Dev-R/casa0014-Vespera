/*
 * screen.ino
 *
 * Purpose:
 * Handles the OLED display for the mood check-in device.
 *
 * Functionality:
 * - Displays the currently selected mood in large text.
 * - Shows temporary status messages such as:
 *     • "Saved"
 *     • "Wait..."
 *     • "Blade Full"
 * - Displays the live vote count for the selected mood when no
 *   status message is active.
 */

Adafruit_SSD1306 display(128, 64, &Wire, -1);

void showScreen(const char* flash) {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("How are you feeling?");

  display.setTextSize(2);
  display.setCursor(0, 20);
  display.print(mood >= 0 ? MOODS[mood] : "...");

  display.setTextSize(1);
  display.setCursor(0, 46);
  if (flash) {
    display.print(flash);
  } else {
    display.print("votes: ");
    display.print(mood >= 0 ? liveVotes(mood) : 0);
  }
  display.display();
}
