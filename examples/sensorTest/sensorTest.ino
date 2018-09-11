/*
  sensorTest.ino - This is example of testing sensors.
  Created by Yasin Kaya (selengalp), September 11, 2018.
*/

#include "Sixfab_GPRSIoT.h"

SixfabGPRSIoT node;

double ax, ay, az;

// setup
void setup() {
  
  node.init();
  
  node.turnOnUserLED();
  delay(TIMEOUT);
  node.turnOffUserLED();
  delay(TIMEOUT);
  node.turnOnRelay();
  delay(TIMEOUT);
  node.turnOffRelay();
  delay(TIMEOUT);

  node.readAccel(&ax, &ay, &az);
  DEBUG.print("ax: "); DEBUG.println(ax);
  DEBUG.print("ay: "); DEBUG.println(ay);
  DEBUG.print("az: "); DEBUG.println(az);
  
  DEBUG.println("Temperature: ");DEBUG.print(node.readTemp());DEBUG.println();
  delay(TIMEOUT);
  DEBUG.println("Humidity: ");DEBUG.print(node.readHum());DEBUG.println();
  delay(TIMEOUT);
  DEBUG.println("Light: ");DEBUG.print(node.readLux());
  delay(TIMEOUT);
}

// loop
void loop() {

}