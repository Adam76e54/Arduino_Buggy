#pragma once

#include <Arduino.h>

class ROB12629{
  private:
    uint8_t pin_;//one digital pin
    volatile unsigned long counter_;
    volatile unsigned long lastCounter_;
    volatile unsigned long lastTime_;
    volatile double double rpm_;
    static constexpr float COUNTS_PER_REV_ = 4.0f;
  public:
    ROB12629(uint8_t pin) : pin_(pin), counter_(0), lastCounter_(0), lastTime_(0), rpm_(0){
      //empty
    }

    //requires us to feed an ISR to it
    void begin(void (*ISR)()){
      pinMode(pin_, INPUT_PULLUP);
      attachInterrupt(digitalPinToInterrupt(pin_), ISR, RISING);
    }
    //NOTE: the reason you can' have an ISR method is because something like func() will compile to func(Class* this) when you call it
    //I.e. it autogenerates a parameter. An ISR must have no parameters so needs to be a "free" function (not attached to an instance of a class)

    //we'll use this inside of a globally defined ISR
    void increment(){
      counter_++;
    }

    unsigned long count() const{
      return counter_;
    }

    unsigned long lastCount() const{
      return lastCounter_;
    }

    void update(unsigned long interval){
      auto now = micros();
      unsigned long dt_m = now - lastTime_;
      if(dt < interval) return;//only update every 100 ms
      lastTime_ = now;

      unsigned int dc = counter_ - lastCounter_;
      lastCounter_ = counter_;

      if(dc == 0){
        rpm_ = 0;
        return;
      }

      float revs = (float)dc / COUNTS_PER_REV_;
      double dt_s = dt_m * 1e-6f;//convert mircos to seconds;
      rpm_ = (revs/dt) * 60.0f;
    }

    float rpm() const{
      return rpm_;
    }
};
