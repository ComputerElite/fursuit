#include <WiFi.h>
#include "preferences.h"
#include "wifi.h"
#include "led.h"
#include "main.h"


String setupSSID = AP_NAME;
String setupPassword = "VerySecure";
const char *hostname = "computerelite-tail.local";
String ssid = AP_NAME;
String password = "VerySecure";
String wifiStatus = "Not connected";

String lastSSID = "";
String lastPassword = "";

WifiStatus wifiStatusEnum = WifiStatus::WIFI_STARTING;

bool firstConnect = true;

long disconnectedTime = 0;
bool softAPStarted = false;


bool isTryingToConnectToNewNetwork = false;
bool startedScanTask = false;

void BeginSetup();


void ConnectWifi() {
    softAPStarted = false;
    disconnectedTime = 0;
    if(ssid != "") {
        Serial.print("Connecting to ");
        Serial.println(ssid);
        Serial.print("With password ");
        Serial.println(password);
        WiFi.begin(ssid.c_str(), password == "" ? NULL : password.c_str()); // only connect if ssid is present. If password is empty, connect without password
    } else {
        BeginSetup();
    }
}

void SetSSIDAndPassword(String newSSID, String newPassword) {
    // Save last SSID to fall back to if connection fails
    // Assume wifi connection suceeded. If it didn't status will be updated
    wifiStatus = "WiFi connected!";
    lastSSID = ssid;
    lastPassword = password;
    ssid = newSSID;
    password = newPassword;
    isTryingToConnectToNewNetwork = true;
    SavePreferences();
    wifiStatusEnum = WifiStatus::WIFI_CONNECTING;
    ConnectWifi();
}

void BeginWifi() {
    isTryingToConnectToNewNetwork = true;
    WiFi.setHostname(hostname);
    WiFi.setAutoConnect(false);
    WiFi.mode(WIFI_STA);
    firstConnect = true;
    if(ssid == setupSSID) {
        // Open AP if ssid is ap ssid
        BeginSetup();
        return;
    }
    ConnectWifi();
}

int attempt = 0;
const int MAX_ATTEMPTS = 2;

void ConnectToLastNetworkIfApplicable() {
    if(isTryingToConnectToNewNetwork) {
        attempt++;
        if(attempt >= MAX_ATTEMPTS) {
            if(firstConnect) {
                firstConnect = false;
                BeginSetup();
                return;
            }
            isTryingToConnectToNewNetwork = false;
            wifiStatusEnum = WifiStatus::WIFI_CONNECTION_FAILED;            
            attempt = 0;
            ConnectToLastNetworkIfApplicable();
            return;
        }
        ConnectWifi();
        return;
    }
    
    // Fallback to old network
    if(lastSSID != "") {
        Serial.println("Trying to reconnect to last network");
        ssid = lastSSID;
        password = lastPassword;
        SavePreferences(); // make sure faulty wifi isn't saved
        // reset password
        lastSSID = "";
        lastPassword = "";
        attempt = 0;
        ConnectWifi();
    } else {
        attempt++;
        if(attempt >= MAX_ATTEMPTS) {
            attempt = 0;
            Serial.println("Max number of attempts reached. Starting SoftAP");
            BeginSetup();
            return;
        }
        Serial.println("Retrying connection");
        Serial.print("Attempt ");
        Serial.println(attempt);
        ConnectWifi();
    }
}


void HandleWifi() {
    if(WiFi.status() == WL_CONNECTED) {
        disconnectedTime = 0;
        firstConnect = false;
        if(isTryingToConnectToNewNetwork) {
            isTryingToConnectToNewNetwork = false;
            wifiStatus = "WiFi connected!";
            Serial.println("WiFi connected");
            Serial.println(WiFi.localIP());
            attempt = 0;
            wifiStatusEnum = WifiStatus::WIFI_CONNECTED;
        }
        return;
    }
    if(WiFi.status() == WL_CONNECT_FAILED) {
        disconnectedTime = 0;
        wifiStatus = "Connection failed. Check password and ssid";
        Serial.println("Connection failed");
        ConnectToLastNetworkIfApplicable();
        return;
    }
    if(WiFi.status() == WL_NO_SSID_AVAIL) {
        disconnectedTime = 0;
        wifiStatus = "SSID not found";
        Serial.println("No SSID available");
        ConnectToLastNetworkIfApplicable();
        return;
    }
    if(WiFi.status() == WL_CONNECTION_LOST) {
        disconnectedTime = 0;
        Serial.println("Connection lost, reconnecting");
        ConnectToLastNetworkIfApplicable();
        return;
    }
    if(WiFi.status() == WL_DISCONNECTED) {
        disconnectedTime += deltaTime;
        if(disconnectedTime > 20000 && !softAPStarted) {
            disconnectedTime = 0;
            wifiStatus = "Timeout";
            Serial.println("Disconnected timeout");
            ConnectToLastNetworkIfApplicable();
        }
        return;
    }
}

const unsigned long softAPScanInterval = 15000;

void wifiScanTask(void* pvParameters) {

  while (true) {
    if(softAPStarted) {
        // scan for wifi
        Serial.println("Scanning for saved SSID while in SoftAP...");
        int n = WiFi.scanNetworks();
        for (int i = 0; i < n; ++i) {
            if (WiFi.SSID(i) == ssid) {
                Serial.println("Saved SSID found, attempting connection...");
                ConnectToLastNetworkIfApplicable();
                return;
            }
        }
    }
    vTaskDelay(softAPScanInterval / portTICK_PERIOD_MS);  // Wait 5 seconds
  }
}

void BeginSetup() {
    wifiStatus = "Please connect to a wifi network to save power";
    WiFi.disconnect();
    Serial.println("SoftAP starting");
    WiFi.mode(WIFI_AP);
    WiFi.softAP(setupSSID, setupPassword);
    softAPStarted = true;
    wifiStatusEnum = WifiStatus::WIFI_AP_OPEN;

    if(!startedScanTask) {
        startedScanTask = true;
        xTaskCreatePinnedToCore(
            wifiScanTask,       // Function to run
            "wifiScanTask",     // Name
            8192,           // Stack size in bytes
            NULL,           // Parameters
            1,              // Priority
            NULL,           // Task handle
            0               // Core 0
        );
    }
}