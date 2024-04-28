#include "main.h"

extern bool applyBeatSignalOntoLEDs;
extern LEDAnimation primaryAnimation;
extern LEDAnimation secondaryAnimation;
extern bool secondaryAnimationEnabled;
extern bool statusLEDsEnabled;

enum AnimationType {
    PRIMARY,
    SECONDARY
};