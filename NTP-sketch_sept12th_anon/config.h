// config.h -- user-tunable settings
// Edit these values, then OTA flash to update the clock.
// This file is safe to commit (no secrets).

#ifndef CONFIG_H
#define CONFIG_H

// Home location -- sunrise/sunset for face1 and brightness dimming.
// Default: Palm Harbor, FL
#define HOME_LAT  28.08
#define HOME_LON -82.73

// Digit brightness limits (0-255).
#define BRIGHTNESS_DAY   70
#define BRIGHTNESS_NIGHT 40

// Matrix brightness: 0.0-1.0, relative to digit brightness.
#define MATRIX_BRIGHTNESS 0.7f

// Touch threshold: ESP32 reads HIGH (~40k-60k) idle, LOW (~3k-8k) touched.
// Raise this value if accidental triggers occur on floating pins.
#define TOUCH_THRESHOLD 8000   // Lower = less sensitive. Raise if false triggers occur.

#endif // CONFIG_H
