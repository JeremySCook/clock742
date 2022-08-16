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

#define NUMPIXELS 85 // Number of LED controlled ==> 58 for JSC + 27 in base display
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
int cursor = 27;

int digit_value = 0; // digit value to be assigned
int digit_number = 0; // digit number to be assigned
int hour1;
int hour2;
int minute1;
int minute2;
int minute2_now = 0;
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

//end neopixels section
int digit_color[3][3] = {{0,0,50}, //blue - morning
                        {50, 0, 50}, //purpleish - ~noon
                       {50, 0, 0}}; //red -afternoon/night
int color_red = 50;
int color_blue = 50;
int color_green = 50;
int hour_total;

// end colorshift section

#include <WiFi.h>
#include "time.h"

const char* ssid     = "MySpectrumWiFia8-2G";
const char* password = "livelywhale990";

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = -14400;
const int   daylightOffset_sec = 0;

//touch
int touch_sensor_value3 = 0;

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
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();

  //disconnect WiFi as it's no longer needed
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);

//neopixel setup

  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  pixels.clear(); // Set all pixel colors to 'off'

// will need a time to array
// will need routine to get time intermittently via WiFi
// dots @ 55, 56 - digits start @ 27, 41, 57, 71
//end neopixel setup

randomSeed(analogRead(0));

}

void loop(){
//delay(100);
touch_sensor_value3 = touchRead(T3); //PIN3
printLocalTime();

pixels.clear();
colors_assign();
//dots_assign();
digit_number = 0;
digit_value = hour1;
if (digit_value == 0) digit_value = 12;
digit_assign();
digit_number = 1;
digit_value = hour2;
digit_assign();
digit_number = 2;
digit_value = minute1;
digit_assign();
digit_number = 3;
digit_value = minute2;
digit_assign();
matrix_assign();
pixels.show();
}



void printLocalTime(){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  Serial.print("Day of week: ");
  Serial.println(&timeinfo, "%A");
  Serial.print("Month: ");
  Serial.println(&timeinfo, "%B");
  Serial.print("Day of Month: ");
  Serial.println(&timeinfo, "%d");
  Serial.print("Year: ");
  Serial.println(&timeinfo, "%Y");
  Serial.print("Hour: ");
  Serial.println(&timeinfo, "%H");
  Serial.print("Hour (12 hour format): ");
  Serial.println(&timeinfo, "%I");
  Serial.print("Minute: ");
  Serial.println(&timeinfo, "%M");
  Serial.print("Second: ");
  Serial.println(&timeinfo, "%S");
  Serial.print("Hour 2: ");
  hour2 = timeinfo.tm_hour %10;
  Serial.println(hour2);
  Serial.print("Hour 1: ");
  hour1 = timeinfo.tm_hour /10;
  Serial.println(hour1);
  Serial.print("Minute 2: "); 
  minute2 = timeinfo.tm_min %10;
  Serial.println(minute2);
  Serial.print("Minute 1: ");
  minute1 = timeinfo.tm_min /10;
  Serial.println(minute1);
  hour_total = timeinfo.tm_hour;
}

/*void dots_assign(){
  //dots
  //leds[55] = CRGB::Green;
  //leds[56] = CRGB::Green;
    pixels.setPixelColor(55, pixels.Color(color_red, color_green, color_blue));
    pixels.setPixelColor(56, pixels.Color(color_red, color_green, color_blue)); 
  //end dots
}*/
  
