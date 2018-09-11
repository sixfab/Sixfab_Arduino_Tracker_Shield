/*
  localHost.ino - This is basic local host example.
  Created by Yasin Kaya (selengalp), September 11, 2018.
*/

#include "Sixfab_GPRSIoT.h"

SixfabGPRSIoT node;

char your_ip[] = "xx.xx.xx.xx"; // change with your ip
char your_port[] = "xxxx"; // change with your port

// setup
void setup() {
  
  node.init();

  node.getIMEI();
  node.getFirmwareInfo();
  node.getHardwareInfo();

  node.setIPAddress(your_ip);
  node.setPort(your_port);
  
  node.connectToOperator();
  node.getSignalQuality();

  node.activateContext();
  node.connectToServerTCP();
  
  node.sendDataTCP("Hello World!\r\n");
}

// loop
void loop() {

}