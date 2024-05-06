#include "led.h"
bool applyBeatSignalOntoLEDs = true;
LEDAnimation primaryAnimation = RAINBOW_FADE;
LEDAnimation secondaryAnimation = RAINBOW_STATIC;
bool secondaryAnimationEnabled = true;
bool statusLEDsEnabled = true;
long msAfterWhichLEDsBrightenOnBeatMode = 5000;
double msLEDsTakeToBrightenOnBeatMode = 4000.0;
double statusLEDBrightness = 1.0;
double secondaryAnimationLightUpFraction = 0.3;
