#include "led.h"
#include "main.h"
#include "preferences.h"
#include "imu.h"
#include "controls.h"
#include "wifi.h"

double hue = 0;
long animationSetTime = 0;
double secondsSinceAnimationStart = 0;
double animationSpeed = 7;
uint8_t brightnessValueInternal = 255;

#define X(a, name, group) name,
char const *led_animation_names[] =
{
    LED_ANIMATIONS
};
#undef X

#define X(a, name, group) group,
int const led_animation_groups[] =
{
    LED_ANIMATIONS
};
#undef X

CRGB combinedLeds[N_LEDS];
CRGB combinedLedsShown[N_LEDS];

void SetupLED() {
    
}

int GetBrightness() {
  return brightnessValueInternal * MAX_BRIGHTNESS / 255;
}

// animation variabls
CRGB currentColor = 0;
CRGB color0 = 0;
CRGB color1 = 0;
double currentBreathsPerSecond = 0;
double breathsPerSecond0 = 0;
double breathsPerSecond1 = 0;
double breathsLerpFactor = 1;

double breathSecondCounter = 0;

CRGB GetColorBrightness(CRGB color, uint8_t brightness) {
  //return color;

  // Scale each 8 bytes
  color.r = color.r * brightness / 255;
  color.g = color.g * brightness / 255;
  color.b = color.b * brightness / 255;
  return color;
}

CRGB LerpColor(CRGB color1, CRGB color2, double percentage) {
  if(percentage > 1) percentage = 1;
  return CRGB(color1.r + (color2.r - color1.r) * percentage,
              color1.g + (color2.g - color1.g) * percentage, 
              color1.b + (color2.b - color1.b) * percentage);
}

double Lerp(double a, double b, double percentage) {
  if(percentage > 1) percentage = 1;
  return a + (b - a) * percentage;
}

double GetStepForTime() {
  return deltaTime / 1000.0 * animationSpeed;
}

void IncrementHue() {
  hue += GetStepForTime();
  if(hue >= 255) hue -= 255;
}

void SetPixelColor(int pixel, CRGB color) {
  color = GetColorBrightness(color, GetBrightness());
  combinedLeds[pixel] = color;
}
void SetPixelColor(int pixel, CRGB color, uint8_t brightness) {
  // sets a pixels color
  color = GetColorBrightness(color, brightness);
  color = GetColorBrightness(color, GetBrightness());
  combinedLeds[pixel] = color;
}

void SetAllPixelsNonShow(CRGB color) {
  for(int i=0; i<N_LEDS; i++) { 
    SetPixelColor(i, color);
  }
}

void SetAllPixelsNonShow(CRGB color, uint8_t brightness) {
  for(int i=0; i<N_LEDS; i++) { 
    SetPixelColor(i, GetColorBrightness(color, brightness));
  }
}

void SetColor(CRGB color) {
  SetAllPixelsNonShow(color);
}

void Breathe(CRGB color) {
  breathSecondCounter += deltaTimeSeconds * currentBreathsPerSecond;
  double brightness = sin(breathSecondCounter * 2*PI) * 0.5 + 0.5;
  if(brightness < 0) brightness = 0;
  if(brightness > 1) brightness = 1;


  SetAllPixelsNonShow(color, static_cast<uint8_t>(brightness * 255.0));
}



void LerpColor0ToColor1() {
  currentColor = LerpColor(color0, color1, secondsSinceAnimationStart);
}

void LerpBreathsPerSecond0ToBreathsPerSecond1() {
  currentBreathsPerSecond = Lerp(breathsPerSecond0, breathsPerSecond1, secondsSinceAnimationStart * breathsLerpFactor);
}

