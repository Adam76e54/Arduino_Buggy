#include "x_14_buggy.h"

constexpr uint8_t N = 200;

Buffer<N> inBuffer;
WiFiServer server(wifi::PORT);
WiFiClient GUI;

L293D driver(11, 12, 6, 7, 10, 9);

HCSR04 ears(5, 4);

ROB12629 leftEncoder(2), rightEncoder(3);

constexpr uint8_t IR_DEADBAND = 64;
TCRT5000 leftIR(1, 8, IR_DEADBAND), rightIR(0, 13, IR_DEADBAND);

void setup(){
  // Serial.begin(115200);

  wifi::initialiseAccessPoint();
  server.begin();

  driver.begin();
  ears.begin();
  leftIR.begin(); rightIR.begin();

  leftEncoder.begin(ISRL); rightEncoder.begin(ISRR);
}

constexpr float KP = 1, KI = 0, KD = 0;
float integralLeft = 0, integralRight;
float lastErrLeft = 0, lastErrRight = 0;
unsigned lastTimeLeft = 0, lastTimeRight = 0;

void loop(){
  WiFiClient newClient = server.available();
  if(newClient){
    GUI = newClient;
  }

  //read commands from the GUI, could be multiple commands
  read(GUI, inBuffer);

  //handle commands in Buffer
  handle(inBuffer);
  
  //control the speed percentages
  noInterrupts();
  float leftRPM= leftEncoder.rpm();
  float rightRPM = rightEncoder.rpm();
  interrupts(); 

  //set speeds based on the PID update
  state::leftSpeedPercentage = pidUpdate(state::targetLeftRPM, leftRPM, lastTimeLeft, lastErrLeft, integralLeft);
  state::rightSpeedPercentage = pidUpdate(state::targetRightRPM, rightRPM, lastTimeRight, lastErrRight, integralRight);

  if(float distance = ears.poll(200); distance > 20){
    driver.forward(state::leftSpeedPercentage, state::rightSpeedPercentage);
  } else{
    //STOP IF NOTE SAFE
    driver.coast();

    //Reset all speed and error stuff
    state::targetLeftRPM = state::targetRightRPM = 0;
    state::leftSpeedPercentage = leftRPM = lastErrLeft = integralLeft = 0;
    state::rightSpeedPercentage = rightRPM = lastErrRight = integralRight = 0;

    delay(10);
  }  
}

//Interrupt Service Routines for the encoders
void ISRL(){
  leftEncoder.poll();
}
void ISRR(){
  rightEncoder.poll();
}

float pidUpdate(float target, float measured, unsigned long &lastTime, float &lastErr, float &integral) {
  unsigned long now = millis();
  float dt = (now - lastTime) * 1e-3;
  if (dt <= 0) dt = 0.001;     // avoid divide‑by‑zero

  float error = target - measured;

  // integral with simple anti‑windup
  integral += error * dt;

  // //clamp integral 
  // float maxIntegral = state::MAX_RPM;    
  // if (integral > maxIntegral) integral = maxIntegral;
  // if (integral < -maxIntegral) integral = -maxIntegral;

  float derivative = (error - lastErr) / dt;

  float output = KP * error + KI * integral + KD * derivative;
  output += target; // output actual speed to be used, not the 
  output /= state::MAX_RPM; //  convert to a percentage of max rpm (we'll need to record this empirically)

  lastErr = error;
  lastTime = now;

  // map to 0–1 (assuming output is roughly in −1 .. +1)
  if (output < 0) output = 0;
  if (output > 1) output = 1;

  return output;
}

