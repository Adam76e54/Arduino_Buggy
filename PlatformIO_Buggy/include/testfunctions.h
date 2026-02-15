#pragma once
#include "x_14_buggy.h"

void forwardAndBackwardsTest(L293D& driver, WiFiClient& GUI){
  driver.forward(255, 255);
  GUI.print("Called forward");
  GUI.print(
    String("Left speed, right speed: ")
    + driver.leftSpeed()
    + ", "
    + driver.rightSpeed()
  );
  delay(10000);

  driver.coast();
  GUI.print("Called coast");
  GUI.print(
    String("Left speed, right speed: ")
    + driver.leftSpeed()
    + ", "
    + driver.rightSpeed()
  );
  delay(500);

  driver.backward();
  GUI.print("Called backward");
  GUI.print(
    String("Left speed, right speed: ")
    + driver.leftSpeed()
    + ", "
    + driver.rightSpeed()
  );
  delay(10000);

  driver.brake();
  GUI.print("Called brake");
  GUI.print(
    String("Left speed, right speed: ")
    + driver.leftSpeed()
    + ", "
    + driver.rightSpeed()
  );
  delay(500);
}

template <uint8_t N>
void BufferTest(WiFiClient& GUI, Buffer<N>& buffer){
  
}