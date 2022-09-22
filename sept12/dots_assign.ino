void dots_assign(){
    pixels.setPixelColor(55 + faceID*85, pixels.Color(0, 0, 0));
    pixels.setPixelColor(56 + faceID*85, pixels.Color(0, 0, 0));
    pixels.show();
    delay(20);
    pixels.setPixelColor((55 + faceID*85), Wheel(minutes_map));
    pixels.setPixelColor((56 + faceID*85), Wheel(minutes_map));
    pixels.show();
    //delay(50);
}
