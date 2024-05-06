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

enum AnimationType {
    PRIMARY,
    SECONDARY
};