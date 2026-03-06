// dots_assign.ino
// Draws the two separator dots between HH and MM.
// fix #9: dot color indicates AM (cool blue) vs PM (warm amber)
void dots_assign() {
  // AM = cool blue, PM = warm amber
  uint32_t dotColor = is_am
    ? pixels.Color(0, 80, 180)    // AM: cool blue
    : pixels.Color(200, 80, 0);   // PM: warm amber

  int dot1 = 55 + faceID * 85;
  int dot2 = 56 + faceID * 85;

  // Brief blink effect
  pixels.setPixelColor(dot1, pixels.Color(0, 0, 0));
  pixels.setPixelColor(dot2, pixels.Color(0, 0, 0));
  pixels.show();
  delay(20);

  pixels.setPixelColor(dot1, dotColor);
  pixels.setPixelColor(dot2, dotColor);
  pixels.show();
}
