#include <Adafruit_NeoPixel.h>
#include <WiFi.h>
#include <ArduinoOTA.h>
#include "time.h"
#include "config.h"
#include "secrets.h"

// Solar globals defined in solar.ino
extern float sunriseMins;
extern float sunsetMins;
extern float dawnMins;
extern float duskMins;

// -- Hardware -------------------------------------------------
#define NUMPIXELS   255
#define PIN          16

// -- Timezone POSIX strings -----------------------------------
const char* TZ_EAST  = "EST5EDT,M3.2.0,M11.1.0";
const char* TZ_WEST  = "PST8PDT,M3.2.0,M11.1.0";
const char* TZ_CHINA = "CST-8";

// -- NTP ------------------------------------------------------
const char* ntpServer              = "pool.ntp.org";
const unsigned long SYNC_INTERVAL  = 6UL * 3600 * 1000;
const unsigned long RETRY_INTERVAL = 30UL * 1000;
const unsigned long WIFI_TIMEOUT   = 15UL * 1000;
unsigned long lastSync             = 0;
unsigned long lastRetry            = 0;
bool          timeSynced           = false;
bool          otaInProgress        = false;

// -- Solar ----------------------------------------------------
int  lastSolarDay  = -1;

// -- WiFi credentials -----------------------------------------
const char* ssid     = WIFI_SSID;
const char* password = WIFI_PASS;

// -- 7-segment digit patterns ---------------------------------
const byte digits[13][14] = {
  {1,1,1,1,0,1,1,1,1,1,1,0,1,1},
  {0,0,0,0,0,0,0,0,0,1,1,0,1,1},
  {1,1,1,1,1,0,0,1,1,1,1,1,0,0},
  {1,1,0,0,1,0,0,1,1,1,1,1,1,1},
  {0,0,0,0,1,1,1,0,0,1,1,1,1,1},
  {1,1,0,0,1,1,1,1,1,0,0,1,1,1},
  {1,1,1,1,1,1,1,1,1,0,0,1,1,1},
  {0,0,0,0,0,0,0,1,1,1,1,0,1,1},
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1},
  {1,1,0,0,1,1,1,1,1,1,1,1,1,1},
  {1,1,1,1,0,1,1,1,1,1,1,0,1,1},
  {1,1,1,1,0,1,1,1,1,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0}
};

// -- Display state --------------------------------------------
bool  Dot         = true;
int   last_digit  = 0;
int   cursor, cursor1;
int   digit_value  = 0;
int   digit_number = 0;
int   hour1, hour2, minute1, minute2;
int   minute2_now  = 0;
int   faceID;

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

// -- Color state ----------------------------------------------
int   digit_color[3][3] = {{0,0,10},{10,0,10},{10,0,0}};
int   color_red       = 50;
int   color_blue      = 50;
int   color_green     = 50;
int   color_scale     = 5;  // 1-5, default max
float color_intensity = 1;

int   hour_total, minute_total;
int   minutes_day, minutes_day_now = 0;
int   minutes_map;
bool  is_am = true;

// -- Touch inputs ---------------------------------------------
int touch_sensor_value3 = 0;
int touch_sensor_value5 = 0;
int touch_sensor_value7 = 0;
int touch_sensor_value9 = 0;

// -- OTA ------------------------------------------------------
int otaProgress = 0;

// =============================================================
//  wifiError()
// =============================================================
void wifiError() {
  Serial.println("WiFi error: showing error pattern.");
  for (int pulse = 0; pulse < 3; pulse++) {
    pixels.clear();
    for (int i = 0; i < NUMPIXELS; i += 5) {
      pixels.setPixelColor(i, pixels.Color(80, 0, 0));
    }
    pixels.show(); delay(300);
    pixels.clear(); pixels.show(); delay(200);
  }
  pixels.setPixelColor(0,   pixels.Color(40, 0, 0));
  pixels.setPixelColor(85,  pixels.Color(40, 0, 0));
  pixels.setPixelColor(170, pixels.Color(40, 0, 0));
  pixels.show();
}

// =============================================================
//  otaProgressBar()
// =============================================================
void otaProgressBar(int percent) {
  pixels.clear();
  int ledsLit = map(percent, 0, 100, 0, NUMPIXELS);
  for (int i = 0; i < ledsLit; i++) {
    pixels.setPixelColor(i, i == ledsLit-1
      ? pixels.Color(0, 100, 255)
      : pixels.Color(0, 20, 60));
  }
  pixels.show();
}

// =============================================================
//  connectWiFi()
// =============================================================
bool connectWiFi() {
  if (WiFi.status() == WL_CONNECTED) return true;
  Serial.print("WiFi connecting");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED) {
    if (millis() - start > WIFI_TIMEOUT) { Serial.println(" TIMEOUT"); return false; }
    delay(500); Serial.print(".");
  }
  Serial.print(" OK  IP: "); Serial.println(WiFi.localIP());
  return true;
}

