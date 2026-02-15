#include "x_14_buggy.h"
#include "testfunctions.h"
//EN1,2 = 9, EN3,4 = 10
//1A = 7, 2A = 11
//3A = 4, 4A = 6
L293D driver(11, 12, 6, 7, 10, 9);

float initialTime = 0;
float dt = 1e9;//assume an initially slow velocity until the encoder corrects dt
constexpr float ds = 1;//wheel circumference

ROB12629 encoderL(2), encoderR(3);
void ISRL(){
  encoderL.increment();

  float now = micros();
  dt = now - initialTime;
  initialTime = now;
}
void ISRR(){
  encoderR.increment();

  float now = micros();
  dt = now - initialTime;
  initialTime = now;
}

WiFiServer server(80);
WiFiClient GUI;

constexpr uint8_t N = 20;
Buffer<100> buffer;

void setup() {
  // Serial.begin(115200);
  // delay(2000);

  driver.begin();

  encoderL.begin(ISRL);
  encoderR.begin(ISRR);

  //wifi::connect("hotspot", "macgiollaeaspaig", address);
  //wifi::connect("VM7354947", "fcaedyaPckusc4tb", address);
  wifi::initialiseAccessPoint();
  server.begin();
}

float speedToUse = 0;

void loop() {
  GUI = server.available();

  driver.forward(speedToUse);

  read(GUI, buffer);
  handle(buffer);


  float speedToSend= ds/dt;
  sendSpeed(GUI, speedToSend);

  
}
