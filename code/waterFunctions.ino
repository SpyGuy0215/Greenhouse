#include <ArduinoJson.h>
#include <SPI.h>
#include <Arduino_MKRENV.h>
#include <WiFiNINA.h>
#include <ArduinoOTA.h>
#include <utility/wifi_drv.h>

void alarmInit() {
  rtc.setAlarmTime(8, 0, 0); 
  rtc.enableAlarm(rtc.MATCH_HHMMSS);
  rtc.attachInterrupt(pumpWater);

}

void waterLevelRead() {
  int level = analogRead(waterSensorPin);  
  if(level == LOW) {
    waterIn = true; 
  }
  else {
    waterIn = false; 
    Serial.println("Water Level is low, please refill the water!"); 
  }
  Serial.println(waterIn); 
  Serial.println(level); 

}

void pumpWater() {
  if(waterOn == false) {
    Serial.println("");
    digitalWrite(motorPin1, LOW); 
    waterOn = true; 
    rtc.disableAlarm(); 
    int flowRateSecs = rtc.getSeconds() + (WATER_AMOUNT/1.666666); //Calculate the time it takes for WATER_AMOUNT to flow at a speed of 100ml/minute, which is the max speed
    int flowRateMins = rtc.getMinutes(); 
    if(flowRateSecs >= 60) {
      flowRateSecs = flowRateSecs % 60; 
      flowRateMins++; 
    }  
    rtc.setAlarmTime(rtc.getHours(), flowRateMins, flowRateSecs);
    rtc.enableAlarm(rtc.MATCH_HHMMSS); 
    rtc.attachInterrupt(alarmResponse);  
    Serial.println("Time: "); 
    Serial.println(rtc.getMinutes()); 
    Serial.println(rtc.getSeconds()); 
    Serial.println(rtc.getDay()); 
    Serial.print("Water end time set for: ");
    Serial.print(flowRateMins); 
    Serial.print(":"); 
    Serial.println(flowRateSecs); 
  }

  else {
    digitalWrite(motorPin1, HIGH); 
    waterOn = false; 
  }
}

void alarmResponse() {
  Serial.println("Done-zo!"); 
  pumpWater(); 
}
