#include <Preferences.h>
#include "wifi.h"
#include "preferences.h"
#include "led.h"
#include <ArduinoJson.h>
#include "controls.h"
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

void SaveConfig(StaticJsonDocument<1024> doc) {
    String output;
    serializeJson(GetConfig(), output);
    prefs.putString("config", output);
}

void ApplyConfig(StaticJsonDocument<1024> doc) {
    primaryAnimation = (LEDAnimation)doc["primary"].as<int>();
    secondaryAnimation = (LEDAnimation)doc["secondary"].as<int>();
    secondaryAnimationEnabled = secondaryAnimation != LEDAnimation::OFF;
    applyBeatSignalOntoLEDs = doc["beatSignal"].as<bool>();
    statusLEDsEnabled = doc["statusLEDsEnabled"].as<bool>();
    msAfterWhichLEDsBrightenOnBeatMode = doc["msAfterWhichLEDsBrightenOnBeatMode"].as<long>();
    msLEDsTakeToBrightenOnBeatMode = doc["msLEDsTakeToBrightenOnBeatMode"].as<double>();
    statusLEDBrightness = doc["statusLEDBrightness"].as<double>();
    secondaryAnimationLightUpFraction = doc["secondaryAnimationLightUpFraction"].as<double>();
    SaveConfig(doc);
}

void LoadConfig() {
    StaticJsonDocument<1024> doc;
    deserializeJson(doc, prefs.getString("config", "{}"));
    ApplyConfig(doc);
}

StaticJsonDocument<1024> GetConfig() {
    StaticJsonDocument<1024> doc;
    doc["primary"] = primaryAnimation;
    doc["secondary"] = secondaryAnimation;
    doc["beatSignal"] = applyBeatSignalOntoLEDs;
    doc["statusLEDsEnabled"] = statusLEDsEnabled;
    doc["msAfterWhichLEDsBrightenOnBeatMode"] = msAfterWhichLEDsBrightenOnBeatMode;
    doc["msLEDsTakeToBrightenOnBeatMode"] = msLEDsTakeToBrightenOnBeatMode;
    doc["statusLEDBrightness"] = statusLEDBrightness;
    doc["secondaryAnimationLightUpFraction"] = secondaryAnimationLightUpFraction;

    return doc;
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
    LoadConfig();
}