/*
   This is AutoGrow! Have fun using it!
   Author: SpyGuy

   TODO: Add Failsafes (v2.0)
   TODO: Add feature for multiple water times (v2.0)
   TODO: Add logging to SD Card (v2.0)
   TODO: Make React Native App (v2.0)

   Upload Password: 123456

   Events at:
   8:00 PM
   6:30 AM
   7:00 AM
   Every 10 minutes (XX:10, XX:20, XX:30, XX:40, XX:50, XX:00)
   Every 30 minutes (XX:30, XX:00)
   Update time every 5 minutes
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
int lastPosted = 9999; 

WiFiClient client;
RTCZero rtc;

void setup() {
  rtc.begin();
  Serial.begin(115200); 

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
  Serial.print("Time: ");
  Serial.println(WiFi.getTime()); 
  lightInit();
  alarmInit(); //Initialize the alarms to pump the water
  readSensors(); 
  writeData(); 

  Serial.println("WDT Enabled"); 
  Watchdog.enable(10000); 
  
}


void loop() {
  Watchdog.reset(); 
  ArduinoOTA.poll();

  if (rtc.getMinutes() % 10 == 0 and (rtc.getMinutes() > lastPosted + 5 or lastPosted == 9999)) {
    Serial.println("Post"); //Post every 10 minutes
    readSensors();
    writeData();
    lastPosted = rtc.getMinutes(); 
  }
  
  if(rtc.getMinutes() % 5 == 0) {
    rtc.setEpoch(WiFi.getTime() - 14400); 
  }

  if (status != WL_CONNECTED) {
    Serial.println("Maybe"); 
  }
  else {
    WiFiDrv::digitalWrite(RGB_BLUE, LOW);
    WiFiDrv::analogWrite(RGB_GREEN, 20);
  }

  if (rtc.getMinutes() == 0) {
    Serial.println("Water");  //Read water level every hour
    waterLevelRead();
  }

  if ((rtc.getHours() >= 20 or rtc.getHours() < 7) and lightOn == false) {
    Serial.println("DB2"); //Turn on lights at 8:00 PM
    turnOnLights();
    readSensors(); 
    writeData(); 
  }

  if ((rtc.getHours() >= 7 and rtc.getHours() < 20) and lightOn == true) {
    Serial.println("DB1"); //Turn off lights at 7:00 AM
    turnOffLights();
    readSensors(); 
    writeData(); 
  }

  if(rtc.getHours() == 6 and rtc.getMinutes() == 30 and waterOn == false) {
    Serial.println("Water");  //Pump water at 6:30 AM
    pumpWater(); 
  }

  readCommands(); //Read Serial commands (Delete if remote)


}
