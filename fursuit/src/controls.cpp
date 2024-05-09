#include "led.h"
#include "controls.h"
bool applyBeatSignalOntoLEDs = true;
LEDAnimation primaryAnimation = RAINBOW_FADE;
LEDAnimation secondaryAnimation = RAINBOW_STATIC;
bool secondaryAnimationEnabled = true;
bool statusLEDsEnabled = true;
long msAfterWhichLEDsBrightenOnBeatMode = 5000;
double msLEDsTakeToBrightenOnBeatMode = 4000.0;
double statusLEDBrightness = 1.0;
double secondaryAnimationLightUpFraction = 0.3;
double ledBrightness = 1.0;
int color0Int = 0xFF0000;
CRGB color0 = CRGB::Red;
int color1Int = 0x00FF00;
CRGB color1 = CRGB::Green;

EarMode leftEarMode = EarMode::COPY_TAIL;
EarMode rightEarMode = EarMode::MIRROR_LEFT_EAR;