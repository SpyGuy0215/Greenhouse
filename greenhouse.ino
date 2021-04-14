/*
   This is the Automated Greenhouse! Have fun using it!
   Author: SpyGuy
   
   TODO: Add Failsafes (Water: v1.0, Light: v2.0)
   TODO: Add feature for multiple water times (v2.0)
   TODO: Add logging to SD Card (v2.0)
   TODO: Make React Native App (v2.0)

   //NOTE: Water pump digitalWrite is commented off until properly powered

*/

#include <ArduinoJson.h>
#include <SPI.h>
#include <Arduino_MKRENV.h>
#include "Firebase_Arduino_WiFiNINA.h"
#include <WiFiNINA.h>
#include <ArduinoOTA.h>
#include <RTCZero.h>  
#include <utility/wifi_drv.h>

//NOTE: Took away avr/dtostrf
#include "arduino_secrets.h"

char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;
int status = WL_IDLE_STATUS;
char server[] = "io.adafruit.com";
String jsonStr; 
char auth[] = FIREBASE_AUTH; 
char host[] = FIREBASE_HOST;

//////////////////////////////// 

int temperatureVal = 0;
int humidityVal = 0;
int pressureVal = 0;
int luxVal = 0;

int RGB_RED = 26;
int RGB_BLUE = 27;
int RGB_GREEN = 25;

int waterSensorPin = A1;
bool waterIn; 

int lastWatered; 

int motorPin1 = 4; 
int motorPin2 = 5; 

unsigned long lastConnectionTime = 0;
const unsigned long postingInterval = 600000; //10 Minutes

WiFiClient client;
RTCZero rtc; 

void setup() {
  //Start Serial
  Serial.begin(9600);
  rtc.begin(); 

  //Set Pinmodes
  WiFiDrv::pinMode(RGB_RED, OUTPUT);  //GREEN
  WiFiDrv::pinMode(RGB_GREEN, OUTPUT);  //RED
  WiFiDrv::pinMode(RGB_BLUE, OUTPUT);  //BLUE

  pinMode(motorPin1, OUTPUT); 
  pinMode(motorPin2, OUTPUT);

  digitalWrite(motorPin1, HIGH); 
  digitalWrite(motorPin2, HIGH); 

  WiFiDrv::digitalWrite(RGB_BLUE, HIGH);

  if (!ENV.begin()) {
    Serial.println("Failed to initialize MKR Shield!");
    WiFiDrv::digitalWrite(RGB_RED, HIGH);
    while (1);
  }
  connectToWiFi(); 
  Firebase.begin(host, auth, ssid, pass);
   
  delay(1000); 
  Serial.println(auth);
  Serial.println(host); 
  Serial.println(ssid); 
  Serial.println(pass); 


  Serial.println(" ");
  Serial.println("Greenhouse Initialized!");
  Serial.println(" ");

  ArduinoOTA.begin(WiFi.localIP(), "Arduino MKR WiFi 1010", "Pras92708", InternalStorage); //Starts OTA update
  rtc.setEpoch(WiFi.getTime() - 14400); //Converted Unix Timestamp (in GMT) to EST Time by subtracting 4 hours (14400)
  alarmInit(); //Initialize the alarms to pump the water
}


void loop() {
  ArduinoOTA.poll();

  if (millis()  - lastConnectionTime > postingInterval) {
    readSensors();
    writeData(); 
  }

  if (status != WL_CONNECTED) {
  }
  else {
    WiFiDrv::digitalWrite(RGB_BLUE, LOW);
    WiFiDrv::analogWrite(RGB_GREEN, 20);
  }

  if(millis()% 3600000 == 0) {
    waterLevelRead(); 
  }

  readCommands(); 

}
