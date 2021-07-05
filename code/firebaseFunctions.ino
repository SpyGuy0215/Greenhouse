#include "Firebase_Arduino_WiFiNINA.h"

#define FIREBASE_HOST "https://greenhouse-367c4-default-rtdb.firebaseio.com/";
#define FIREBASE_AUTH "QM6NyUt9EMJj2p9qQB6IVnUUtnsW0f1tGMoSHjZb";

FirebaseData firebaseData; 

void writeData() {
  String writeDate = String(rtc.getMonth()) + "/" + String(rtc.getDay()) + "/" + String(rtc.getYear()); 
  String writeTime = String(rtc.getHours() % 12) + ":" + String(rtc.getMinutes()) + ":" + String(rtc.getSeconds()); 
    
  Firebase.setInt(firebaseData, "/temperature", temperatureVal);
  Firebase.setInt(firebaseData, "/humidity", humidityVal);
  Firebase.setInt(firebaseData, "/pressure", pressureVal);
  Firebase.setInt(firebaseData, "/brightness", luxVal);
  Firebase.setBool(firebaseData, "/waterIn", waterIn);
  Firebase.setString(firebaseData, "/lastPosted/date", writeDate); 
  Firebase.setString(firebaseData, "/lastPosted/time", writeTime); 

  lastConnectionTime = millis(); 
}

void readLastWatered(){
  if(lastWatered == 0) {
        
  }
}

void testPoll() {
  bool lightBool;
  bool waterBool; 
  if(Firebase.getBool(firebaseData, "/testing/light")) {
    lightBool = firebaseData.boolData(); 
    if(lightBool == true) {
      turnOnLights(); 
    }
    else {
      turnOffLights(); 
    }
  }
  if(Firebase.getBool(firebaseData, "/testing/water")) {
    waterBool = firebaseData.boolData(); 
    if(waterBool == true) {
      pumpWater(); 
    }
  }
}

void connectToWiFi() {
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < "1.0.0") {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    ;
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);
    WiFiDrv::digitalWrite(RGB_BLUE, LOW); 

    // wait 10 seconds for connection:
    for (int i = 0; i < 5; i++) {
      WiFiDrv::digitalWrite(RGB_GREEN, HIGH);
      WiFiDrv::digitalWrite(RGB_RED, HIGH);
      delay(1000);
      WiFiDrv::digitalWrite(RGB_GREEN, LOW);
      WiFiDrv::digitalWrite(RGB_RED, LOW);
      delay(1000);
    }
  }

  Serial.println("Connected to WiFi");
  delay(2000);
}
