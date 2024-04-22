#include <Arduino.h>
#include "imu.h"
#include "led.h"

double deltaTimeSeconds = 0;
long deltaTime = 0;
long lastLoop = 0;

// put function declarations here:
int myFunction(int, int);

void setup() {
  // put your setup code here, to run once:
  InitIMU();
  FastLED.addLeds<WS2812B, TAIL_LED_PIN>(tailLeds, getRGBWsize(TAIL_N_LEDS));
  FastLED.addLeds<WS2812B, HEAD_LED_PIN>(headLeds, getRGBWsize(HEAD_N_LEDS));
}

void loop() {
  deltaTime = millis() - lastLoop;
  deltaTimeSeconds = static_cast<double>(millis() - lastLoop) / 1000.0;
  lastLoop = millis();
  // put your main code here, to run repeatedly:
  UpdateIMU();
  UpdateLED();
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}