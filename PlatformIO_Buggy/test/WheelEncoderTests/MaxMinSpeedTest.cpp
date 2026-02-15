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
  leftEncoder.poll();
}
void ISRR(){
  rightEncoder.poll();
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
  WiFiClient newClient = server.available();
  if(newClient){
    GUI = newClient;
  }

  //read commands from the GUI, could be multiple commands
  read(GUI, inBuffer);

  //handle commands in Buffer
  handle(inBuffer);
  float distance = ears.poll(200); 

  //print the current rpm on the left side
  auto leftRPM = leftEncoder.rpm();
  Serial.println(leftEncoder.rpm());

  auto now = millis();
  if(now - lastTime > 100){
    lastTime = now;

    GUI.print(comm::EVENT);
    GUI.print(comm::DELIMITER);
    GUI.print("Distance from ears: ");
    GUI.print(distance);
    GUI.print(" Left RPM: ");
    GUI.println(leftRPM);
  }

  if(distance > 20){
    driver.forward(state::leftSpeedPercentage, state::rightSpeedPercentage);
  } else{
    driver.coast();
    delay(10);
  }  
}

