// matrix_assign_2.ino -- face0 (China) and face2 (West Coast)
// Same as matrix_assign with AM/PM indicator on col 8.

int matrixLED2(int col, int row, int faceOffset) {
  int physCol = 8 - col;
  int baseIdx = physCol * 3;
  int ledRow  = (physCol % 2 == 0) ? row : (2 - row);
  return faceOffset + baseIdx + ledRow;
}

void matrix_assign_2(int r, int g, int b) {
  int litLEDs    = 24 - hour_total;
  int faceOffset = faceID * 85;

  float scale     = MATRIX_BRIGHTNESS;  // constant brightness, no hourly fade

  uint32_t onColor = pixels.Color(
    (uint8_t)(r * scale),
    (uint8_t)(g * scale),
    (uint8_t)(b * scale)
  );

  // Cols 0-7: gravity drain
  int ledsToLight = litLEDs;
  for (int col = 0; col < 8; col++) {
    for (int row = 2; row >= 0; row--) {
      uint32_t c;
      if (ledsToLight-- > 0) {
        c = onColor;
      } else {
        c = (random(20) == 0)
          ? pixels.Color(
              (uint8_t)(8 * MATRIX_BRIGHTNESS),
              (uint8_t)(8 * MATRIX_BRIGHTNESS),
              (uint8_t)(8 * MATRIX_BRIGHTNESS))
          : 0;
      }
      pixels.setPixelColor(matrixLED2(col, row, faceOffset), c);
    }
  }
}
