# clock742 — sept12 (Fixed)

ESP32 + WS2812B LED clock with three simultaneous clock faces.

## Files

All files must be in a folder named `NTP-sketch_sept12th_anon` for the Arduino IDE to compile them together.

| File | Purpose |
|---|---|
| `NTP-sketch_sept12th_anon.ino` | Main sketch — setup, loop, face rendering |
| `secrets.h` | **Your WiFi credentials** — see setup below |
| `dots_assign.ino` | Separator dot LEDs (AM=blue / PM=amber) |
| `digit_assign.ino` | 7-segment digit rendering |
| `matrix_assign.ino` | Background matrix for face1 |
| `matrix_assign_2.ino` | Background matrix for face0 & face2 |
| `colors_assign_sin.ino` | Sinusoidal color cycling |
| `colors_assign_wheel.ino` | Rainbow wheel helper |
| `touch_inputs.ino` | Capacitive touch brightness control |

## Setup

### 1. WiFi credentials
Edit `secrets.h` and fill in your network details:
```cpp
#define WIFI_SSID "your_network_name"
#define WIFI_PASS "your_password"
```
**Never commit `secrets.h` to git.** A `.gitignore` entry is included.

### 2. Timezones
The three faces are pre-configured for:
- **face0** — China Standard Time (UTC+8, no DST)
- **face1** — US Eastern (EST/EDT, auto DST)
- **face2** — US Pacific (PST/PDT, auto DST)

To change a timezone, edit the POSIX strings near the top of the main `.ino`:
```cpp
const char* TZ_EAST  = "EST5EDT,M3.2.0,M11.1.0";
const char* TZ_WEST  = "PST8PDT,M3.2.0,M11.1.0";
const char* TZ_CHINA = "CST-8";
```
A full list of POSIX timezone strings: https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv

## Fixes Applied (vs. sept12 original)

- **DST now works automatically** — POSIX timezone strings replace hardcoded UTC offsets
- **NTP called once at startup** — no more per-face NTP hammering; timezone switching is local
- **Periodic NTP re-sync** every 6 hours prevents RTC clock drift
- **12-hour display corrected** — hours 13–23 now show as 1–11 PM (not 13–23)
- **AM/PM dot indicator** — separator dots glow blue (AM) or amber (PM)
- **WiFi credentials** moved to `secrets.h` / `.gitignore`
- **Syntax error fixed** in `matrix_assign.ino` (dangling `pixels.` statement)
- **Code deduplicated** — face0/1/2 share a `renderFace()` helper
