#include "x_14_buggy.h"

constexpr uint8_t N = 200;

Buffer<N> inBuffer;
WiFiServer server(wifi::PORT);
WiFiClient GUI;

// L293D driver(11, 12, 6, 7, 10, 9);//old set up
L293D driver(6,7,11,12,9,10);//new setup


HCSR04 ears(5, 4);

ROB12629 leftEncoder(2), rightEncoder(3);

TCRT5000 leftEye(1, 8, 1), rightEye(0, 13, 1);

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
  leftEye.begin(), rightEye.begin();
  leftEncoder.begin(ISRL); rightEncoder.begin(ISRR);
}

unsigned long lastTime = 0;
void loop(){
  keep(GUI, server);

  //read commands from the GUI, could be multiple commands
  read(GUI, inBuffer);

  //handle commands in Buffer
  handle(inBuffer, driver);

  float distance = ears.poll(200); 
  unsigned int leftEyeValue = leftEye.band();
  unsigned int rightEyeValue = rightEye.band();
  
  auto now = millis();
  if(now - lastTime > 100){
    lastTime = now;
    sendIR(GUI, leftEyeValue, rightEyeValue);
    sendDistance(GUI, distance);

    // sendEvent(GUI, state::rightThreshold);
  }

  if(state::mode == MANUAL){
    manualLoop();//this currently does nothing
  }
  if(state::mode == TRACK){
    bool left = leftEyeValue > state::leftThreshold;
    bool right = rightEyeValue > state::rightThreshold;
    trackLoop(left, right, distance);
  }

}

void manualLoop(){
  //do very little? 
}

void trackLoop(bool left, bool right, unsigned int distance){
  constexpr float offset = 0.21;
  constexpr float scaleDown = 1;
  if(distance > state::maxDistance){

    if(left && right){
      driver.forward(state::leftSpeedPercentage - offset, state::rightSpeedPercentage);
    } else if(left && !right){
      driver.forward((state::leftSpeedPercentage - offset), 0);
    } else if(!left && right){
      driver.forward(0, (state::rightSpeedPercentage));
    } else{
      driver.coast();
    }

  } else{
    driver.coast();
    delay(200);
  }
}