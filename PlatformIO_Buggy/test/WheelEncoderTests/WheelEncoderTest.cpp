#include "x_14_buggy.h"

ROB12629 leftEncoder(2), rightEncoder(3);

void left_isr() {
  leftEncoder.increment();
}

void right_isr(){
  rightEncoder.increment();
}

void setup(){
  leftEncoder.begin(left_isr);
  rightEncoder.begin(right_isr);
  Serial.begin(115200);
}

void loop() {
  Serial.println(leftEncoder.count());
  Serial.println(rightEncoder.count());
  delay(2000);
}