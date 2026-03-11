#pragma once
#include <Arduino.h>

//yellow = VCC, orange = TRIG, red = ECHO, brown = GND for the ultrasonic
//grey = VCC, purple = GND, blue = DO, green = AO

enum MODE : uint8_t {TRACK, MANUAL, FOLLOW};

namespace state{
  struct State{
    uint8_t mode = MANUAL;

    float leftSpeedPercentage = 0;
    float rightSpeedPercentage = 0;

    float targetLeftRPM = 0;
    float targetRightRPM = 0;

    float maxDistance = 15.0;
    float currentDistance = 999.0;

    bool stopped = true;
  } STATE;
  
  inline uint8_t& mode = STATE.mode;
  
  inline float& leftSpeedPercentage = STATE.leftSpeedPercentage;
  inline float& rightSpeedPercentage = STATE.rightSpeedPercentage;

  inline float& targetLeftRPM = STATE.targetLeftRPM;
  inline float& targetRightRPM = STATE.targetRightRPM;

  inline float& maxDistance = STATE.maxDistance;
  inline float& currentDistance = STATE.currentDistance;

  inline bool& stopped = STATE.stopped;

  float MAX_RPM = 120;        
  float leftThreshold;  
  float rightThreshold; 
}

struct Coefficients{
  float KP, KI, KD;
};