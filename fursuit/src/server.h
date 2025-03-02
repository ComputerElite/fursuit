
#include <ESPAsyncWebServer.h>
extern void sendMessageToAllWSClients(char* msg);
extern AsyncWebSocket ws;
extern AsyncWebServer server;
extern void SetupServer();
extern void RestartServer();