// =============================================================
//  syncNTP()
// =============================================================
bool syncNTP() {
  if (WiFi.status() != WL_CONNECTED) {
    if (!connectWiFi()) return false;
  }
  Serial.print("NTP syncing...");
  configTime(0, 0, ntpServer);
  delay(1500);
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) { Serial.println(" FAILED"); return false; }
  Serial.println(" OK");
  lastSync = millis(); lastRetry = millis(); timeSynced = true;
  return true;
}

// =============================================================
//  setupOTA()
// =============================================================
void setupOTA() {
  ArduinoOTA.setHostname("clock742");
  ArduinoOTA.setPassword(OTA_PASS);

  ArduinoOTA.onStart([]() {
    otaInProgress = true;
    Serial.println("OTA start");
    pixels.clear();
    for (int i = 0; i < NUMPIXELS; i++) pixels.setPixelColor(i, pixels.Color(0, 0, 40));
    pixels.show();
    otaProgress = 0;
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    int pct = (progress * 100) / total;
    if (pct != otaProgress) { otaProgress = pct; otaProgressBar(pct); }
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("OTA complete.");
    pixels.fill(pixels.Color(0, 80, 0)); pixels.show(); delay(500);
  });
  ArduinoOTA.onError([](ota_error_t error) {
    otaInProgress = false;
    pixels.fill(pixels.Color(80, 0, 0)); pixels.show(); delay(1000);
    pixels.clear(); pixels.show();
  });
  ArduinoOTA.begin();
  Serial.println("OTA ready. Hostname: clock742");
}

// =============================================================
//  bootAnimation()
//  Randomly blinks LEDs across all three 9x3 matrix sections
//  while WiFi connects and NTP syncs. Stops when timeSynced.
// =============================================================
void bootAnimation() {
  uint32_t colors[] = {
    pixels.Color(80,  0,  120),   // violet
    pixels.Color(0,   60, 180),   // blue
    pixels.Color(0,   160, 160),  // teal
    pixels.Color(160, 80,  0),    // amber
    pixels.Color(180, 30,  0),    // orange
    pixels.Color(60,  160, 255),  // light blue
  };
  int numColors = 6;
  unsigned long lastFrame = 0;

  // Attempt WiFi + NTP while animating
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  unsigned long wifiStart = millis();
  bool wifiOk = false;

  while (!timeSynced) {
    // Animate at ~10fps without blocking
    if (millis() - lastFrame >= 100) {
      lastFrame = millis();
      pixels.clear();
      for (int face = 0; face < 3; face++) {
        int faceOffset = face * 85;
        for (int n = 0; n < 6; n++) {
          int col     = random(9);
          int row     = random(3);
          int physCol = 8 - col;
          int baseIdx = physCol * 3;
          int ledRow  = (physCol % 2 == 0) ? row : (2 - row);
          pixels.setPixelColor(faceOffset + baseIdx + ledRow, colors[random(numColors)]);
        }
      }
      pixels.show();
    }

    // Check WiFi then attempt NTP once connected
    if (!wifiOk && WiFi.status() == WL_CONNECTED) {
      wifiOk = true;
      Serial.print(" OK  IP: "); Serial.println(WiFi.localIP());
    }

    if (wifiOk && !timeSynced) {
      configTime(0, 0, ntpServer);
      struct tm timeinfo;
      if (getLocalTime(&timeinfo)) {
        lastSync = millis(); lastRetry = millis(); timeSynced = true;
        Serial.println("NTP OK");
        setenv("TZ", TZ_EAST, 1); tzset();
        updateSolarTimes();
      }
    }

    // Timeout: if WiFi never connects show error and exit
    if (!wifiOk && millis() - wifiStart > WIFI_TIMEOUT) {
      Serial.println("WiFi timeout during boot");
      wifiError();
      break;
    }
  }

  pixels.clear();
  pixels.show();
}

// =============================================================
//  setup()
// =============================================================
void setup() {
  pixels.begin();
  pixels.setBrightness(BRIGHTNESS_DAY);
  pixels.clear();
  pixels.show();
  delay(500);

  Serial.begin(115200);

  randomSeed(analogRead(0));

  // Run boot animation while WiFi connects and NTP syncs in parallel
  bootAnimation();
  setupOTA();
}

