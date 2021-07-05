#include <SPI.h>
#include <WiFiNINA.h>
#include <ArduinoOTA.h>
#include <utility/wifi_drv.h>

String incomingString; 

void readCommands() {
  if (Serial.available() > 0) {
    incomingString = Serial.readString();
    incomingString.trim();  
  
    if(incomingString.equals("/water")) {
      pumpWater(); 
    } 

    else if(incomingString.equals("/sendData")) {
      readSensors(); 
      writeData(); 
    }
  }
  }
