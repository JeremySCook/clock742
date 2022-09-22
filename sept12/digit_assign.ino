void digit_assign(){
int delay_between_numbers = 50;
  
  if (digit_number==0){

  for(int scroll_value = 0; scroll_value<=digit_value; scroll_value++){ 
  
  cursor = 27 + faceID*85;
  cursor1 = cursor;
  
  for(int k=0; k<=13;k++){ //clear digit
  pixels.setPixelColor(cursor1, pixels.Color(0, 0, 0));
  cursor1 ++;
  }
  pixels.show();

  if (hour1 != 0){ //keeps from scrolling on blank preceeding digit
  delay(delay_between_numbers);
  for(int k=0; k<=13;k++){
  //Serial.print("cursor "); Serial.println(cursor); 
  // Serial.print(digits[digit][k]);
  if (digits[scroll_value][k]== 1){pixels.setPixelColor(cursor, Wheel(minutes_map));}
  else if (digits[scroll_value][k]==0){pixels.setPixelColor(cursor, pixels.Color(0, 0, 0));};
  cursor ++;
  }
  pixels.show();
  //delay(5);
  }
  }
  }

  
  if (digit_number==1){

  for(int scroll_value = 0; scroll_value<=digit_value; scroll_value++){ 
    
  cursor = 41 + faceID*85;
  cursor1 = cursor;
  
  for(int k=0; k<=13;k++){ //clear digit
  pixels.setPixelColor(cursor1, pixels.Color(0, 0, 0));
  cursor1 ++;
  }
  pixels.show();
  delay(delay_between_numbers);
  for(int k=0; k<=13;k++){ 
  // Serial.print(digits[digit][k]);
  if (digits[scroll_value][k]== 1){pixels.setPixelColor(cursor, Wheel(minutes_map));}
  else if (digits[scroll_value][k]==0){pixels.setPixelColor(cursor, pixels.Color(0, 0, 0));};
  cursor ++;
  }
  pixels.show();
  //delay(50);
  }
  }
  
  if (digit_number==2){

  for(int scroll_value = 0; scroll_value<=digit_value; scroll_value++){ 
    
  cursor = 57 + faceID*85;
  cursor1 = cursor;

  for(int k=0; k<=13;k++){ //clear digit
  pixels.setPixelColor(cursor1, pixels.Color(0, 0, 0));
  cursor1 ++;
  }
  pixels.show();
  delay(delay_between_numbers);
  for(int k=0; k<=13;k++){ 
  // Serial.print(digits[digit][k]);
  if (digits[scroll_value][k]== 1){pixels.setPixelColor(cursor, Wheel(minutes_map));}
  else if (digits[scroll_value][k]==0){pixels.setPixelColor(cursor, pixels.Color(0, 0, 0));};
  cursor ++;
  }
  pixels.show();
  //delay(50);
  }
  }
  
  if (digit_number==3){
    
  for(int scroll_value = 0; scroll_value<=digit_value; scroll_value++){ 
    
  cursor = 71 + faceID*85;
  cursor1 = cursor;
  
  for(int k=0; k<=13;k++){ //clear digit
  pixels.setPixelColor(cursor1, pixels.Color(0, 0, 0));
  cursor1 ++;
  }
  pixels.show();
  delay(delay_between_numbers);
  for(int k=0; k<=13;k++){ 
  // Serial.print(digits[digit][k]);
  if (digits[scroll_value][k]== 1){pixels.setPixelColor(cursor, Wheel(minutes_map));}
  else if (digits[scroll_value][k]==0){pixels.setPixelColor(cursor, pixels.Color(0, 0, 0));};
  cursor ++;
  }
  pixels.show();
  //delay(50);
  }
  }
}