// =============================================================
//  loop()
// =============================================================
void loop() {
  ArduinoOTA.handle();
  if (otaInProgress) return;

  touch_inputs();

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi lost. Reconnecting...");
    connectWiFi();
  }

  if (!timeSynced) {
    if (millis() - lastRetry > RETRY_INTERVAL) {
      lastRetry = millis();
      bool synced = syncNTP();
      if (synced) {
        setenv("TZ", TZ_EAST, 1); tzset();
        updateSolarTimes();
        pixels.clear(); pixels.show();
      } else { wifiError(); }
    }
    return;
  }

  if (millis() - lastSync > SYNC_INTERVAL) {
    syncNTP();
  }

  // Read home TZ time into a local var -- do NOT use minutes_day
  // global here because printLocalTime() inside renderFace() will
  // overwrite it with whatever TZ that face uses.
  int home_minutes;
  {
    struct tm timeinfo;
    setenv("TZ", TZ_EAST, 1); tzset();
    if (!getLocalTime(&timeinfo)) return;
    home_minutes = timeinfo.tm_hour * 60 + timeinfo.tm_min;
  }

  if (home_minutes != minutes_day_now) {
    // Recalculate solar times once per day
    struct tm timeinfo;
    setenv("TZ", TZ_EAST, 1); tzset();
    if (getLocalTime(&timeinfo)) {
      if (timeinfo.tm_yday != lastSolarDay) {
        updateSolarTimes();
        lastSolarDay = timeinfo.tm_yday;
      }
    }

    pixels.setBrightness(solarBrightness());
    face2();
    face1();
    face0();
    minutes_day_now = home_minutes;
  }
}

// =============================================================
//  renderFace()
// =============================================================
void renderFace(int id, const char* tz) {
  faceID = id;
  setenv("TZ", tz, 1);
  tzset();
  printLocalTime();

  int hour12 = hour_total % 12;
  if (hour12 == 0) hour12 = 12;
  hour1 = hour12 / 10;
  hour2 = hour12 % 10;
  is_am = (hour_total < 12);

  colors_assign();

  // Snapshot this face's state BEFORE rendering digits
  int  snap_r    = color_red;
  int  snap_g    = color_green;
  int  snap_b    = color_blue;
  bool snap_am   = is_am;
  int  snap_face = faceID;

  digit_number = 0; digit_value = hour1;   digit_assign();
  digit_number = 1; digit_value = hour2;   digit_assign();
  digit_number = 2; digit_value = minute1; digit_assign();
  digit_number = 3; digit_value = minute2; digit_assign();

  // Restore face state, write colon last after all digit animation
  color_red = snap_r; color_green = snap_g; color_blue = snap_b;
  is_am     = snap_am;
  faceID    = snap_face;

  dots_assign();
}

// Write AM/PM column (col 8) for a face -- called after matrix to avoid
// digit animation overwriting it. Uses is_am and faceID from snapshot.
void ampmColumn(bool face_is_am, int face_id, bool useMatrixLED1) {
  int faceOffset = face_id * 85;
  uint32_t ampmColor = face_is_am
    ? pixels.Color((uint8_t)(60  * MATRIX_BRIGHTNESS), (uint8_t)(160 * MATRIX_BRIGHTNESS), (uint8_t)(255 * MATRIX_BRIGHTNESS))
    : pixels.Color((uint8_t)(220 * MATRIX_BRIGHTNESS), 0, 0);
  for (int row = 0; row < 3; row++) {
    int physCol = 8;  // col 8 -> physCol = 8-8 = 0
    int baseIdx = 0;
    int ledRow  = row;  // physCol 0 is even, no flip
    pixels.setPixelColor(faceOffset + baseIdx + ledRow, ampmColor);
  }
}

// =============================================================
//  Clock faces
// =============================================================
void face0() {
  renderFace(0, TZ_CHINA);
  matrix_assign_2(color_red, color_green, color_blue);
  ampmColumn(is_am, 0, false);
  pixels.show();
}
void face1() {
  renderFace(1, TZ_EAST);
  matrix_assign(color_red, color_green, color_blue);
  ampmColumn(is_am, 1, true);
  pixels.show();
}
void face2() {
  renderFace(2, TZ_WEST);
  matrix_assign_2(color_red, color_green, color_blue);
  ampmColumn(is_am, 2, false);
  pixels.show();
}

// =============================================================
//  printLocalTime()
// =============================================================
void printLocalTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) { Serial.println("Failed to obtain time"); return; }
  hour_total   = timeinfo.tm_hour;
  minute_total = timeinfo.tm_min;
  hour2   = timeinfo.tm_hour % 10;
  hour1   = timeinfo.tm_hour / 10;
  minute2 = timeinfo.tm_min  % 10;
  minute1 = timeinfo.tm_min  / 10;
  minutes_day = timeinfo.tm_hour * 60 + timeinfo.tm_min;
  minutes_map = map(minutes_day, 0, 1440, 0, 255);
}
