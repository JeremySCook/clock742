// ============================================================
//  secrets.h  --  WiFi credentials + OTA password
//  !! Add this file to your .gitignore -- never commit it !!
//
//  .gitignore entry:
//      secrets.h
// ============================================================

#ifndef SECRETS_H
#define SECRETS_H

#define WIFI_SSID "YOUR_SSID_HERE"
#define WIFI_PASS "YOUR_PASSWORD_HERE"

// OTA update password -- required to push firmware wirelessly.
// Set this to something strong. You'll enter it in Arduino IDE
// the first time you do an OTA upload (it remembers it after).
#define OTA_PASS  "YOUR_OTA_PASSWORD_HERE"

#endif // SECRETS_H
