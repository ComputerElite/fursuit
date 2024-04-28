#include "main.h"

extern bool applyBeatSignalOntoLEDs;
extern LEDAnimation primaryAnimation;
extern LEDAnimation secondaryAnimation;
extern bool secondaryAnimationEnabled;

enum AnimationType {
    PRIMARY,
    SECONDARY
};