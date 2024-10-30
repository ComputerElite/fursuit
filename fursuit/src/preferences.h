#define AP_NAME "computerelite-suit-setup"
#include <ArduinoJson.h>

extern void LoadPreferences();
extern void SavePreferences();
extern StaticJsonDocument<1024> GetConfig();
extern void ResetConfig();
extern void ApplyConfig(StaticJsonDocument<1024> doc);
extern void UpdateLEDBrightness(double b);
extern void SaveConfig();

extern void  ResetPreferences(bool alsoWifi);