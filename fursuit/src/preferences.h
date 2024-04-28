#define AP_NAME "computerelite-suit-setup"
#include <ArduinoJson.h>

extern void LoadPreferences();
extern void SavePreferences();
extern StaticJsonDocument<1024> GetConfig();
extern void ApplyConfig(StaticJsonDocument<1024> doc);

extern void  ResetPreferences(bool alsoWifi);