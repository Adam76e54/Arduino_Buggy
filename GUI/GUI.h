#pragma once
#include "x_14_buggy.h"
#include "StateVariables.h"
#include "Commands.h"
#include "Buffer.h"

void keep(WiFiClient& GUI, WiFiServer& server){
  if(!GUI){
    GUI = server.available();
  }
}

void handleSpeed(char command[], const uint8_t n);
void handleDirections(char command[], const uint8_t n);
void handleModes(char command[]);
void handleThresholds(char command[]);
void handleDistance(char command[]);
void handleStopStart(char command[]);


//read command into buffer
template <uint8_t N> 
bool read(WiFiClient& GUI, Buffer<N>& buffer){
  if(GUI && GUI.connected()){
    for(uint8_t i = 0; i < 5; ++i){//we can tune this
      while(GUI.available() > 0){
        byte commandByte = GUI.read();
        buffer.write(commandByte);
        if(commandByte == '\n')break;
      }
    }
  }
  return false;
}

//handle current buffer of commands
template <uint8_t N> 
void handle(Buffer<N>& buffer, L293D& driver){
  for(uint8_t i = 0; i < 5; ++i){//we can tune this
    if(state::mode == MANUAL){
      manualHandle(buffer, driver);
    }
    if(state::mode == TRACK){
      trackHandle(buffer, driver);
    }
    if(state::mode == FOLLOW){
      followHandle(buffer, driver);
    }
  }
}

//********HANDLING BRANCHES********
template <uint8_t N> 
void manualHandle(Buffer<N>& buffer, L293D& driver){
  if(buffer.available() > 0){
    char command[N];
    const uint8_t n = buffer.read(command, N);
    // Serial.println(command);
    // if(Serial) Serial.println(command);
    handleSpeed(command, n);
    handleModes(command, driver);
    handleDirections(command, n, driver);
    handleThresholds(command);

  }
}

template <uint8_t N> 
void trackHandle(Buffer<N>& buffer, L293D& driver){
  if(buffer.available() > 0){
    char command[N];
    const uint8_t n = buffer.read(command, N);
    // if(Serial) Serial.println(command);
    handleSpeed(command, n);
    handleModes(command, driver);
    handleThresholds(command);
    handleStopStart(command);
  }
}

template <uint8_t N> 
void followHandle(Buffer<N>& buffer, L293D& driver){
  if(buffer.available() > 0){
    char command[N];
    buffer.read(command, N);

    handleModes(command, driver);
  }
}

void handleSpeed(char command[], const uint8_t n){
  static float lastPercentage = 0;
  const uint8_t length = strlen(comm::SPEED);
  if(n > length && command[0] != '\0' && strncmp(command, comm::SPEED, length) == 0 && command[length] == comm::DELIMITER){    
    const char* value = command + length + 1;//points to the first character of the value-portion of the command
    float percentage = atof(value);
    
    if(lastPercentage != percentage){
      lastPercentage = percentage;
      state::leftSpeedPercentage = state::rightSpeedPercentage = percentage;
      state::targetLeftRPM = state::targetRightRPM = state::MAX_RPM * percentage;

      // sendEvent(GUI, "Speed changed to ", percentage);
      // if(Serial) Serial.println("Changed speed");
    }

  }
}

