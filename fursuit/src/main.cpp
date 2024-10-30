#include <Arduino.h>
#include "imu.h"
#include "led.h"
#include "wifi.h"
#include "server.h"
#include "screen.h"
#include "preferences.h"

double deltaTimeSeconds = 0;
long deltaTime = 0;
long lastLoop = 0;
bool serialOn = false;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); // initialize Serial communication
  FastLED.addLeds<NEOPIXEL, TAIL_LED_PIN>(combinedLedsShown, TAIL_N_LEDS);
  //FastLED.addLeds<NEOPIXEL, TAIL_LED_PIN>(combinedLedsShown, TAIL_N_LEDS, HEAD_N_LEDS);
  //Scanner();
  LoadPreferences();
  //InitIMU();
  BeginWifi();
  SetupServer();
  RestartServer();
  SetupScreen();
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
  //UpdateIMU();
  UpdateLED();
  //HandleWifi();
  UpdateScreen();
  delay(20);
}