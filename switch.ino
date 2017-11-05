#include <FS.h> //this needs to be first, or it all crashes and burns...
#include <DNSServer.h>
#include <ESP8266WiFi.h> //https://github.com/esp8266/Arduino
#include <ESPAsyncWebServer.h>
#include <ESPAsyncWiFiManager.h> //https://github.com/tzapu/WiFiManager
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
#include <Ticker.h>
#include <fauxmoESP.h>
#include "switch.h"
#include "switch-time.h"
#include "switch-settings.h"
#include "switch-device-edragon.h"
#include "switch-http.h"
#include "switch-ws.h"

/************ Global State ******************/
DNSServer dns;
AsyncWebServer httpServer(80);
AsyncWebSocket webSocket("/ws");
AsyncWiFiManager wifiManager(&httpServer, &dns);
fauxmoESP fauxmo;
SwitchHttp httpApi;
SwitchWs wsApi;
Ticker ticker;

//flag for saving data
bool shouldSaveConfig = false;

/*************************** Sketch Code ************************************/

void setup()
{
  Serial.begin(115200);

  Serial.print("[MAIN] Reset reason: ");
  Serial.println(ESP.getResetReason());

  setupDevice();

  ticker.attach(0.6, tick);

  setupSPIFFS();
  setupWifi();
  setupOTA();

  if (shouldSaveConfig)
  {
    saveConfig();
  }

  setupAlexa();
  setupHttp();
  setupTime();

  ticker.detach();

  Serial.println("[MAIN] System started.");
}

void loop()
{
  ArduinoOTA.handle();
  fauxmo.handle();
  Alarm.delay(0);
}

void setupDevice()
{
  Device.setup(Switch);
}

void setupSPIFFS()
{
  Settings.load();
}

void setupWifi()
{

  //set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
  wifiManager.setAPCallback(configModeCallback);
  //set config save notify callback
  wifiManager.setSaveConfigCallback(saveConfigCallback);

  char relay_name[50] = "";
  Settings.getDeviceName(relay_name, 49);
  // The extra parameters to be configured (can be either global or just in the setup)
  // After connecting, parameter.getValue() will get you the configured value
  // id/name placeholder/prompt default length
  AsyncWiFiManagerParameter custom_relay_name("relay_name", "Name", relay_name, sizeof(relay_name) - 1);

  //add all your parameters here
  wifiManager.addParameter(&custom_relay_name);

  wifiManager.setConfigPortalTimeout(300); // wait 5 minutes for Wifi config and then return

  String hostname("Switch-");
  hostname += String(ESP.getChipId(), HEX);

  if (!wifiManager.autoConnect(hostname.c_str()))
  {
    Serial.println("[MAIN] failed to connect and hit timeout");
    ESP.reset();
  }

  //if you get here you have connected to the WiFi
  Serial.println("[MAIN] connected to Wifi");

  Settings.setDeviceName(custom_relay_name.getValue());
}

void setupOTA()
{
  Serial.println("[OTA] Setup OTA");
  // OTA
  // An important note: make sure that your project setting of Flash size is at least double of size of the compiled program. Otherwise OTA fails on out-of-memory.
  ArduinoOTA.onStart([]() {
    Serial.println("[OTA] OTA: Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("[OTA] OTA: End");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("OTA progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    char errormsg[100];
    sprintf(errormsg, "OTA: Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR)
      strcpy(errormsg + strlen(errormsg), "Auth Failed");
    else if (error == OTA_BEGIN_ERROR)
      strcpy(errormsg + strlen(errormsg), "Begin Failed");
    else if (error == OTA_CONNECT_ERROR)
      strcpy(errormsg + strlen(errormsg), "Connect Failed");
    else if (error == OTA_RECEIVE_ERROR)
      strcpy(errormsg + strlen(errormsg), "Receive Failed");
    else if (error == OTA_END_ERROR)
      strcpy(errormsg + strlen(errormsg), "End Failed");
    Serial.println(errormsg);
  });
  ArduinoOTA.begin();
}

void saveConfig()
{
  Settings.save();
}

void setupAlexa()
{
  // Setup Alexa devices
  char relay_name[50] = "";
  if (Settings.getDeviceName(relay_name, 49) && sizeof(relay_name) > 1)
  {
    fauxmo.addDevice(relay_name);
    Serial.print("[MAIN] Added alexa device: ");
    Serial.println(relay_name);
  }

  fauxmo.onSet([](unsigned char device_id, const char *device_name, bool state) {
    Serial.printf("[MAIN] Set Device #%d (%s) state: %s\n", device_id, device_name, state ? "ON" : "OFF");
    state ? Switch.turnOn() : Switch.turnOff();
  });

  fauxmo.onGet([](unsigned char device_id, const char *device_name) {
    bool state = Switch.isOn();
    Serial.printf("[MAIN] Get Device #%d (%s) state: %s\n", device_id, device_name, state ? "ON" : "OFF");
    return state;
  });

}

void setupTime()
{
  // sync time
  Serial.println("[MAIN] Setup time synchronization");
  setSyncProvider([]() { return NTP.getTime(); });
  setSyncInterval(3600);
}

void setupHttp()
{
  // Setup Web UI
  Serial.println("[MAIN] Setup http server.");
  httpApi.setup(httpServer);
  wsApi.setup(webSocket);
  httpServer.addHandler(&webSocket);
  httpServer.begin();
}

void tick()
{
  //toggle state
  int state = digitalRead(LED); // get the current state of GPIO pin
  digitalWrite(LED, !state);    // set pin to the opposite state
}

//callback notifying us of the need to save config
void saveConfigCallback()
{
  Serial.println("[MAIN] Should save config");
  shouldSaveConfig = true;
}

//gets called when WiFiManager enters configuration mode
void configModeCallback(AsyncWiFiManager *myWiFiManager)
{
  Serial.println("[MAIN] Entered config mode");
  Serial.println(WiFi.softAPIP());
  //if you used auto generated SSID, print it
  Serial.println(myWiFiManager->getConfigPortalSSID());
  //entered config mode, make led toggle faster
  ticker.attach(0.2, tick);
}
