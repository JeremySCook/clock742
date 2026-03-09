// dots_assign.ino
// Separator colon between HH and MM.
// AM = light blue, PM = red
// No pixels.show() here -- committed by the final show() in face0/1/2.

void dots_assign() {
  uint32_t dotColor = is_am
    ? pixels.Color(60, 160, 255)   // AM: light blue
    : pixels.Color(220, 0, 0);     // PM: red

  int dot1 = 55 + faceID * 85;
  int dot2 = 56 + faceID * 85;

  pixels.setPixelColor(dot1, dotColor);
  pixels.setPixelColor(dot2, dotColor);
}
