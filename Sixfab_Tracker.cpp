/*
  Sixfab_Tracker.cpp 
  -
  Library for Sixfab Tracker Shield for Arduino.
  -
  Created by Yasin Kaya (selengalp), September 10, 2018.
*/

#include "Sixfab_Tracker.h"

SoftwareSerial M95_AT(8,7); // RX, TX - 9600 baud rate 

// default
SixfabTracker::SixfabTracker()
{

}

/******************************************************************************************
 *** Base Functions : Set or Clear Hardwares - Status Controls - Helper AT Functions  *****
 ******************************************************************************************/

// function for initializing M95 module.
void SixfabTracker::init()
{
  // setting pin directions
  pinMode(USER_LED, OUTPUT);
  pinMode(USER_BUTTON, INPUT);
  pinMode(ENABLE, OUTPUT);
  pinMode(M95_POWERKEY,OUTPUT);
  pinMode(STATUS,INPUT);

  disable();
  delay(1000);
  enable();

  // setting serials
  M95_AT.begin(9600);
  DEBUG.begin(115200);
  L96.begin(9600);

  powerUp();  

  DEBUG.println("Module initializing");
  delay(1000); // wait until module is ready 

  sendATComm("ATE1","OK\r\n"); 
  sendATComm("AT","OK\r\n");
}

// power up M95 module and all peripherals from voltage regulator 
void SixfabTracker::enable()
{
  digitalWrite(ENABLE,LOW);
}

// power down M95 module and all peripherals from voltage regulator 
void SixfabTracker::disable()
{
  digitalWrite(ENABLE,HIGH);
}

// power up or down M95 module
void SixfabTracker::powerUp()
{  
  delay(100);
  digitalWrite(M95_POWERKEY,HIGH);
  
  while(getModemStatus()){
    //DEBUG.println(getModemStatus());
  }
  digitalWrite(M95_POWERKEY,LOW);
}

// power up or down M95 module
uint8_t SixfabTracker::getModemStatus()
{
  return digitalRead(STATUS);
}

// send at comamand to module
void SixfabTracker::sendATCommOnce(const char *comm)
{
  M95_AT.print(comm);
  M95_AT.print("\r");
  //DEBUG.println(comm);
}

// get response from modem
const char* SixfabTracker::getResponse(const char *desired_reponse)
{
  char response[AT_RESPONSE_LEN]; // module response for AT commands.
  char c;
  int i = 0;
  while(1){
    while(M95_AT.available()){
        c = M95_AT.read();
        DEBUG.write(c);
        response[i++]=c;
        delay(2);
        }

    if(strstr(response, desired_reponse)){
      return response;
      memset(response, 0 , strlen(response));
      break;
    }
  }
}

// function for sending at command to M95_AT.
const char* SixfabTracker::sendATComm(const char *command, const char *desired_reponse)
{
  uint32_t timer;
  char response[AT_RESPONSE_LEN]; // module response for AT commands. 

  memset(response, 0 , AT_RESPONSE_LEN);
  M95_AT.flush();

  sendATCommOnce(command);
 
  timer = millis();
   while(true){
    if(millis()-timer > timeout){
      sendATCommOnce(command);
      timer = millis();
    }
    char c;
    int i = 0; 

    while(M95_AT.available()){
      c = M95_AT.read();
      DEBUG.write(c);
      response[i++]=c;
      delay(2);
      }
      if(strstr(response, desired_reponse)){
        return response;
        memset(response, 0 , strlen(response));
        break;
      }    
  }
}

// function for sending data to M95_AT.
const char* SixfabTracker::sendDataComm(const char *command, const char *desired_reponse)
{
  uint32_t timer;
  char response[AT_RESPONSE_LEN]; // module response for AT commands. 

  memset(response, 0 , AT_RESPONSE_LEN);
  M95_AT.flush();

  M95_AT.print(command);
 
  timer = millis();
   while(true){
    if(millis()-timer > timeout){
      M95_AT.print(command);
      timer = millis();
    }
    char c;
    int i = 0; 

    while(M95_AT.available()){
      c = M95_AT.read();
      DEBUG.write(c);
      response[i++]=c;
      delay(2);
      }
      if(strstr(response, desired_reponse)){
        return response;
        memset(response, 0 , strlen(response));
        break;
      }    
  }
}

// function for reset M95_AT module
void SixfabTracker::resetModule()
{
  saveConfigurations();
  delay(200);

  digitalWrite(ENABLE,HIGH);
  delay(200);
  digitalWrite(ENABLE,LOW);
  delay(200);

  powerUp();
}

// Function for save configurations that be done in current session. 
void SixfabTracker::saveConfigurations()
{
  sendATComm("AT&W","OK\r\n");
}

