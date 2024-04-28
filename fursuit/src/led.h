#include <FastLED.h>
#define TAIL_LED_PIN 16
#define TAIL_N_LEDS 200
#define HEAD_LED_PIN 17
#define HEAD_N_LEDS 43
#define STATUS_LED_START_INDEX 0
#define STATUS_LED_MAX_BRIGHTNESS 1.094
#define N_LEDS (TAIL_N_LEDS + HEAD_N_LEDS)

#define MAX_BRIGHTNESS 255

extern CRGB combinedLeds[N_LEDS];

#define LED_ANIMATIONS \
X(ENUM_START, "Enum start", -1) \
X(RAINBOW_FADE, "Rainbow Fade", 1) \
X(RAINBOW_FRONT_BACK, "Rainbow Front to Back", 1) \
X(WIFI_SOFT_AP_OPEN, "Wifi Soft AP Open", 0) \
X(WIFI_CONNECTING, "Wifi Connecting", 0) \
X(WIFI_CONNECTION_FAILED, "Wifi Connection Failed", 0) \
X(WIFI_CONNECTED, "Wifi Connected", 0) \
X(OFF, "Off", 1) \
X(CHRISTMAS_1, "Candy cane",1) \
X(MOVING_LIGHT, "Moving light",1) \
X(STATIC_LIGHT, "Static",1) \
X(BREATHE_SLOW, "Breathe slow",1) \
X(BREATHE_MID, "Breathe mid",1) \
X(BREATHE_FAST, "Breathe fast",1) \
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