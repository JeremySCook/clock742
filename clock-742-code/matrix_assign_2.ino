void matrix_assign_2(){
    
//colon       
//if (minute2 != minute2_now) {//changes matrix only when minute advances
  //pixels.clear();
  cursor = 0 + faceID*85;
  for(int k=0; k<=26;k++){ 

  int randNumber = random(2);
  int randNumber1 = random(27);
  int randNumber2 = random(54);
  
  if (randNumber == 1){ //random pixels (1/2)
      pixels.setPixelColor(cursor, 0);  
  }
  if (randNumber1 == k){ //random pixels (1/27)
      pixels.setPixelColor(cursor, Wheel(minutes_map-25));
  }
  if (randNumber2 == k){ //random pixels (1/54)
      pixels.setPixelColor(cursor, Wheel(minutes_map-75));
  }
    cursor ++;
  }
  pixels.show();
}
