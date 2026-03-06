#if 0
void colors_assign(){
  Serial.print("touch_sensor_value3 = ");
  Serial.println(touch_sensor_value3);

//color_red = map(hour_total, 0, 23, 0, 50);
//if (hour_total < 12) color_green = map((hour_total -11), 0, 12, 0, 30);
//if (hour_total >= 12) color_green = map((23 - hour_total), 0, 12, 0, 30);
//color_blue = map(abs(24 - hour_total), 0, 23, 0, 50);

  if (hour_total < 12){
    color_red = digit_color[0][0]*color_scale;
    color_green = digit_color[0][1]*color_scale;
    color_blue = digit_color[0][2]*color_scale;
  }
  if (hour_total == 12){
    color_red = digit_color[1][0]*color_scale;
    color_green = digit_color[1][1]*color_scale;
    color_blue = digit_color[1][2]*color_scale;
  }
    if (hour_total > 12){
    color_red = digit_color[2][0]*color_scale;
    color_green = digit_color[2][1]*color_scale;
    color_blue = digit_color[2][2]*color_scale;
  }

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
#endif
