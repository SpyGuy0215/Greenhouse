#include <ArduinoJson.h>
#include <SPI.h>
#include <Arduino_MKRENV.h>
#include <WiFiNINA.h>
#include <ArduinoOTA.h>
#include <utility/wifi_drv.h>

//Read MKR Sensors 
void readSensors() {
  temperatureVal = ENV.readTemperature(FAHRENHEIT);
  humidityVal = ENV.readHumidity();
  pressureVal = ENV.readPressure();
  luxVal = ENV.readIlluminance();
}

//Display MKR Sensor Values on Serial Port
void displayValuesOnSerial() {
  Serial.println();

  Serial.print("Temperature = ");
  Serial.print(temperatureVal);
  Serial.println(" °F");

  Serial.print("Humidity = ");
  Serial.print(humidityVal);
  Serial.println(" %");

  Serial.print("Pressure = ");
  Serial.print(pressureVal);
  Serial.println(" kPa");

  Serial.print("Lux = ");
  Serial.println(luxVal);

  Serial.println();

}
