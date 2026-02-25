#include "x_14_buggy.h"

constexpr uint8_t N = 200;

Buffer<N> inBuffer;
WiFiServer server(wifi::PORT);
WiFiClient GUI;

// L293D driver(11, 12, 6, 7, 10, 9);//old set up
L293D driver(6,7,11,12,9,10);//new setup

HCSR04 ears(5, 4);
int left = analogRead(1);
TCRT5000 leftEye(1, 8, 1), rightEye(0, 13, 1);

void setup(){

  Serial.begin(115200);

  wifi::initialiseAccessPoint();
  server.begin();

  driver.begin();
  ears.begin();
  leftEye.begin(), rightEye.begin();
}

void loop(){
  keep(GUI, server);

  //read commands from the GUI, could be multiple commands
  read(GUI, inBuffer);

  //handle commands in Buffer
  handle(inBuffer, driver);

  //poll sensors
  constexpr uint8_t US_POLLING_RATE = 150;
  float distance = ears.poll(US_POLLING_RATE); 
  unsigned int leftEyeValue = leftEye.band();
  unsigned int rightEyeValue = rightEye.band();

  //act on sensor data (drive)
  if(state::mode == MANUAL){
    // Serial.println("Mode = Manual");
    manualLoop();//this currently does nothing
  }
  if(state::mode == TRACK){
    // Serial.println("Mode = Track");
    bool left = leftEyeValue > state::leftThreshold;
    bool right = rightEyeValue > state::rightThreshold;
    trackLoop(left, right, distance, GUI);
  }

  //tick through the sending commands every 500 ms (may be able to tweek it to be lower)
  //NOTE: we tick through rather than send everything at once to avoid slowing the IR sensing
  constexpr unsigned int SEND_RATE = 400;
  static unsigned long lastTime = 0; 
  static uint8_t clock = 0, numberOfSends = 3;
  auto now = millis();
  if(now - lastTime > SEND_RATE){
    // if(state::stopped) sendEvent(GUI, "Stopped state = ", state::stopped);
    lastTime = now;
    clock = (clock + 1)% numberOfSends;
    if(clock % numberOfSends == 0) sendIRright(GUI, rightEyeValue);

    if(clock % numberOfSends == 1) sendIRleft(GUI, leftEyeValue);

    if(clock % numberOfSends == 2) sendDistance(GUI, distance);

  }

  //poll sensors again 
  distance = ears.poll(US_POLLING_RATE); 
  leftEyeValue = leftEye.band();
  rightEyeValue = rightEye.band();

  //act on sensor data (drive)
  if(state::mode == MANUAL){
    // Serial.println("Mode = Manual");
    manualLoop();//this currently does nothing
  }
  if(state::mode == TRACK){
    // Serial.println("Mode = Track");
    bool left = leftEyeValue > state::leftThreshold;
    bool right = rightEyeValue > state::rightThreshold;
    trackLoop(left, right, distance, GUI);
  }
}

void manualLoop(){
  //do very little? We could add the PID logic for straight driving here
}

void trackLoop(bool left, bool right, unsigned int distance, WiFiClient& GUI){
  // constexpr float minTurnSpeed = 0.4;//may need to set this to 0 but we'll tune it to get nicer movement if we can
  static uint8_t closeCount = 0, farCount = 0;
  constexpr uint8_t MAX_CLOSE_COUNT = 15;
  constexpr uint8_t MAX_FAR_COUNT = 15; 

  //sanity check to filter dodgy sensor readings
  if(distance <= state::maxDistance){

    if(closeCount < MAX_CLOSE_COUNT){
      ++closeCount;
    } 
    farCount = 0;
    
  } else {

    if(farCount < MAX_FAR_COUNT){
      ++farCount;
    }
    closeCount = 0;
    
  }

  // we're doing some debouncing here in code because there was jitter
  static bool safe = false;
  if(closeCount >= MAX_CLOSE_COUNT){
    safe = false;
  } else if(farCount >= MAX_FAR_COUNT){
    safe = true;
  }

  static bool previousObstacle = false;  
  if(!state::stopped && safe){//if the distance is safe and we're not stop-commanded, go ahead

    previousObstacle = false;//if we're moving then this should be reset

    constexpr float turnFactor = 0.3;
    constexpr float offset = 0.18;

    if(left && right){
      driver.forward(state::leftSpeedPercentage - offset, state::rightSpeedPercentage);
    } else if(left && !right){
      driver.forward((state::leftSpeedPercentage - offset), state::rightSpeedPercentage * turnFactor);
    } else if(!left && right){
      driver.forward((state::leftSpeedPercentage - offset) * turnFactor, state::rightSpeedPercentage);
    } else{
      driver.coast();
    }

  } else{

    driver.coast();
    if(!safe && !previousObstacle){//if there previously wasn't an previousObstacle but now there is, send event
      previousObstacle = true;
      sendEvent(GUI, comm::OBSTACLE_MESSAGE);
    }

    if(previousObstacle && safe){
      previousObstacle = false;
      sendEvent(GUI, comm::REMOVED_MESSAGE);
    }
  }
}