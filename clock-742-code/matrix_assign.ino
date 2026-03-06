// matrix_assign.ino
// Renders the 27-LED background matrix for face1 (East Coast).
// Fixed: removed dangling "pixels." syntax error from original.
void matrix_assign() {
  cursor = 0 + faceID * 85;
  for (int k = 0; k <= 26; k++) {
    int randNumber  = random(2);
    int randNumber1 = random(27);
    int randNumber2 = random(54);

    pixels.setPixelColor(cursor, Wheel(minutes_map));

    if (randNumber == 1) {         // random off pixel (1/2 chance)
      pixels.setPixelColor(cursor, 0);
    }
    if (randNumber1 == k) {        // accent color (1/27 chance)
      pixels.setPixelColor(cursor, Wheel(minutes_map - 25));
    }
    if (randNumber2 == k) {        // secondary accent (1/54 chance)
      pixels.setPixelColor(cursor, Wheel(minutes_map - 75));
    }
    cursor++;
  }
  pixels.show();
}
