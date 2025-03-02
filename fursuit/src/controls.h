#include "main.h"

extern bool applyBeatSignalOntoLEDs;
extern LEDAnimation primaryAnimation;
extern LEDAnimation secondaryAnimation;
extern bool secondaryAnimationEnabled;
extern bool statusLEDsEnabled;
extern bool wsEnabled;
extern long msAfterWhichLEDsBrightenOnBeatMode;
extern double msLEDsTakeToBrightenOnBeatMode;
extern double statusLEDBrightness;
extern double secondaryAnimationLightUpFraction;
extern double ledBrightness;
extern int color0Int;
extern CRGB color0;
extern int color1Int;
extern CRGB color1;

#define EAR_MODE \
X(EAR_MODE_ENUM_START, "Enum start", -1) \
X(COPY_TAIL, "Copy tail", 1) \
X(MIRROR_LEFT_EAR, "Mirror left ear", 1) \
X(MIRROR_RIGHT_EAR, "Mirror right ear", 1) \
X(COPY_ORG_LEFT_EAR, "Copy org left ear", 1) \
X(COPY_ORG_RIGHT_EAR, "Copy org right ear", 1) \
X(EAR_MODE_ENUM_END, "Enum end", -1)

#define X(a, name, group) a,
enum EarMode {
    EAR_MODE
};
#undef X

extern char const *ear_mode_names[];

extern int const ear_mode_groups[];

extern EarMode leftEarMode;
extern EarMode rightEarMode;



enum AnimationType {
    PRIMARY,
    SECONDARY
};