#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include "server.h"
#include "wifi.h"
#include "led.h"
#include "html.h"
#include "preferences.h"
#include "controls.h"
#include "weather.hpp"

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

void sendMessageToAllWSClients(char* msg)
{
  ws.textAll(msg);
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len)
{
  AwsFrameInfo *info = (AwsFrameInfo *)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT)
  {
    data[len] = 0;
    if (strcmp((char *)data, "toggle") == 0)
    {
      // this is just example code I stole leeel
    }
  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len)
{
  switch (type)
  {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
      break;
    case WS_EVT_ERROR:
      break;
  }
}

void initWebSocket()
{
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

void RestartServer()
{
  server.end();
  server.begin();
}

char *extractString(uint8_t *data, size_t len)
{
  char *str = (char *)data;
  str[len] = '\0';
  return str;
}

int extractNumber(uint8_t *data, size_t len)
{
  return atoi(extractString(data, len));
}

void SetupServer()
{
  initWebSocket();
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send_P(200, "text/html", index_html); });
  server.on("/wheel.js", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send_P(200, "text/html", wheelJs); });
  server.on("/wheel.css", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send_P(200, "text/style", wheelCss); });
  server.on("/ledpatterns", HTTP_GET, [](AsyncWebServerRequest *request)
            {
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
    request->send(200, "application/json", output); });
  server.on("/earmodes", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    StaticJsonDocument<2048> doc;
    for (int i = EarMode::EAR_MODE_ENUM_START + 1; i < EarMode::EAR_MODE_ENUM_END; ++i)
    {
      JsonObject pattern = doc.createNestedObject();
      pattern["id"] = i;
      pattern["name"] = ear_mode_names[i];
      pattern["group"] = ear_mode_groups[i];
    }
    String output;
    serializeJson(doc, output);
    request->send(200, "application/json", output); });
  server.on("/config", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    String output;
    serializeJson(GetConfig(), output);
    request->send(200, "application/json", output); });
  server.on("/reset", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    ResetConfig();
    request->send(200, "application/json", "Reset successful"); });
  server.on("/config", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
            {
    if(len < 1) return request->send(400);
    StaticJsonDocument<1024> doc;
    deserializeJson(doc, data, len);
    ApplyConfig(doc);
    request->send(200); });
  server.on("/weather", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    StaticJsonDocument<1024> doc;
    doc["t"] = temperature;
    doc["h"] = humidity;
    doc["p"] = pressure;
    doc["a"] = altitude;
    String output;
    serializeJson(doc, output);
    request->send(200, "application/json", output); });
  // get/set wifi
  server.on("/wifi", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    StaticJsonDocument<1024> doc;
    doc["ssid"] = ssid;
    doc["password"] = password;
    doc["status"] = wifiStatus;
    doc["setupSSID"] = setupSSID;
    doc["setupPassword"] = setupPassword;
    String output;
    serializeJson(doc, output);
    request->send(200, "application/json", output); });
  server.on("/wifi", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
            {
    if(len < 1) return request->send(400);
    StaticJsonDocument<1024> doc;
    deserializeJson(doc, data, len);
    String newSSID = doc["ssid"].as<String>();
    String newPassword = doc["password"].as<String>();
    setupSSID = doc["setupSSID"].as<String>();
    setupPassword = doc["setupPassword"].as<String>();
    SetSSIDAndPassword(newSSID, newPassword);
    request->send(200); });
}