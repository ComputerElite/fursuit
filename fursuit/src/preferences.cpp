#include <Preferences.h>
#include "wifi.h"
#include "preferences.h"
#include "led.h"
Preferences prefs;

const char* ns = "Tail";

void SavePreferences() {
    prefs.putString("ssid", ssid);
    prefs.putString("password", password);
}

void ResetPreferences(bool alsoWifi = false) {
    prefs.remove("animation");
    prefs.remove("ledAnimation");
    prefs.remove("speed");
    prefs.remove("ledSpeed");
    prefs.remove("brightness");
    prefs.remove("timeRunning");
    if(alsoWifi) {

        prefs.remove("ssid");
        prefs.remove("password");
    }
}

void SaveBrightness(int brightness) {
    prefs.putInt("brightness", brightness);
}
int LoadSavedBrightness() {
    return prefs.getInt("brightness", 255);
}

void LoadPreferences() {
    prefs.begin(ns);
    ssid = prefs.getString("ssid", AP_NAME);
    password = prefs.getString("password", "VerySecure");
}