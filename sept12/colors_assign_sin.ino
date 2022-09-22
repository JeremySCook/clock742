
void colors_assign(){

float color_intensity_sin = 2 + sin(((((hour_total*60+minute_total)*360)/1440)-90)*3.14/180);
color_intensity = color_intensity_sin;
float color_red_sin = 1 + sin(((((hour_total*60+minute_total)*360)/1440)-270)*3.14/180);
color_red = color_red_sin*color_scale*color_intensity*4;
float color_blue_sin = 1 + sin(((((hour_total*60+minute_total)*360)/1440)-90)*3.14/180);
color_blue = color_blue_sin*color_scale*color_intensity*4;
float color_green_sin = 1 + sin(((((hour_total*60+minute_total)*360)/1440)+225)*3.14/180);
color_green = color_green_sin*color_scale*color_intensity*0;

  Serial.print("face = "); Serial.println(faceID);
  Serial.print("minutes_map ");
  Serial.println(minutes_map);
    
    if (touch_sensor_value3 > 30000){
    color_red = 75;
    color_green = 0;
    color_blue = 0;
    }
    if (touch_sensor_value9 > 30000){
    color_red = 0;
    color_green = 75;
    color_blue = 0;
    }
    if ((touch_sensor_value5 > 30000) && (color_scale < 5)){ //turn up color
    color_scale++;
    }
    if ((touch_sensor_value7 > 30000) && (color_scale > 1)){ //turn down color
    color_scale--;  
    }
}
