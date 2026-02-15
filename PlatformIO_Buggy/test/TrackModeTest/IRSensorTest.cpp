#include "x_14_buggy.h"

constexpr uint8_t IR_DEADBAND = 1;
TCRT5000 leftIR(1, 8, IR_DEADBAND), rightIR(0, 13, IR_DEADBAND);

void setup(){
  Serial.begin(115200);
}

unsigned long lastTime = 0;
void loop(){
  auto now = millis();
  if(now - lastTime > 1000){
    Serial.println(leftIR.band());
    lastTime = now;
  }
}

//left side read around 40 - 50 on the white line, 
