// v3 -- non-blocking delays using millis() to avoid watchdog resets
void digit_assign() {
  int delay_between_numbers = 40;

  int baseOffset = faceID * 85;
  int digitStart;
  switch (digit_number) {
    case 0: digitStart = 27 + baseOffset; break;
    case 1: digitStart = 41 + baseOffset; break;
    case 2: digitStart = 57 + baseOffset; break;
    case 3: digitStart = 71 + baseOffset; break;
    default: return;
  }

  // Skip leading zero on hour tens digit
  if (digit_number == 0 && digit_value == 0) {
    // Clear digit 0 and leave blank
    for (int k = 0; k < 14; k++)
      pixels.setPixelColor(digitStart + k, pixels.Color(0, 0, 0));
    pixels.show();
    return;
  }

  for (int scroll_value = 0; scroll_value <= digit_value; scroll_value++) {
    cursor = digitStart;

    // Clear digit
    for (int k = 0; k < 14; k++)
      pixels.setPixelColor(digitStart + k, pixels.Color(0, 0, 0));
    pixels.show();

    // Draw digit
    unsigned long t = millis();
    while (millis() - t < delay_between_numbers) {
      ArduinoOTA.handle();  // keep OTA alive during animation
    }

    for (int k = 0; k < 14; k++) {
      if (digits[scroll_value][k] == 1)
        pixels.setPixelColor(cursor, pixels.Color(color_red, color_green, color_blue));
      else
        pixels.setPixelColor(cursor, pixels.Color(0, 0, 0));
      cursor++;
    }
    pixels.show();
  }
}
