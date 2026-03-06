void touch_inputs(){
  touch_sensor_value3 = touchRead(T3); //PIN3 - UPPER-LEFT
  touch_sensor_value5 = touchRead(T5); //UPPER-RIGHT
  touch_sensor_value7 = touchRead(T7); //LOWER-RIGHT
  touch_sensor_value9 = touchRead(T9); //LOWER-LEFT

/*  if(touch_sensor_value3 = HIGH){
    faceID == 1;
    matrix_assign_2();
  }
*/  
}
