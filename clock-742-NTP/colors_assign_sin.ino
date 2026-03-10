// colors_assign_sin.ino
//
// face1 (home, Eastern): circadian arc shifts with actual sunrise/sunset
//   from solar.ino -- waypoints are dawn/sunrise/sunset/dusk each day.
//
// face0, face2 (China, West Coast): fixed time-based arc, unchanged.

// Forward declarations for globals defined in solar.ino

// Matrix arc color globals -- set by colors_assign, used by face0/1/2 for matrix
int matrix_arc_r = 0;
int matrix_arc_g = 0;
int matrix_arc_b = 0;
extern float sunriseMins;
extern float sunsetMins;
extern float dawnMins;
extern float duskMins;

float lerpf(float a, float b, float t) { return a + (b - a) * t; }
float clampf(float t) { return t < 0.0 ? 0.0 : t > 1.0 ? 1.0 : t; }
float windowProgress(int mins, int s, int e) {
  return clampf((float)(mins - s) / (float)(e - s));
}

// Fixed arc for non-home faces (face0, face2)
void colors_assign_fixed(int mins) {
  float r, g, b;
  float nr=120,ng=20, nb=200;
  float dr=60, dg=160,db=180;
  float mr=100,mg=180,mb=120;
  float ar=220,ag=140,ab=40;
  float er=240,eg=100,eb=10;
  float lr=160,lg=20, lb=180;

  if      (mins < 300)  { r=nr;g=ng;b=nb; }
  else if (mins < 480)  { float t=windowProgress(mins,300,480);  r=lerpf(nr,dr,t);g=lerpf(ng,dg,t);b=lerpf(nb,db,t); }
  else if (mins < 720)  { float t=windowProgress(mins,480,720);  r=lerpf(dr,mr,t);g=lerpf(dg,mg,t);b=lerpf(db,mb,t); }
  else if (mins < 1020) { float t=windowProgress(mins,720,1020); r=lerpf(mr,ar,t);g=lerpf(mg,ag,t);b=lerpf(mb,ab,t); }
  else if (mins < 1200) { float t=windowProgress(mins,1020,1200);r=lerpf(ar,er,t);g=lerpf(ag,eg,t);b=lerpf(ab,eb,t); }
  else                  { float t=windowProgress(mins,1200,1440);r=lerpf(er,lr,t);g=lerpf(eg,lg,t);b=lerpf(eb,lb,t); }

  float scale   = color_scale / 5.0;
  color_red   = (int)(r * scale);
  color_green = (int)(g * scale);
  color_blue  = (int)(b * scale);
}

// Solar arc for home face (face1)
void colors_assign_solar(int mins) {
  float r, g, b;
  int   noonMins = (int)((sunriseMins + sunsetMins) / 2.0);

  float nr=120,ng=20, nb=200;
  float dn=40, dg=80, db=200;
  float sr=100,sg=160,sb=220;
  float mn=200,mg=210,mb=220;
  float st=240,sg2=150,sb2=60;
  float dk=200,dkg=30,dkb=10;
  float lr=120,lg=20, lb=200;

  if (mins < (int)dawnMins) {
    r=nr;g=ng;b=nb;
  } else if (mins < (int)sunriseMins) {
    float t=windowProgress(mins,(int)dawnMins,(int)sunriseMins);
    r=lerpf(nr,sr,t);g=lerpf(ng,sg,t);b=lerpf(nb,sb,t);
  } else if (mins < noonMins) {
    float t=windowProgress(mins,(int)sunriseMins,noonMins);
    r=lerpf(sr,mn,t);g=lerpf(sg,mg,t);b=lerpf(sb,mb,t);
  } else if (mins < (int)sunsetMins) {
    float t=windowProgress(mins,noonMins,(int)sunsetMins);
    r=lerpf(mn,st,t);g=lerpf(mg,sg2,t);b=lerpf(mb,sb2,t);
  } else if (mins < (int)duskMins) {
    float t=windowProgress(mins,(int)sunsetMins,(int)duskMins);
    r=lerpf(st,dk,t);g=lerpf(sg2,dkg,t);b=lerpf(sb2,dkb,t);
  } else {
    float t=windowProgress(mins,(int)duskMins,1440);
    r=lerpf(dk,lr,t);g=lerpf(dkg,lg,t);b=lerpf(dkb,lb,t);
  }

  float scale   = color_scale / 5.0;
  color_red   = (int)(r * scale);
  color_green = (int)(g * scale);
  color_blue  = (int)(b * scale);
}

void colors_assign() {
  int mins = hour_total * 60 + minute_total;

  // Run arc to calculate circadian colors
  if (faceID == 1) {
    colors_assign_solar(mins);
  } else {
    colors_assign_fixed(mins);
  }

  // Snapshot arc colors for matrix (restored in renderFace after digit animation)
  matrix_arc_r = color_red;
  matrix_arc_g = color_green;
  matrix_arc_b = color_blue;

  // Digits: fixed AM/PM color matching colon, scaled by color_scale
  float scale = color_scale / 5.0;
  if (is_am) {
    color_red   = (int)(60  * scale);
    color_green = (int)(160 * scale);
    color_blue  = (int)(255 * scale);
  } else {
    color_red   = (int)(220 * scale);
    color_green = 0;
    color_blue  = 0;
  }

  // Touch color overrides
  if (touch_sensor_value3 < TOUCH_THRESHOLD) {
    color_red=200; color_green=0; color_blue=0;
  }
  if (touch_sensor_value9 < TOUCH_THRESHOLD) {
    color_red=0; color_green=200; color_blue=0;
  }
}
