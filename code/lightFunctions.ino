#include <SPI.h>
#include <WiFiNINA.h>
#include <Adafruit_NeoPixel.h>
#include <ArduinoOTA.h>
#include <RTCZero.h>  
#include <utility/wifi_drv.h>

const int ledPin = 3; 
const int ledCount = 30; 

Adafruit_NeoPixel strip(ledCount, ledPin, NEO_GRB + NEO_KHZ800);

void lightInit() {
  strip.begin(); 
  strip.show(); //Turns off all pixels
}

void turnOnLights() {
      for(int i = 0; i < 31; i += 3) {
        strip.setPixelColor(i, 255, 0, 0); 
      }
      for(int i = 1; i < 31; i += 3) {
        strip.setPixelColor(i, 255, 0, 0); 
      }
      for(int i = 2; i < 31; i += 3) {
        strip.setPixelColor(i, 0, 0, 255); 
      }
       strip.show(); 
       lightOn = true; 
}

void turnOffLights() {
  strip.clear(); 
  strip.show();
  lightOn = false; 
}
