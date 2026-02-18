#include "x_14_buggy.h"

constexpr uint8_t N = 200;

Buffer<N> inBuffer;
WiFiServer server(wifi::PORT);
WiFiClient GUI;

L293D driver(11, 12, 6, 7, 10, 9);

HCSR04 ears(5, 4);

ROB12629 leftEncoder(2), rightEncoder(3);

//Interrupt Service Routines for the encoders
void ISRL(){
  leftEncoder.increment();
}
void ISRR(){
  rightEncoder.increment();
}

void setup(){
  Serial.begin(115200);

  wifi::initialiseAccessPoint();
  server.begin();

  driver.begin();
  ears.begin();

  leftEncoder.begin(ISRL); rightEncoder.begin(ISRR);
}

unsigned long lastTime = 0;
void loop(){
  keep(GUI, server);

  //read commands from the GUI, could be multiple commands
  read(GUI, inBuffer);

  //handle commands in Buffer
  handle(inBuffer, driver);

  //update rpm
  leftEncoder.update(100);
  rightEncoder.update(100);

  //print the current rpm on the left side
  auto now = millis();
  if(now - lastTime > 500){
    lastTime = now;

    //sendEvent(GUI, "Left dc = ", leftEncoder.count() - leftEncoder.lastCount());
    //sendEvent(GUI, "Right dc = ", rightEncoder.count() - rightEncoder.lastCount());

    sendEvent(GUI, "Left rpm = ", leftEncoder.rpm());
    sendEvent(GUI, "Right rpm = ", rightEncoder.rpm());
  }

  driver.forward(1 - 0.18, 1);
}