long beatLEDTriggerTime = 0;
double beatLEDTriggerLength = 0;
uint8_t beatLEDBrightness = 0;
void UpdateStatusLEDs() {
  uint8_t defaultStatusLedBrightness = static_cast<uint8_t>(statusLEDBrightness * 255);
  if(applyBeatSignalOntoLEDs) {
    if(beatSignal) {
      beatLEDTriggerTime = millis();
      beatLEDTriggerLength = isStrongBeat ? 100 : 10;
    }
    beatLEDBrightness = millis() > beatLEDTriggerTime + beatLEDTriggerLength ? 0 : 255;
    SetPixelColor(TAIL_N_LEDS + STATUS_LED_HEAD_START_INDEX, CRGB(255, 255, 255), static_cast<uint8_t>(statusLEDBrightness * beatLEDBrightness));
  } else {
    SetPixelColor(TAIL_N_LEDS + STATUS_LED_HEAD_START_INDEX, CRGB(255, 0, 0), defaultStatusLedBrightness);
  }
  
  CRGB wifiColor = CRGB(0, 0, 0);
  uint8_t wifiBrightness = static_cast<uint8_t>(statusLEDBrightness * 255);
  switch(wifiStatusEnum) {
    case WifiStatus::WIFI_CONNECTED:
      wifiColor = CRGB(0, 255, 0);

      break;
    case WifiStatus::WIFI_CONNECTING:
      wifiColor = CRGB(0, 229, 255);
      wifiBrightness = static_cast<uint8_t>((lastLoop / 10 % 255) * statusLEDBrightness);
      break;
    case WifiStatus::WIFI_AP_OPEN:
      wifiColor = CRGB(255, 255, 0);
      break;
    case WifiStatus::WIFI_CONNECTION_FAILED:
      wifiColor = CRGB(255, 0, 0);
      wifiBrightness = static_cast<uint8_t>((lastLoop / 6 % 255) * statusLEDBrightness);
      break;
  }
  SetPixelColor(TAIL_N_LEDS + STATUS_LED_HEAD_START_INDEX +1, wifiColor, wifiBrightness);
  SetPixelColor(TAIL_N_LEDS + STATUS_LED_HEAD_START_INDEX +2, combinedLeds[0], defaultStatusLedBrightness);
  SetPixelColor(TAIL_N_LEDS + STATUS_LED_HEAD_START_INDEX +3, combinedLeds[1], defaultStatusLedBrightness);
  SetPixelColor(TAIL_N_LEDS + STATUS_LED_HEAD_START_INDEX +4, combinedLeds[2], defaultStatusLedBrightness);
  SetPixelColor(TAIL_N_LEDS + STATUS_LED_HEAD_START_INDEX +5, combinedLeds[3], defaultStatusLedBrightness);
}

double brightnessMovementFlashesPrimary = 1;
double brightnessMovementFlashesPrimaryFrameBrightness = 1;

double GetBeatBrightnessMultiplier(AnimationType type, int ledIndex) {
  if(!applyBeatSignalOntoLEDs) return 1.0;
  switch(type) {
    case PRIMARY:
      return brightnessMovementFlashesPrimaryFrameBrightness;
      break;
    case SECONDARY:
      break;
  }
  return 1.0;
}
uint8_t GetBeatBrightnessMultiplierUInt8(AnimationType type, int ledIndex) {
  return static_cast<uint8_t>(GetBeatBrightnessMultiplier(type, ledIndex) * 255);
}

double GetHueBasedOnAnimationType(AnimationType type) {
  switch(type) {
    case PRIMARY:
      return hue;
      break;
    case SECONDARY:
      return hue + 127.0;
      break;
  }
  return hue;
}


double movementDeltaTime = 0.0;
const double movementFlashesStepTime = 0.015;
CRGB movementFlashedLEDStatus[N_LEDS] = {CRGB(0, 0, 0)};
void SetPixelColorWithType(int ledIndex, CRGB desiredColor, double desiredBrightness, AnimationType type) {
  if(desiredBrightness > 1) desiredBrightness = 1;
  if(desiredBrightness < 0) desiredBrightness = 0;
  switch(type) {
    case PRIMARY:
      // Primary is just pulsing. The brightness multiplier is important for controling everything.
      SetPixelColor(ledIndex, desiredColor, static_cast<uint8_t>(desiredBrightness * GetBeatBrightnessMultiplierUInt8(type, ledIndex)));
      return;
    case SECONDARY:
      if(!secondaryAnimationEnabled) return;
      if(ledIndex != 0) return;
      movementDeltaTime += deltaTimeSeconds;
      while(movementDeltaTime >= movementFlashesStepTime) {
        movementDeltaTime -= movementFlashesStepTime;
        for(int i=N_LEDS-1; i>=0; i--) {
          if(i > 0) {
            movementFlashedLEDStatus[i] = movementFlashedLEDStatus[i-1];
          } else {
            // they should light up for 30% of the beat length
            movementFlashedLEDStatus[i] = !isStrongBeat && beatSignalTime + secondaryAnimationLightUpFraction * 1000 / bps >= millis() ? desiredColor : CRGB(0, 0, 0);
          }
          if(movementFlashedLEDStatus[i].r + movementFlashedLEDStatus[i].g + movementFlashedLEDStatus[i].b > 10) {
            SetPixelColor(i, movementFlashedLEDStatus[i]);
          }
        }
      }
      return;
  }
}

