#include "x_14_buggy.h"

constexpr uint8_t N = 200;
Buffer<N> inBuffer;
WiFiServer server(wifi::PORT);
WiFiClient GUI;

L293D driver(11, 12, 6, 7, 10, 9);

TCRT5000 leftEye(1, 8, 1), rightEye(0, 13, 1);

void setup(){
  wifi::initialiseAccessPoint();
  server.begin();
  driver.begin();
  leftEye.begin(), rightEye.begin();
}

unsigned long lastTime = 0;
void loop(){
  keep(GUI, server);

  read(GUI, inBuffer);

  handle(inBuffer);

  auto now = millis();
  if(now - lastTime > 100){
    lastTime = now;
    sendIR(GUI, leftEye.band(), rightEye.band());
  }

  driver.forward(state::leftSpeedPercentage, state::rightSpeedPercentage);

}