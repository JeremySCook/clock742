// touch_inputs.ino
// ESP32 capacitive touch: HIGH (~40k-60k) = idle, LOW (~3k-8k) = touched.
// color_scale adjustment is debounced here so it fires once per touch,
// not once per face render.

// Debounce: require touch to be released before firing again
bool t5_wasPressed = false;
bool t7_wasPressed = false;

void touch_inputs() {
  touch_sensor_value3 = touchRead(T3);
  touch_sensor_value5 = touchRead(T5);
  touch_sensor_value7 = touchRead(T7);
  touch_sensor_value9 = touchRead(T9);

  // Scale up -- only increment once per press
  if (touch_sensor_value5 < TOUCH_THRESHOLD) {
    if (!t5_wasPressed && color_scale < 5) {
      color_scale++;
      t5_wasPressed = true;
    }
  } else {
    t5_wasPressed = false;
  }

  // Scale down -- only decrement once per press
  if (touch_sensor_value7 < TOUCH_THRESHOLD) {
    if (!t7_wasPressed && color_scale > 1) {
      color_scale--;
      t7_wasPressed = true;
    }
  } else {
    t7_wasPressed = false;
  }
}
