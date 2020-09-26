
/*
**  AT_Command.ino Sketch
**
**  
**
**
*/

#include "Sixfab_Tracker.h"
SixfabTracker node;

void setup() {
  
  while(!DEBUG);  // Waits for serial monitor
  delay(2000);
  node.init();    // Initializes the HW

node.sendSMS("123456789","Hello"); //Replace 12346789 with phone number and Hello with message


}

void loop() {
  // put your main code here, to run repeatedly:
      
}
