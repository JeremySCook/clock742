/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-date-time-ntp-client-server-arduino/

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

#include <Adafruit_NeoPixel.h>
#include <WiFi.h>
#include <ArduinoOTA.h>
#include "time.h"
#include "secrets.h"

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
bool          otaInProgress        = false;  // blocks WiFi watchdog during OTA

// -- WiFi credentials -----------------------------------------
const char* ssid     = WIFI_SSID;
const char* password = WIFI_PASS;

// -- 7-segment digit patterns ---------------------------------
const byte digits[13][14] = {
  {1,1,1,1,0,1,1,1,1,1,1,0,1,1},  // 0
  {0,0,0,0,0,0,0,0,0,1,1,0,1,1},  // 1
  {1,1,1,1,1,0,0,1,1,1,1,1,0,0},  // 2
  {1,1,0,0,1,0,0,1,1,1,1,1,1,1},  // 3
  {0,0,0,0,1,1,1,0,0,1,1,1,1,1},  // 4
  {1,1,0,0,1,1,1,1,1,0,0,1,1,1},  // 5
  {1,1,1,1,1,1,1,1,1,0,0,1,1,1},  // 6
  {0,0,0,0,0,0,0,1,1,1,1,0,1,1},  // 7
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1},  // 8
  {1,1,0,0,1,1,1,1,1,1,1,1,1,1},  // 9
  {1,1,1,1,0,1,1,1,1,1,1,0,1,1},  // *0
  {1,1,1,1,0,1,1,1,1,0,0,0,0,0},  // C (11)
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0}   // blank (12)
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
int   digit_color[3][3] = {
  {0,  0,  10},
  {10, 0,  10},
  {10, 0,   0}
};
int   color_red       = 50;
int   color_blue      = 50;
int   color_green     = 50;
int   color_scale     = 5;
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

// -- OTA state ------------------------------------------------
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
    pixels.show();
    delay(300);
    pixels.clear();
    pixels.show();
    delay(200);
  }
  pixels.setPixelColor(0,   pixels.Color(40, 0, 0));
  pixels.setPixelColor(85,  pixels.Color(40, 0, 0));
  pixels.setPixelColor(170, pixels.Color(40, 0, 0));
  pixels.show();
}

// =============================================================
//  otaProgressBar() -- blue sweep during OTA upload
// =============================================================
void otaProgressBar(int percent) {
  pixels.clear();
  int ledsLit = map(percent, 0, 100, 0, NUMPIXELS);
  for (int i = 0; i < ledsLit; i++) {
    if (i == ledsLit - 1) {
      pixels.setPixelColor(i, pixels.Color(0, 100, 255));  // bright blue tip
    } else {
      pixels.setPixelColor(i, pixels.Color(0, 20, 60));    // dim blue trail
    }
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
    if (millis() - start > WIFI_TIMEOUT) {
      Serial.println(" TIMEOUT");
      return false;
    }
    delay(500);
    Serial.print(".");
  }
  Serial.print(" OK  IP: ");
  Serial.println(WiFi.localIP());
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
  if (!getLocalTime(&timeinfo)) {
    Serial.println(" FAILED");
    return false;
  }
  Serial.println(" OK");
  lastSync   = millis();
  lastRetry  = millis();
  timeSynced = true;
  return true;
}

// =============================================================
//  setupOTA()
// =============================================================
void setupOTA() {
  ArduinoOTA.setHostname("clock742");
  ArduinoOTA.setPassword(OTA_PASS);

  ArduinoOTA.onStart([]() {
    otaInProgress = true;  // stop WiFi watchdog and wifiError() interfering
    String type = (ArduinoOTA.getCommand() == U_FLASH) ? "firmware" : "filesystem";
    Serial.println("OTA start: " + type);
    pixels.clear();
    for (int i = 0; i < NUMPIXELS; i++) {
      pixels.setPixelColor(i, pixels.Color(0, 0, 40));  // solid dim blue
    }
    pixels.show();
    otaProgress = 0;
  });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    int pct = (progress * 100) / total;
    if (pct != otaProgress) {
      otaProgress = pct;
      Serial.printf("OTA: %d%%\n", pct);
      otaProgressBar(pct);
    }
  });

  ArduinoOTA.onEnd([]() {
    Serial.println("OTA complete. Rebooting...");
    pixels.fill(pixels.Color(0, 80, 0));  // flash green on success
    pixels.show();
    delay(500);
    // otaInProgress stays true -- device reboots immediately after
  });

  ArduinoOTA.onError([](ota_error_t error) {
    otaInProgress = false;  // allow normal operation to resume
    Serial.printf("OTA error [%u]\n", error);
    pixels.fill(pixels.Color(80, 0, 0));
    pixels.show();
    delay(1000);
    pixels.clear();
    pixels.show();
  });

  ArduinoOTA.begin();
  Serial.println("OTA ready. Hostname: clock742");
}

// =============================================================
//  setup()
// =============================================================
void setup() {
  pixels.begin();
  pixels.setBrightness(80);
  pixels.clear();
  pixels.show();
  delay(500);

  Serial.begin(115200);

  bool connected = connectWiFi();
  if (connected) {
    bool synced = syncNTP();
    if (synced) {
      setenv("TZ", TZ_EAST, 1);
      tzset();
      printLocalTime();
    } else {
      wifiError();
    }
    setupOTA();
  } else {
    wifiError();
  }

  randomSeed(analogRead(0));
}

// =============================================================
//  loop()
// =============================================================
void loop() {
  ArduinoOTA.handle();

  // Skip everything else during OTA upload
  if (otaInProgress) return;

  touch_inputs();

  // WiFi watchdog -- only reconnect if not uploading OTA
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi lost. Reconnecting...");
    connectWiFi();
  }

  if (!timeSynced) {
    if (millis() - lastRetry > RETRY_INTERVAL) {
      lastRetry = millis();
      bool synced = syncNTP();
      if (synced) {
        setenv("TZ", TZ_EAST, 1);
        tzset();
        pixels.clear();
        pixels.show();
      } else {
        wifiError();
      }
    }
    return;
  }

  if (millis() - lastSync > SYNC_INTERVAL) {
    syncNTP();
  }

  printLocalTime();

  if (minutes_day != minutes_day_now) {
    face2();
    face1();
    face0();
    minutes_day_now = minutes_day;
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

  digit_number = 0; digit_value = hour1;   digit_assign();
  digit_number = 1; digit_value = hour2;   digit_assign();
  dots_assign();
  digit_number = 2; digit_value = minute1; digit_assign();
  digit_number = 3; digit_value = minute2; digit_assign();
}

// =============================================================
//  Clock faces
// =============================================================
void face0() { renderFace(0, TZ_CHINA); matrix_assign_2(); }
void face1() { renderFace(1, TZ_EAST);  matrix_assign();   }
void face2() { renderFace(2, TZ_WEST);  matrix_assign_2(); }

// =============================================================
//  printLocalTime()
// =============================================================
void printLocalTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }
  hour_total   = timeinfo.tm_hour;
  minute_total = timeinfo.tm_min;
  hour2   = timeinfo.tm_hour % 10;
  hour1   = timeinfo.tm_hour / 10;
  minute2 = timeinfo.tm_min  % 10;
  minute1 = timeinfo.tm_min  / 10;
  minutes_day = timeinfo.tm_hour * 60 + timeinfo.tm_min;
  minutes_map = map(minutes_day, 0, 1440, 0, 255);
}
