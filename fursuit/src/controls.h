#include "main.h"

extern bool applyBeatSignalOntoLEDs;
extern LEDAnimation primaryAnimation;
extern LEDAnimation secondaryAnimation;
extern bool secondaryAnimationEnabled;
extern bool statusLEDsEnabled;
extern long msAfterWhichLEDsBrightenOnBeatMode;
extern double msLEDsTakeToBrightenOnBeatMode;
extern double statusLEDBrightness;
extern double secondaryAnimationLightUpFraction;
extern double ledBrightness;
extern int color0Int;
extern CRGB color0;
extern int color1Int;
extern CRGB color1;

enum AnimationType {
    PRIMARY,
    SECONDARY
};