void digit_assign(){
  //pixels.clear(); // Set all pixel colors to 'off'
  if (digit_number==0){
  cursor = 27;
  for(int k=0; k<=13;k++){ 
  // Serial.print(digits[digit][k]);
  if (digits[digit_value][k]== 1){pixels.setPixelColor(cursor, pixels.Color(color_red, color_green, color_blue));}
  else if (digits[digit_value][k]==0){pixels.setPixelColor(cursor, pixels.Color(0, 0, 0));};
  cursor ++;
  }
  }
  if (digit_number==1){
  cursor = 41;
  for(int k=0; k<=13;k++){ 
  // Serial.print(digits[digit][k]);
  if (digits[digit_value][k]== 1){pixels.setPixelColor(cursor, pixels.Color(color_red, color_green, color_blue));}
  else if (digits[digit_value][k]==0){pixels.setPixelColor(cursor, pixels.Color(0, 0, 0));};
  cursor ++;
  }
  }
  if (digit_number==2){
  cursor = 57;
  for(int k=0; k<=13;k++){ 
  // Serial.print(digits[digit][k]);
  if (digits[digit_value][k]== 1){pixels.setPixelColor(cursor, pixels.Color(color_red, color_green, color_blue));}
  else if (digits[digit_value][k]==0){pixels.setPixelColor(cursor, pixels.Color(0, 0, 0));};
  cursor ++;
  }
  }
  if (digit_number==3){
  cursor = 71;
  for(int k=0; k<=13;k++){ 
  // Serial.print(digits[digit][k]);
  if (digits[digit_value][k]== 1){pixels.setPixelColor(cursor, pixels.Color(color_red, color_green, color_blue));}
  else if (digits[digit_value][k]==0){pixels.setPixelColor(cursor, pixels.Color(0, 0, 0));};
  cursor ++;
  }
  }
}

void matrix_assign(){
    
//colon       
//if (minute2 != minute2_now) {//changes matrix only when minute advances
  //pixels.clear();
  cursor = 0;
  for(int k=0; k<=26;k++){ 
  // Serial.print(digits[digit][k]);
  int randNumber = random(20);
  int randNumber1 = random(27);
  int randNumber2 = random(54);
  pixels.setPixelColor(cursor, pixels.Color((color_red*randNumber/10), (color_green*randNumber/10), (color_blue*randNumber/10)));
  if (randNumber1 == k){ //random pixels (1/27)
      pixels.setPixelColor(cursor, pixels.Color((color_blue*randNumber/10), (color_red*randNumber/10), (color_green*randNumber/10)));
  }
  if (randNumber2 == k){ //random pixels (1/54)
      pixels.setPixelColor(cursor, pixels.Color((color_green*randNumber/10), (color_blue*randNumber/10), (color_red*randNumber/10)));
  }
    cursor ++;
  }
//colon
    pixels.setPixelColor(55, pixels.Color(color_red, color_green, color_blue));
    pixels.setPixelColor(56, pixels.Color(color_red, color_green, color_blue)); 
    pixels.show();
    delay(500);   
    pixels.setPixelColor(55, pixels.Color(0, 0, 0));
    pixels.setPixelColor(56, pixels.Color(0, 0, 0)); 
    pixels.show();    
  delay(500); //need to change this to pick up on seconds later and coordinate with colon
//  minute2_now = minute2;
}
//}


void colors_assign(){
  Serial.print("touch_sensor_value3 = ");
  Serial.println(touch_sensor_value3);

//color_red = map(hour_total, 0, 23, 0, 50);
//if (hour_total < 12) color_green = map((hour_total -11), 0, 12, 0, 30);
//if (hour_total >= 12) color_green = map((23 - hour_total), 0, 12, 0, 30);
//color_blue = map(abs(24 - hour_total), 0, 23, 0, 50);

  if (hour_total < 12){
    color_red = digit_color[0][0];
    color_green = digit_color[0][1];
    color_blue = digit_color[0][2];
  }
  if (hour_total == 12){
    color_red = digit_color[1][0];
    color_green = digit_color[1][1];
    color_blue = digit_color[1][2];
  }
    if (hour_total > 12){
    color_red = digit_color[2][0];
    color_green = digit_color[2][1];
    color_blue = digit_color[2][2];
  }

    if (touch_sensor_value3 > 18000){
    color_red = 75;
    color_green = 75;
    color_blue = 25;
    }
}
