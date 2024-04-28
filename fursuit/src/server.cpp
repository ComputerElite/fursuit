#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include "server.h"
#include "wifi.h"
#include "led.h"
#include "html.h"
#include "controls.h"

AsyncWebServer server(80);

void RestartServer() {
  server.end();
  server.begin();
}

char* extractString(uint8_t *data, size_t len) {
  char* str = (char*)data;
  str[len] = '\0';
  return str;
}

int extractNumber(uint8_t *data, size_t len) {
  return atoi(extractString(data, len));
}

void SetupServer() {
  
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });
  server.on("/ledpatterns", HTTP_GET, [](AsyncWebServerRequest *request){
    StaticJsonDocument<2048> doc;
    for (int i = LEDAnimation::ENUM_START + 1; i < LEDAnimation::ENUM_END; ++i)
    {
      JsonObject pattern = doc.createNestedObject();
      pattern["id"] = i;
      pattern["name"] = led_animation_names[i];
      pattern["group"] = led_animation_groups[i];
    }
    String output;
    serializeJson(doc, output);
    request->send(200, "application/json", output);
  });
  server.on("/config", HTTP_GET, [](AsyncWebServerRequest *request){
    StaticJsonDocument<256> doc;
    doc["primary"] = primaryAnimation;
    doc["secondary"] = secondaryAnimation;
    String output;
    serializeJson(doc, output);
    request->send(200, "application/json", output);
  });
  server.on("/config", HTTP_POST, [](AsyncWebServerRequest * request){},NULL,[](AsyncWebServerRequest * request, uint8_t *data, size_t len, size_t index, size_t total) {
    if(len < 1) return request->send(400);
    StaticJsonDocument<256> doc;
    deserializeJson(doc, data, len);
    primaryAnimation = (LEDAnimation)doc["primary"].as<int>();
    secondaryAnimation = (LEDAnimation)doc["secondary"].as<int>();
    request->send(200);
  });
  // get/set wifi
  server.on("/wifi", HTTP_GET, [](AsyncWebServerRequest *request){
    StaticJsonDocument<256> doc;
    doc["ssid"] = ssid;
    doc["password"] = password;
    doc["status"] = wifiStatus;
    String output;
    serializeJson(doc, output);
    request->send(200, "application/json", output);
  });
  server.on("/wifi", HTTP_POST, [](AsyncWebServerRequest * request){},NULL,[](AsyncWebServerRequest * request, uint8_t *data, size_t len, size_t index, size_t total) {
    if(len < 1) return request->send(400);
    StaticJsonDocument<256> doc;
    deserializeJson(doc, data, len);
    String newSSID = doc["ssid"].as<String>();
    String newPassword = doc["password"].as<String>();
    SetSSIDAndPassword(newSSID, newPassword);
    request->send(200);
  });
}