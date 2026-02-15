#include "x_14_buggy.h"
#include "testfunctions.h"
//EN1,2 = 9, EN3,4 = 10
//1A = 7, 2A = 11
//3A = 4, 4A = 6
L293D driver(7, 11, 4, 6, 9, 10);
int address;
WiFiServer server(80);
WiFiClient GUI;

void setup() {
  Serial.begin(115200);
  delay(2000);
  driver.begin();
  //wifi::connect("hotspot", "macgiollaeaspaig", address);
  //wifi::connect("VM7354947", "fcaedyaPckusc4tb", address);
  wifi::initialiseAccessPoint();
  server.begin();

  Serial.print("Listening on: ");
  Serial.println(WiFi.localIP());
  WiFiClient GUI = server.available();
  if(GUI){
    GUI.print("Connected\n");
    Serial.println("Connected");
  } else{
    Serial.println("No client yet");
    delay(1000);
  }
}

void loop() {
  GUI = server.available();
  driver.forward(200, 200);
 if (GUI && Serial.available() > 0) {
    String line = Serial.readStringUntil('\n');
    GUI.println(line);
 }

}
