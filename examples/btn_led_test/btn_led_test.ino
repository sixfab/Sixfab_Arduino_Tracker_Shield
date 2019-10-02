/*
  btn_led_test.ino - This is example of testing button and led.
  Created by Yasin Kaya (selengalp), September 11, 2018.
*/

#include "Sixfab_Tracker.h"

SixfabTracker node;

// setup
void setup() {
  
  node.init();
  
  node.turnOnUserLED();
  delay(TIMEOUT);
  node.turnOffUserLED();
  delay(TIMEOUT);
 
}

// loop
void loop() {

}