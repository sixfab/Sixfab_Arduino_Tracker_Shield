/*
  tracker_test.ino - This is example of testing tracker shield roughly.
  Created by Saeed Johar (saeedjohar), October 2, 2019.
*/

#include "Sixfab_Tracker.h"

SixfabTracker node;

String gpsSentence;
// setup
void setup() {
  delay(2000);
  node.init();

  DEBUG.println("Press button to continue test");
  while(1){
    if(node.readUserButton() == 0) //0 when pressed
      break;
    }
  node.turnOnUserLED();
  delay(TIMEOUT);
  node.turnOffUserLED();

  node.getModemStatus();
  node.getIMEI();
  node.getSignalQuality();
  
  
  DEBUG.println("Press button to finish test");
  while(1){
    if(node.readUserButton() == 0) //long press the user button to stop the GNSS data.
      break;
    else {
        gpsSentence = L96.readStringUntil(13);
        gpsSentence.trim();
        DEBUG.println(gpsSentence);
      }
      
    }

 
}

// loop
void loop() {

}