// Function for getting IMEI number
const char* SixfabTracker::getIMEI()
{
  return sendATComm("AT+CGSN","OK\r\n");
}

// Function for getting firmware info
const char* SixfabTracker::getFirmwareInfo()
{
  return sendATComm("AT+CGMR","OK\r\n");
}

//Function for getting hardware info
const char* SixfabTracker::getHardwareInfo()
{
  return sendATComm("AT+CGMM","OK\r\n");
}

// function for getting ip_address
const char* SixfabTracker::getIPAddress()
{
  return ip_address;
}

// function for setting ip_address
void SixfabTracker::setIPAddress(char *ip)
{
  strcpy(ip_address, ip);
}

// function for getting domain_name
const char* SixfabTracker::getDomainName()
{
  return domain_name;
}

// function for setting domain name
void SixfabTracker::setDomainName(char *domain)
{
	strcpy(domain_name, domain);
}

// function for getting domain_name
const char* SixfabTracker::getPort()
{
  return port_number;
}

// function for setting port
void SixfabTracker::setPort(char *port)
{
	strcpy(port_number, port);
}

// get timout in ms
uint16_t SixfabTracker::getTimeout()
{
  return timeout;
}

// set timeout in ms    
void SixfabTracker::setTimeout(uint16_t new_timeout)
{
  timeout = new_timeout; 
}


/******************************************************************************************
 *** Network Service Functions ************************************************************
 ******************************************************************************************/ 

//
const char* SixfabTracker::getSignalQuality()
{
  return sendATComm("AT+CSQ","OK\r\n");
}

// connect to base station of operator
void SixfabTracker::connectToOperator()
{
  DEBUG.println("Trying to connect base station of operator...");
  setTimeout(3000);

  sendATComm("AT+CGREG?","+CGREG: 0,1\r\n");
  getSignalQuality(); 
}

/******************************************************************************************
 *** L96 Functions ***********************************************************************
 ******************************************************************************************/
// function for getting raw nmea messages
char SixfabTracker::getRawCharFromL96()
{
  return L96.read();
}

/******************************************************************************************
 *** TCP & UDP Protocols Functions ********************************************************
 ******************************************************************************************/

// function for configurating and activating TCP context 
void SixfabTracker::activateContext()
{
  sendATComm("AT+CGDCONT=1,\"IP\",\"CMNET\"","OK\r\n"); 
  delay(1000);
  sendATComm("AT+CGACT=1,1","\r\n");
}

// function for deactivating TCP context 
void SixfabTracker::deactivateContext()
{
  sendATComm("AT+CGACT=0,1","\r\n");
}

// function for connecting to server via TCP
// just buffer access mode is supported for now.
void SixfabTracker::connectToServerTCP()
{
  strcpy(compose, "AT+QIOPEN=");
  strcat(compose, "\"TCP\",\"");
  strcat(compose, ip_address);
  strcat(compose, "\",");
  strcat(compose, port_number);
  sendATComm(compose,"OK\r\n");

  sendATComm("AT+QISTATE","OK\r\n");
  clear_compose();
}

// fuction for sending data via tcp.
// just buffer access mode is supported for now.
void SixfabTracker::sendDataTCP(const char *data)
{
  char data_len[4];
  sprintf(data_len, "%d", strlen(data));

  strcpy(compose, "AT+QISEND=");
  strcat(compose, data_len);

  sendATComm(compose,">");
  sendATComm(data,"SEND OK");
  clear_compose();
}

// function for connecting to server via UDP
void SixfabTracker::startUDPService()
{
  strcpy(compose, "AT+QIOPEN=\"UDP\",\"");
  strcat(compose, ip_address);
  strcat(compose, "\",");
  strcat(compose, port_number);
  
  sendATComm(compose,"OK\r\n");
  clear_compose();

  sendATComm("AT+QISTATE","\r\n");
}

// fuction for sending data via udp.
void SixfabTracker::sendDataUDP(const char *data)
{
  char data_len[5];
  sprintf(data_len, "%d", strlen(data));

  strcpy(compose, "AT+QISEND=");
  strcat(compose, data_len);

  sendATComm(compose,">");
  sendATComm(data,"SEND OK");
  clear_compose();
}

// function for closing server connection
void SixfabTracker::closeConnection()
{
  sendATComm("AT+QICLOSE","\r\n");
}

/******************************************************************************************
 *** Peripheral Devices' Functions : Read button and set LEDs ********************
 ******************************************************************************************/  

//
uint8_t SixfabTracker::readUserButton()
{
  return digitalRead(USER_BUTTON);	
  // LOW when pressed
}

//
void SixfabTracker::turnOnUserLED()
{
  digitalWrite(USER_LED, HIGH);
}

//
void SixfabTracker::turnOffUserLED()
{
  digitalWrite(USER_LED, LOW);
}