void handleDirections(char command[], const uint8_t n, L293D& driver){
  const uint8_t length = strlen(comm::DIRECTION);
  if(n > length && command[0] != '\0' && strncmp(command, comm::DIRECTION, length) == 0 && command[length] == comm::DELIMITER){ 
    const char* payload = command + length + 1;//points to the first character of the value-portion of the command

    constexpr float offset = 0.21;//raw offset to match the motors. Not very fancy but works

    if(strncmp(payload, comm::W, strlen(comm::W)) == 0){
      //just use the global speeds
      // Serial.println(payload);
      driver.forward(state::leftSpeedPercentage - offset, state::rightSpeedPercentage);
    }
    if(strncmp(payload, comm::A, strlen(comm::A)) == 0){
      // driver.forward(0,state::rightSpeedPercentage);
      driver.leftBackward(state::leftSpeedPercentage - offset - offset);
      driver.rightForward(state::rightSpeedPercentage - offset);
    }
    if(strncmp(payload, comm::S, strlen(comm::S)) == 0){
      driver.backward(state::leftSpeedPercentage - offset, state::rightSpeedPercentage);
    }
    if(strncmp(payload, comm::D, strlen(comm::D)) == 0){
      // driver.forward(state::leftSpeedPercentage - offset,0);
      driver.leftForward(state::leftSpeedPercentage - offset - offset);
      driver.rightBackward(state::rightSpeedPercentage - offset);
      
    }
    if(strncmp(payload, comm::STOP, strlen(comm::STOP)) == 0){
      driver.coast();
    }
  }  
}
void handleModes(char command[], L293D& driver){
  if(strncmp(command, comm::MANUAL, strlen(comm::MANUAL)) == 0){  
    state::mode = MANUAL;
    driver.coast();//immediately stop
  }  
  if(strncmp(command, comm::TRACK, strlen(comm::TRACK)) == 0){  
    state::mode = TRACK;
    state::stopped = true;
    driver.coast();
  }  
  if(strncmp(command, comm::FOLLOW, strlen(comm::FOLLOW)) == 0){  
    state::mode = FOLLOW;
    driver.coast();
  }  
}

void handleThresholds(char command[]){

  constexpr uint8_t delimiterSize = 1;
  uint8_t irSize = strlen(comm::IR);
  if(strncmp(command, comm::LEFT, strlen(comm::LEFT)) == 0){
    const char* nextPart = command + strlen(comm::LEFT) + delimiterSize;

    if(strncmp(nextPart, comm::IR, irSize) == 0){
      const char* payload = nextPart + irSize + delimiterSize;
      unsigned int value = atof(payload);
      state::leftThreshold = value;
    }
  }
  if(strncmp(command, comm::RIGHT, strlen(comm::RIGHT)) == 0){
    const char* nextPart = command + strlen(comm::RIGHT) + delimiterSize;

    if(strncmp(nextPart, comm::IR, irSize) == 0){
      const char* payload = nextPart + irSize + delimiterSize;
      unsigned int value = atof(payload);
      state::rightThreshold = value;
    }
  }
}

void handleDistance(char command[]){
  constexpr uint8_t delimiterSize = 1;
  if(strncmp(command, comm::ULTRA_SONIC, strlen(comm::ULTRA_SONIC)) == 0){

    const char* payload = command + strlen(comm::ULTRA_SONIC) + delimiterSize;
    unsigned int value = atof(payload);
    state::leftThreshold = value;
    
  }
}

void handleStopStart(char command[]){
  if(strncmp(command, comm::STOP, strlen(comm::STOP)) == 0){
    state::stopped = true;
  }
  if(strncmp(command, comm::START, strlen(comm::START)) == 0){
    state::stopped = false;
  }
}

//++++++++SENDING COMMANDS++++++++
void sendIRleft(WiFiClient& GUI, unsigned int left){
  if(GUI && GUI.connected()){
    // GUI.print(comm::EVENT);
    // GUI.print(comm::DELIMITER);
    GUI.print(comm::LEFT);
    GUI.print(comm::DELIMITER);
    GUI.print(comm::IR);
    GUI.print(comm::DELIMITER);
    GUI.println(left);
  }
}

void sendIRright(WiFiClient& GUI, unsigned int right){
  if(GUI && GUI.connected()){
    // GUI.print(comm::EVENT);
    // GUI.print(comm::DELIMITER);
    GUI.print(comm::RIGHT);
    GUI.print(comm::DELIMITER);
    GUI.print(comm::IR);
    GUI.print(comm::DELIMITER);
    GUI.println(right);
  }
}

void sendDistance(WiFiClient& GUI, float distance){
  if(GUI && GUI.connected()){
    // GUI.print(comm::EVENT);
    // GUI.print(comm::DELIMITER);
    GUI.print(comm::ULTRA_SONIC);
    GUI.print(comm::DELIMITER);
    GUI.println(distance);
  }
}

void sendEvent(WiFiClient& GUI, const char* message){
  if(GUI && GUI.connected()){
    GUI.print(comm::EVENT);
    GUI.print(comm::DELIMITER);
    GUI.println(message);
  }
}

void sendEvent(WiFiClient& GUI, const char* message, float num){
  if(GUI && GUI.connected()){
    GUI.print(comm::EVENT);
    GUI.print(comm::DELIMITER);
    GUI.print(message);
    GUI.println(message);
  }
}