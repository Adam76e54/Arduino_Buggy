#pragma once
#include <Arduino.h>

//yellow = VCC, orange = TRIG, red = ECHO, brown = GND for the ultrasonic
//grey = VCC, purple = GND, blue = DO, green = AO

enum MODE : uint8_t {TRACK, MANUAL, FOLLOW};

//L293D driver(11, 12, 6, 7, 10, 9); should place these in here
namespace state{
  extern uint8_t mode;

  //distinguish between percentage speed to feed to methods and actual RPM
  extern float leftSpeedPercentage;
  extern float rightSpeedPercentage;

  constexpr uint8_t MAX_RPM = 60;

  extern float targetLeftRPM;
  extern float targetRightRPM;

  extern unsigned int leftThreshold;
  extern unsigned int rightThreshold;
  extern float maxDistance;

  extern bool stopped;
};

struct Coefficients{
  float KP, KI, KD;
};

// void manualLoop(){
//   //do very little? 
// }

// void trackLoop(bool left, bool right, unsigned int distance, WiFiClient& GUI){
//   constexpr float offset = 0.21;
//   constexpr float scaleDown = 1;
//   if(distance > state::maxDistance){

//     if(left && right){
//       driver.forward(state::leftSpeedPercentage - offset, state::rightSpeedPercentage);
//     } else if(left && !right){
//       driver.forward((state::leftSpeedPercentage - offset), 0);
//     } else if(!left && right){
//       driver.forward(0, (state::rightSpeedPercentage));
//     } else{
//       driver.coast();
//     }

//   } else{
//     driver.coast();
//     // sendEvent(GUI, "Stopped");
//     // delay(10);
//   }
// }