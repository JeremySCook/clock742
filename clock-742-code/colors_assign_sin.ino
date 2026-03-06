// colors_assign_sin.ino

#define TOUCH_THRESHOLD 10000

float lerpf(float a, float b, float t) {
  return a + (b - a) * t;
}

float clampf(float t) {
  if (t < 0.0) return 0.0;
  if (t > 1.0) return 1.0;
  return t;
}

float windowProgress(int mins, int startMins, int endMins) {
  return clampf((float)(mins - startMins) / (float)(endMins - startMins));
}

void colors_assign() {
  int mins = hour_total * 60 + minute_total;

  float r, g, b;

  // Waypoints
  float nr=30,  ng=0,   nb=60;   // night: violet
  float dr=100, dg=140, db=220;  // dawn: blue-white
  float mr=220, mg=230, mb=255;  // morning: bright white
  float ar=255, ag=160, ab=60;   // afternoon: amber
  float er=220, eg=30,  eb=0;    // evening: red
  float lr=50,  lg=0,   lb=80;   // late night: violet

  if      (mins < 300)  { r=nr; g=ng; b=nb; }
  else if (mins < 480)  { float t=windowProgress(mins,300,480);  r=lerpf(nr,dr,t); g=lerpf(ng,dg,t); b=lerpf(nb,db,t); }
  else if (mins < 720)  { float t=windowProgress(mins,480,720);  r=lerpf(dr,mr,t); g=lerpf(dg,mg,t); b=lerpf(db,mb,t); }
  else if (mins < 1020) { float t=windowProgress(mins,720,1020); r=lerpf(mr,ar,t); g=lerpf(mg,ag,t); b=lerpf(mb,ab,t); }
  else if (mins < 1200) { float t=windowProgress(mins,1020,1200);r=lerpf(ar,er,t); g=lerpf(ag,eg,t); b=lerpf(ab,eb,t); }
  else                  { float t=windowProgress(mins,1200,1440);r=lerpf(er,lr,t); g=lerpf(eg,lg,t); b=lerpf(eb,lb,t); }

  float scale = color_scale / 5.0;
  color_red   = (int)(r * scale);
  color_green = (int)(g * scale);
  color_blue  = (int)(b * scale);

  Serial.print("CIRC mins="); Serial.print(mins);
  Serial.print(" pretouch RGB=("); Serial.print(color_red);
  Serial.print(","); Serial.print(color_green);
  Serial.print(","); Serial.print(color_blue); Serial.print(")");
  Serial.print(" T3="); Serial.print(touch_sensor_value3);
  Serial.print(" T5="); Serial.print(touch_sensor_value5);
  Serial.print(" T7="); Serial.print(touch_sensor_value7);
  Serial.print(" T9="); Serial.println(touch_sensor_value9);

  // ESP32 touch reads HIGH when idle, LOW when touched
  if (touch_sensor_value3 < TOUCH_THRESHOLD) {
    color_red=200; color_green=0; color_blue=0;
    Serial.println("TOUCH OVERRIDE T3 fired!");
  }
  if (touch_sensor_value9 < TOUCH_THRESHOLD) {
    color_red=0; color_green=200; color_blue=0;
    Serial.println("TOUCH OVERRIDE T9 fired!");
  }
  if ((touch_sensor_value5 < TOUCH_THRESHOLD) && (color_scale < 5)) {
    color_scale++;
    Serial.println("TOUCH OVERRIDE T5 fired!");
  }
  if ((touch_sensor_value7 < TOUCH_THRESHOLD) && (color_scale > 1)) {
    color_scale--;
    Serial.println("TOUCH OVERRIDE T7 fired!");
  }
}
