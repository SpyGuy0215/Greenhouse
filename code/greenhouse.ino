/*
   This is AutoGreen! Have fun using it!
   Author: SpyGuy

   TODO: Add Failsafes (v2.0)
   TODO: Add feature for multiple water times (v2.0)
   TODO: Add logging to SD Card (v2.0)
   TODO: Make React Native App (v2.0)

   Upload Password: 123456
*/

#include <ArduinoJson.h>
#include <SPI.h>
#include <Arduino_MKRENV.h>
#include "Firebase_Arduino_WiFiNINA.h"
#include <WiFiNINA.h>
#include <ArduinoOTA.h>
#include <RTCZero.h>
#include <Adafruit_SleepyDog.h>
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
bool waterOn = false; 
int lastWatered;

bool lightOn = false;

int motorPin1 = 4;
int motorPin2 = 5;

unsigned long lastConnectionTime = 0;
const unsigned long postingInterval = 600000; //10 Minutes

WiFiClient client;
RTCZero rtc;

void setup() {
  rtc.begin();
  Serial.begin(9600); 

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


  ArduinoOTA.begin(WiFi.localIP(), "Arduino MKR WiFi 1010", "123456", InternalStorage); //Starts OTA update
  rtc.setEpoch(WiFi.getTime() - 14400); //Converted Unix Timestamp (in GMT) to EST Time by subtracting 4 hours (14400)
  lightInit();
  alarmInit(); //Initialize the alarms to pump the water
  readSensors(); 
  writeData(); 

  Serial.println("WDT Enabled"); 
  Watchdog.enable(5000); 
  
}


void loop() {
  Watchdog.reset(); 
  ArduinoOTA.poll();

  if (millis()  - lastConnectionTime > postingInterval) {
    Serial.println("Post"); 
    readSensors();
    writeData();
  }

  if (status != WL_CONNECTED) {
    Serial.println("Maybe"); 
  }
  else {
    WiFiDrv::digitalWrite(RGB_BLUE, LOW);
    WiFiDrv::analogWrite(RGB_GREEN, 20);
  }

  if (millis() % 3600000 == 0) {
    Serial.println("Umbruh"); 
    waterLevelRead();
  }

  if (rtc.getHours() == 20 and lightOn == false) {
    Serial.println("DB2"); 
    turnOnLights();
    readSensors(); 
    writeData(); 
  }

  if (rtc.getHours() == 7 and lightOn == true) {
    Serial.println("DB1"); 
    turnOffLights();
    readSensors(); 
    writeData(); 
  }

  if(rtc.getHours() == 18 and rtc.getMinutes() == 6 and waterOn == false) {
    Serial.println("Water"); 
    pumpWater(); 
  }


  readCommands();
  testPoll(); 

}
