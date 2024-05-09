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

enum EarMode {
    COPY_TAIL,
    MIRROR_LEFT_EAR,
    MIRROR_RIGHT_EAR,
    COPY_ORG_LEFT_EAR,
    COPY_ORG_RIGHT_EAR,
};

extern EarMode leftEarMode;
extern EarMode rightEarMode;



enum AnimationType {
    PRIMARY,
    SECONDARY
};