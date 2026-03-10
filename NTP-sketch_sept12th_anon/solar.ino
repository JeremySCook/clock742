// solar.ino
//
// Calculates sunrise, sunset, civil dawn and civil dusk
// for a given latitude, longitude, and day of year.
// Uses the NOAA solar calculation algorithm (accurate to ~1 min).
//
// All times returned as fractional hours in LOCAL time (0.0-24.0).
// e.g. 6.5 = 6:30 AM,  19.75 = 7:45 PM
//
// Results stored in globals updated once per day in loop().

float sunriseMins  = 360;   // default 6:00 AM until calculated
float sunsetMins   = 1140;  // default 7:00 PM until calculated
float dawnMins     = 330;   // civil dawn ~30 min before sunrise
float duskMins     = 1170;  // civil dusk ~30 min after sunset

// Convert degrees to radians
float deg2rad(float d) { return d * PI / 180.0; }
float rad2deg(float r) { return r * 180.0 / PI; }

// Calculate solar noon, sunrise, sunset for given day
// lat/lon in decimal degrees, dayOfYear 1-365
// timezone offset in hours (e.g. -5 for EST, -4 for EDT)
void calcSunriseSunset(float lat, float lon, int dayOfYear, float tzOffset) {
  // Fractional year (radians)
  float gamma = 2.0 * PI / 365.0 * (dayOfYear - 1 + (12.0 - 12) / 24.0);

  // Equation of time (minutes)
  float eqTime = 229.18 * (0.000075
    + 0.001868 * cos(gamma)
    - 0.032077 * sin(gamma)
    - 0.014615 * cos(2*gamma)
    - 0.04089  * sin(2*gamma));

  // Solar declination (radians)
  float decl = 0.006918
    - 0.399912 * cos(gamma)
    + 0.070257 * sin(gamma)
    - 0.006758 * cos(2*gamma)
    + 0.000907 * sin(2*gamma)
    - 0.002697 * cos(3*gamma)
    + 0.00148  * sin(3*gamma);

  // Hour angle for sunrise/sunset (degrees)
  // zenith = 90.833 for standard sunrise/sunset
  float latRad  = deg2rad(lat);
  float cosHA   = (cos(deg2rad(90.833)) / (cos(latRad) * cos(decl)))
                  - tan(latRad) * tan(decl);

  // Clamp to valid range (handles polar day/night)
  cosHA = constrain(cosHA, -1.0, 1.0);
  float ha = rad2deg(acos(cosHA));  // degrees

  // Solar noon in minutes from midnight UTC
  float solarNoonUTC = 720.0 - 4.0 * lon - eqTime;

  // Sunrise / sunset in minutes from midnight UTC
  float sunriseUTC = solarNoonUTC - ha * 4.0;
  float sunsetUTC  = solarNoonUTC + ha * 4.0;

  // Civil twilight: zenith = 96 degrees
  float cosHA_civil = (cos(deg2rad(96.0)) / (cos(latRad) * cos(decl)))
                      - tan(latRad) * tan(decl);
  cosHA_civil = constrain(cosHA_civil, -1.0, 1.0);
  float ha_civil    = rad2deg(acos(cosHA_civil));
  float dawnUTC     = solarNoonUTC - ha_civil * 4.0;
  float duskUTC     = solarNoonUTC + ha_civil * 4.0;

  // Convert UTC minutes to local minutes
  float tzMins  = tzOffset * 60.0;
  sunriseMins   = sunriseUTC + tzMins;
  sunsetMins    = sunsetUTC  + tzMins;
  dawnMins      = dawnUTC    + tzMins;
  duskMins      = duskUTC    + tzMins;

  Serial.print("Solar: dawn=");   Serial.print(dawnMins/60.0);
  Serial.print(" sunrise=");      Serial.print(sunriseMins/60.0);
  Serial.print(" sunset=");       Serial.print(sunsetMins/60.0);
  Serial.print(" dusk=");         Serial.println(duskMins/60.0);
}

// Call once per day (or on boot) to update solar times.
// Reads date from ESP32 RTC, determines EST vs EDT offset automatically.
void updateSolarTimes() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) return;

  int dayOfYear = timeinfo.tm_yday + 1;  // 1-365

  // Determine if DST is active for timezone offset
  // tm_isdst > 0 means DST is in effect
  float tzOffset = (timeinfo.tm_isdst > 0) ? -4.0 : -5.0;  // EDT or EST

  calcSunriseSunset(HOME_LAT, HOME_LON, dayOfYear, tzOffset);
}

// Returns global brightness (0-255) based on solar position of home timezone.
// Full brightness during day, dims through twilight, minimum at night.
int solarBrightness() {
  // Get current home time in minutes, preserving the current TZ
  struct tm timeinfo;
  char prevTZ[64] = "";
  const char* tz = getenv("TZ");
  if (tz) strncpy(prevTZ, tz, sizeof(prevTZ) - 1);

  setenv("TZ", "EST5EDT,M3.2.0,M11.1.0", 1);
  tzset();
  if (!getLocalTime(&timeinfo)) {
    // Restore TZ before returning
    setenv("TZ", prevTZ, 1); tzset();
    return BRIGHTNESS_DAY;  // safe fallback
  }

  int nowMins = timeinfo.tm_hour * 60 + timeinfo.tm_min;

  // Restore previous TZ immediately after reading time
  setenv("TZ", prevTZ, 1);
  tzset();

  int bMin = BRIGHTNESS_NIGHT;
  int bMax = BRIGHTNESS_DAY;

  if (nowMins < dawnMins) {
    return bMin;
  } else if (nowMins < sunriseMins) {
    float t = (float)(nowMins - dawnMins) / (float)(sunriseMins - dawnMins);
    return (int)(bMin + t * (bMax - bMin));
  } else if (nowMins < sunsetMins) {
    return bMax;
  } else if (nowMins < duskMins) {
    float t = (float)(nowMins - sunsetMins) / (float)(duskMins - sunsetMins);
    return (int)(bMax - t * (bMax - bMin));
  } else {
    return bMin;
  }
}
