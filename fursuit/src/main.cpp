#include <Arduino.h>
#include "imu.h"
#include "led.h"
#include "wifi.h"
#include "server.h"
#include "preferences.h"
#include "controls.h"
#include "weather.hpp"

double deltaTimeSeconds = 0;
long deltaTime = 0;
long lastLoop = 0;
bool serialOn = false;

/*
#include <Wire.h>
 

void setup()
{
  Wire.begin();
 
  Serial.begin(9600);
  while (!Serial);             // Leonardo: wait for serial monitor
  Serial.println("\nI2C Scanner");
}
 
 
void loop()
{
  byte error, address;
  int nDevices;
 
  Serial.println("Scanning...");
 
  nDevices = 0;
  for(address = 1; address < 127; address++ )
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
 
    if (error == 0)
    {
      Serial.print("I2C device found at address 0x");
      if (address<16)
        Serial.print("0");
      Serial.print(address,HEX);
      Serial.println("  !");
 
      nDevices++;
    }
    else if (error==4)
    {
      Serial.print("Unknown error at address 0x");
      if (address<16)
        Serial.print("0");
      Serial.println(address,HEX);
    }    
  }
  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("done\n");
 
  delay(5000);           // wait 5 seconds for next scan
}
*/
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); // initialize Serial communication
  FastLED.addLeds<NEOPIXEL, TAIL_LED_PIN>(combinedLedsShown, TAIL_N_LEDS);
  FastLED.addLeds<NEOPIXEL, HEAD_LED_PIN>(combinedLedsShown, TAIL_N_LEDS, HEAD_N_LEDS);
  //Scanner();
  
  LoadPreferences();
  InitIMU();
  BeginWifi();
  SetupWeather();
  SetupServer();
  RestartServer();
  
}

void loop() {
  deltaTime = millis() - lastLoop;
  deltaTimeSeconds = static_cast<double>(millis() - lastLoop) / 1000.0;
  lastLoop = millis();
  if(serialOn) {
    Serial.print("t");
    Serial.println(lastLoop);
    Serial.print("u");
    Serial.println(deltaTimeSeconds);
  }
  // put your main code here, to run repeatedly:
  UpdateIMU();
  if(wsEnabled) sendMessageToAllWSClients(concatinated);
  UpdateLED();
  HandleWifi();
  UpdateWeather();
}