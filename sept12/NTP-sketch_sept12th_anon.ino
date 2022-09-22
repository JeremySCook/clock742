/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-date-time-ntp-client-server-arduino/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

//neopixels section

#include <Adafruit_NeoPixel.h>

#define NUMPIXELS 255 // Number of LED controlled ==> 85 * number of faces
#define COLOR_ORDER GRB  // Define color order for your strip
#define PIN 16 // Data pin for led comunication ==> will need to adjust all of this for new clock

byte digits[13][14] = {{1,1,1,1,0,1,1,1,1,1,1,0,1,1},  // Digit 0
                       {0,0,0,0,0,0,0,0,0,1,1,0,1,1},   // Digit 1
                       {1,1,1,1,1,0,0,1,1,1,1,1,0,0},   // Digit 2
                       {1,1,0,0,1,0,0,1,1,1,1,1,1,1},   // Digit 3
                       {0,0,0,0,1,1,1,0,0,1,1,1,1,1},   // Digit 4
                       {1,1,0,0,1,1,1,1,1,0,0,1,1,1},   // Digit 5
                       {1,1,1,1,1,1,1,1,1,0,0,1,1,1},   // Digit 6
                       {0,0,0,0,0,0,0,1,1,1,1,0,1,1},   // Digit 7
                       {1,1,1,1,1,1,1,1,1,1,1,1,1,1},   // Digit 8
                       {1,1,0,0,1,1,1,1,1,1,1,1,1,1},   // Digit 9 | 2D Array for numbers on 7 segment
                       {1,1,1,1,0,1,1,1,1,1,1,0,1,1},   // Digit *0
                       {1,1,1,1,0,1,1,1,1,0,0,0,0,0},  // Digit C (11)
                       {0,0,0,0,0,0,0,0,0,0,0,0,0,0}}; // Digit blank (12)

bool Dot = true;  //Dot state
int last_digit = 0;
int cursor;
int cursor1;

int digit_value = 0; // digit value to be assigned
int digit_number = 0; // digit number to be assigned
int hour1;
int hour2;
int minute1;
int minute2;
int minute2_now = 0;
int faceID;

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

//end neopixels section
int digit_color[3][3] = {{0,0,10}, //blue - morning
                        {10, 0, 10}, //purpleish - ~noon
                       {10, 0, 0}}; //red -afternoon/night
int color_red = 50;
int color_blue = 50;
int color_green = 50;
int color_scale = 5; //scales brightness between 1-5 based on touch inputs
float color_intensity = 1; //scales brightness based on time of day

int hour_total;
int minute_total;
int minutes_day; //minutes into day
int minutes_day_now = 0; //minutes into day set
int minutes_map;

// end colorshift section

#include <WiFi.h>
#include "time.h"

const char* ssid     = "INPUT YOUR SSID HERE";
const char* password = "INPUT YOUR PASSWORD HERE";

const char* ntpServer = "pool.ntp.org";
long  gmtOffset_sec = -14400; //modify for each face
const int   daylightOffset_sec = 0;

//touch
int touch_sensor_value3 = 0;
int touch_sensor_value5 = 0;
int touch_sensor_value7 = 0;
int touch_sensor_value9 = 0;

void setup(){
  Serial.begin(115200);

  // Connect to Wi-Fi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  
  // Init and get the time
  configTime(-14400, daylightOffset_sec, ntpServer); //Does this log on each time and reset time??? Neet to find more efficient way
  printLocalTime();

  //disconnect WiFi as it's no longer needed
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);

//neopixel setup

  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  pixels.setBrightness(20);
  pixels.clear(); // Set all pixel colors to 'off'

// will need a time to array
// will need routine to get time intermittently via WiFi
// dots @ 55, 56 - digits start @ 27, 41, 57, 71
//end neopixel setup

randomSeed(analogRead(0));

}

void loop(){
//pixels.clear();
touch_inputs();
printLocalTime();
if (minutes_day != minutes_day_now){
face2();
face1();
face0();
minutes_day_now = minutes_day;
}
}

void face0(){
faceID = 0;
configTime(28800, daylightOffset_sec, ntpServer); // +8 hour offset - China

printLocalTime();
colors_assign();
digit_number = 0;
digit_value = hour1;
if (digit_value == 0) digit_value = 12;
digit_assign();
digit_number = 1;
digit_value = hour2;
digit_assign();
dots_assign();
digit_number = 2;
digit_value = minute1;
digit_assign();
digit_number = 3;
digit_value = minute2;
digit_assign();
matrix_assign_2();
}

void face1(){
faceID = 1;
configTime(gmtOffset_sec, daylightOffset_sec, ntpServer); //home face - east coast USA

printLocalTime();
colors_assign();
digit_number = 0;
digit_value = hour1;
if (digit_value == 0) digit_value = 12;
digit_assign();
digit_number = 1;
digit_value = hour2;
digit_assign();
dots_assign();
digit_number = 2;
digit_value = minute1;
digit_assign();
digit_number = 3;
digit_value = minute2;
digit_assign();
matrix_assign();
}

void face2(){
faceID = 2;
configTime(-25200, daylightOffset_sec, ntpServer); // -7 hour offset - West Coast USA

printLocalTime();
colors_assign();
dots_assign();
digit_number = 0;
digit_value = hour1;
if (digit_value == 0) digit_value = 12;
digit_assign();
digit_number = 1;
digit_value = hour2;
digit_assign();
dots_assign();
digit_number = 2;
digit_value = minute1;
digit_assign();
digit_number = 3;
digit_value = minute2;
digit_assign();
matrix_assign_2();
}

void printLocalTime(){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  hour2 = timeinfo.tm_hour %10;
  hour1 = timeinfo.tm_hour /10;
  minute2 = timeinfo.tm_min %10;
  minute1 = timeinfo.tm_min /10;
  hour_total = timeinfo.tm_hour;
  minute_total = timeinfo.tm_min;
  minutes_day = timeinfo.tm_hour*60 + timeinfo.tm_min;
  minutes_map = map(minutes_day, 0, 1440, 0, 255);
}

/*void dots_assign(){
  //dots
  //leds[55] = CRGB::Green;
  //leds[56] = CRGB::Green;
    pixels.setPixelColor(55, pixels.Color(color_red, color_green, color_blue));
    pixels.setPixelColor(56, pixels.Color(color_red, color_green, color_blue)); 
  //end dots
}*/
