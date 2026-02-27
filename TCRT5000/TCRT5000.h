#pragma once
#include <Arduino.h>

//grey = VCC, purple = GND, blue = D0, green = A0
class TCRT5000 {
  uint8_t analog_, digitalPin;
  int deadband_, levels_;

public:
  TCRT5000(uint8_t analog, uint8_t digital, int deadband = 1) 
  : analog_(analog), digitalPin(digital), deadband_(deadband), levels_(1024/deadband_) {
  }

  void begin() const {
    pinMode(digitalPin, INPUT);
  }

  //Note: don't call bright() or dark() if you're not using a digital pin, it's undefined behaviour
  bool bright() const{
    return digitalRead(digitalPin) == LOW;//more reflective gives lower analogRead(), counterintuitively
  }

  bool dark() const{
    return digitalRead(digitalPin) == HIGH;
  }

  //defines the number of possible levels of analogRead() can occupy, reads the sensor and returns the value. 
  //NOTE: 0 is brightest, higher values are darker surfaces
  unsigned int band() const{
    int value = analogRead(analog_);
    for(int i = 0; i < levels_; ++i){
      int level = deadband_ * (i+1);
      if(value < level) return i;
    }

    return 0;
  }

  //sets deadband, probably should set this in the constructor though.
  void deadband(int deadband){
    deadband_ = deadband;
    levels_ = 1023/deadband_;
  }

};