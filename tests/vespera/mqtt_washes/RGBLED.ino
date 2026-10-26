/*
 * RGBLED.ino
 * Purpose:
 * Controls the onboard RGB LED on the MKR1010 using WiFiDrv::analogWrite.
 * Adapted from the CASA0014 workshop sketch by Duncan Wilson (CASA, UCL).
 * Source: https://github.com/ucl-casa-ce/casa0014/blob/c900cddb/vespera/workshop-sketch/mkr1010_mqtt_simple/RGBLED.ino
 * Changes: Adapted to use WiFiDrv::analogWrite for controlling the RGB LED pins.
 * Accessed:  26 October 2026
 */

 
void toggleRGB() {
  // Red
  WiFiDrv::analogWrite(25, 255);
  WiFiDrv::analogWrite(26, 0);
  WiFiDrv::analogWrite(27, 0);
  delay(1000);

  // Green
  WiFiDrv::analogWrite(25, 0);
  WiFiDrv::analogWrite(26, 255);
  WiFiDrv::analogWrite(27, 0);
  delay(1000);

  // Blue
  WiFiDrv::analogWrite(25, 0);
  WiFiDrv::analogWrite(26, 0);
  WiFiDrv::analogWrite(27, 255);
  delay(1000);

  // Off
  WiFiDrv::analogWrite(25, 0);
  WiFiDrv::analogWrite(26, 0);
  WiFiDrv::analogWrite(27, 0);
  delay(1000);
}

void LedRed() {
  WiFiDrv::analogWrite(25, 155);
  WiFiDrv::analogWrite(26, 0);
  WiFiDrv::analogWrite(27, 0);
}

void LedGreen() {
  WiFiDrv::analogWrite(25, 0);
  WiFiDrv::analogWrite(26, 155);
  WiFiDrv::analogWrite(27, 0);
}

void LedBlue() {
  WiFiDrv::analogWrite(25, 0);
  WiFiDrv::analogWrite(26, 0);
  WiFiDrv::analogWrite(27, 155);
}