void CorrectHead() {
  // Copy tail
  for(int i = 0; i < TAIL_N_LEDS; i++) {
    combinedLedsShown[i] = combinedLeds[i];
  }
  // Mirror tail to ears
  for(int i=0; i<HEAD_N_LEDS; i++) {
    combinedLedsShown[TAIL_N_LEDS +i] = combinedLeds[i];
  }
  // Mirror left ear to right ear

  for(int i=0; i<EAR_N_LEDS; i++) {
    combinedLedsShown[TAIL_N_LEDS + EAR_N_LEDS + i] = combinedLedsShown[TAIL_N_LEDS +i];
  }

  // Preview on staus leds
  for(int i=0; i<STATUS_LED_HEAD_N; i++) {
    combinedLedsShown[TAIL_N_LEDS + STATUS_LED_HEAD_START_INDEX + i] = combinedLeds[TAIL_N_LEDS + STATUS_LED_HEAD_START_INDEX + i];
  }
}

void AnimationRainbowStatic(AnimationType type) {
  for(int i=0; i<N_LEDS; i++) {
    SetPixelColorWithType(i, CHSV(static_cast<uint8_t>(GetHueBasedOnAnimationType(type)), 255, 255), 1.0, type);
  }
}

void AnimationRainbowFade(AnimationType type) {
  double perPixel = 255.0 / N_LEDS;
  for(int i=0; i<N_LEDS; i++) { 
    long pixelHue = static_cast<long>(GetHueBasedOnAnimationType(type)+ (i * perPixel)) % 255;
    SetPixelColorWithType(i, CHSV(static_cast<uint8_t>(pixelHue), 255, 255), 1.0, type);
  }
}

void AnimationStatic(AnimationType type) {
  for(int i=0; i<N_LEDS; i++) { 
    SetPixelColorWithType(i, CRGB(255, 0, 0), 1.0, type);
  }
}


void PrecomputeBrightnessMultiplier() {
  // Precompute brightness for primary animation
  // light up on beats and slowly fade out
  // If no movement for 5 seconds, start brightening leds again

  if(beatSignal) brightnessMovementFlashesPrimary = 1;
  brightnessMovementFlashesPrimary -= deltaTimeSeconds * bps;
  if(brightnessMovementFlashesPrimary < 0) brightnessMovementFlashesPrimary = 0;
  double startValue = isStrongBeat ? 1.1 : 0.5;
  double multiplyValue = isStrongBeat ? 1.05 : 0.45;
  if(secondaryAnimationEnabled && !isStrongBeat) {
    startValue = 0;
    multiplyValue = 0.0;
  }
  double expectedValue = startValue - sqrt(1 - brightnessMovementFlashesPrimary) * multiplyValue;
  if(expectedValue > 1) expectedValue = 1;
  brightnessMovementFlashesPrimaryFrameBrightness = expectedValue;
  if(lastLoop - beatSignalTime > msAfterWhichLEDsBrightenOnBeatMode) {
    
    // after 5 seconds start brightening leds again
    brightnessMovementFlashesPrimaryFrameBrightness = expectedValue + (lastLoop - beatSignalTime - msAfterWhichLEDsBrightenOnBeatMode) / msLEDsTakeToBrightenOnBeatMode;
    if(brightnessMovementFlashesPrimaryFrameBrightness > 1) brightnessMovementFlashesPrimaryFrameBrightness = 1;
  }
}

void ApplyAnimation(AnimationType type, LEDAnimation animation) {
  switch (animation)
  {
  case RAINBOW_STATIC:
    AnimationRainbowStatic(type);
    break;
  case RAINBOW_FADE:
    AnimationRainbowFade(type);
    break;
  case STATIC:
    AnimationStatic(type);
    break;
  default:
    break;
  }
}

void ApplyPrimaryAnimation() {
  ApplyAnimation(PRIMARY, primaryAnimation);
}

void ApplySecondaryAnimation() {
  ApplyAnimation(SECONDARY, secondaryAnimation);
}


void UpdateLED() {
  SetAllPixelsNonShow(CRGB(0, 0, 0)); // reset strip
  IncrementHue(); // Increment hue each frame
  PrecomputeBrightnessMultiplier();
  ApplyPrimaryAnimation();
  if(secondaryAnimationEnabled) ApplySecondaryAnimation();
  if(statusLEDsEnabled) UpdateStatusLEDs();
  CorrectHead();
  FastLED.show();
  //RainbowFade();
  return;
}