#include <FastLED.h>
#define TAIL_LED_PIN 16
#define TAIL_N_LEDS 47
#define HEAD_LED_PIN 17
#define EAR_N_LEDS 44
#define STATUS_LED_HEAD_N 6
#define HEAD_N_LEDS (EAR_N_LEDS + EAR_N_LEDS + STATUS_LED_HEAD_N)
#define STATUS_LED_HEAD_START_INDEX 88
#define STATUS_LED_MAX_BRIGHTNESS 0.1
#define N_LEDS (TAIL_N_LEDS + HEAD_N_LEDS)

#define MAX_BRIGHTNESS 255

extern CRGB combinedLeds[N_LEDS];
extern CRGB combinedLedsShown[N_LEDS];

#define LED_ANIMATIONS \
X(ENUM_START, "Enum start", -1) \
X(RAINBOW_STATIC, "Rainbow Static", 1) \
X(RAINBOW_FADE, "Rainbow Fade", 1) \
X(STATIC, "Static red", 2) \
X(OFF, "Off", 1) \
X(ENUM_END, "Enum end", -1)

#define X(a, name, group) a,
enum LEDAnimation
{
    LED_ANIMATIONS
};
#undef X

extern char const *led_animation_names[];

extern int const led_animation_groups[];

extern CRGB color0;


extern void SetLED(LEDAnimation animation);
extern LEDAnimation GetLED();
extern uint8_t brightnessValueInternal;
extern void SetLEDSpeed(double speed);
extern double GetLEDSpeed();
extern void UpdateLED();
extern void SetupLED();
extern void SetBrightness(int brightness);
extern int GetBrightness();