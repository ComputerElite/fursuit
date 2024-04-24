#include <Arduino.h>
#include "imu.h"
#include "led.h"

double deltaTimeSeconds = 0;
long deltaTime = 0;
long lastLoop = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); // initialize Serial communication
  //Scanner();

  InitIMU();
  FastLED.addLeds<WS2812B, TAIL_LED_PIN>(combinedLeds, TAIL_N_LEDS);
  FastLED.addLeds<WS2812B, HEAD_LED_PIN>(combinedLeds, TAIL_N_LEDS, HEAD_N_LEDS);
}

void loop() {
  deltaTime = millis() - lastLoop;
  deltaTimeSeconds = static_cast<double>(millis() - lastLoop) / 1000.0;
  lastLoop = millis();
  Serial.print("time per update (s) ");
  Serial.println(deltaTimeSeconds);
  // put your main code here, to run repeatedly:
  UpdateIMU();
  UpdateLED();
}