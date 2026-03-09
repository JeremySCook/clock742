// matrix_assign.ino -- face1 (East Coast)
// 8x3 gravity drain, 1 LED per hour.
// Col 8 (rightmost): AM/PM indicator matching colon color, dimmed by hours remaining.

int matrixLED(int col, int row, int faceOffset) {
  int physCol = 8 - col;
  int baseIdx = physCol * 3;
  int ledRow  = (physCol % 2 == 0) ? row : (2 - row);
  return faceOffset + baseIdx + ledRow;
}

void matrix_assign(int r, int g, int b) {
  int litLEDs    = 24 - hour_total;
  int faceOffset = faceID * 85;

  float remaining = max(0.1f, (float)litLEDs / 24.0f);
  float scale     = MATRIX_BRIGHTNESS * remaining;

  uint32_t onColor = pixels.Color(
    (uint8_t)(r * scale),
    (uint8_t)(g * scale),
    (uint8_t)(b * scale)
  );

  // Cols 0-7: gravity drain
  int ledsToLight = litLEDs;
  for (int col = 0; col < 8; col++) {
    for (int row = 2; row >= 0; row--) {
      pixels.setPixelColor(
        matrixLED(col, row, faceOffset),
        ledsToLight-- > 0 ? onColor : 0
      );
    }
  }
}
