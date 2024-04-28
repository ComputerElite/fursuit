#include <Arduino.h>

enum WifiStatus {
    WIFI_STARTING,
    WIFI_CONNECTING,
    WIFI_CONNECTED,
    WIFI_AP_OPEN,
    WIFI_CONNECTION_FAILED
};

extern WifiStatus wifiStatusEnum;

extern void BeginWifi();
extern void HandleWifi();
extern void SetSSIDAndPassword(String newSSID, String newPassword);

extern String ssid;
extern String password;
extern String wifiStatus;