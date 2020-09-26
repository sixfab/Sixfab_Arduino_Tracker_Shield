
/*
**  GPS_Parse.ino Sketch
**
**  
**
**
*/

#include "Sixfab_Tracker.h"
SixfabTracker node;

String gpsSentence;


//Function to convert the raw data to degree


void setup() {
  
  while(!DEBUG);  // Waits for serial monitor
  delay(2000);
  node.init();    // Initializes the HW

  DEBUG.println("Starting test");
}

void loop() {
  //gpsSentence = L96.readStringUntil(13);
  //gpsSentence.trim();
  gpsSentence = node.getNmeaSentence();
  DEBUG.println(gpsSentence); //Uncomment it to see the RAW Data. 
  // Starts parsing if RMC sentence received
  if (gpsSentence.startsWith("$GPRMC") || gpsSentence.startsWith("$GNRMC") ) {  
  
        
    DEBUG.println(gpsSentence); // displays raw RMC sentence
     returnData();  // returns $GPRMC or $GNRMC
     removeString();  //removes $GPRMC or $GNRMC

//UTC
      float utc = returnData().toFloat();//print(utc);
      removeString();    // removes UTC

//Validity
      String validity = returnData();
      removeString();    //removes validity
    
//Latitude
      float lat = returnData().toFloat();  
      removeString();  //remove latitude from the string

//North/South
      String northOrSouth = returnData();
      removeString();
  
//Longitude
    
      float lon = returnData().toFloat();
      removeString();     //remove latitude from the string

//East/West
      String eastOrWest = returnData();
      removeString();

//Speed in knots

// TODO: Speed to be parsed
  DEBUG.print(validity);
    if ( validity == "A"){
      DEBUG.print("UTC Time "); printTime(utc);
      DEBUG.print("\tLat : ");DEBUG.print(ConvertData(lat),6);DEBUG.print("  ");DEBUG.println(northOrSouth);
      DEBUG.print("\tLon : ");DEBUG.print(ConvertData(lon),6);DEBUG.print("  ");DEBUG.println(eastOrWest);
    }
    
  }
}


float ConvertData(float RawDegrees){ 
  float RawAsFloat = RawDegrees; 
  int firstdigits = ((int)RawAsFloat)/100; // Get the first digits by turning f into an integer, then doing an integer divide by 100;
  float nexttwodigits = RawAsFloat - (float)(firstdigits*100);
  float Converted = (float)(firstdigits + nexttwodigits/60.0);
  return Converted;
}

//Function to print the raw time in hh:mm:ss format
void printTime(float rawTime){
  int hour = (int)(rawTime/10000); //DEBUG.println(hour);
  int minute = (int)(rawTime - hour*10000) /100;  //DEBUG.println(minute);
  int sec = (int)(rawTime - hour*10000 - minute*100); //DEBUG.println(sec); // 

  if (sec < 10)
    {DEBUG.print(hour);DEBUG.print(":");DEBUG.print(minute);DEBUG.print(":0");DEBUG.print(sec);}
  else
    {DEBUG.print(hour);DEBUG.print(":");DEBUG.print(minute);DEBUG.print(":");DEBUG.print(sec);}
  }

  
String returnData(){
  int pos = gpsSentence.indexOf(','); // return index of first comma (,)
  return gpsSentence.substring(0,pos); // returns the String from 0 index till comma
  }

void removeString(){
  int pos=gpsSentence.indexOf(',');
  gpsSentence.remove(0, pos+1);
  }
