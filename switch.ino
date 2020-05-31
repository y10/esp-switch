#define ESP8266

//#define NO_UI
//#define NO_IMAGE

/************ Includes ******************/
#include <FS.h> //this needs to be first, or it all crashes and burns...
#include <ESP8266WiFi.h> 
#include <ESP8266mDNS.h>
#include <DNSServer.h>
#include <ESPAsyncWebServer.h>
#include <ESPAsyncWiFiManager.h> 
#include <AsyncMqttClient.h>
#include <ArduinoOTA.h>
#include <ArduinoJson.h> //https://github.com/bblanchon/ArduinoJson
#include <fauxmoESP.h>
#include <Ticker.h>
#include "switch.h"
#include "switch-log.h"
#include "switch-settings.h"
#include "switch-device-sonoff.h"
#include "switch-time.h"
#include "switch-http.h"

/************ Global State ******************/
DNSServer dns;
AsyncWebServer httpServer(80);
AsyncEventSource webEvents("/events");
AsyncWebSocket webSocket("/ws");
AsyncWiFiManager wifiManager(&httpServer, &dns);
WiFiEventHandler wifiConnectHandler;
WiFiEventHandler wifiDisconnectHandler;
AsyncMqttClient mqttClient;
Ticker mqttLoop;
#ifdef ALEXA
fauxmoESP fauxmo;
#endif
Ticker ticker;
SwitchHttp httpApi;


void connectMqtt()
{
  if (!mqttClient.connected() && WiFi.isConnected()) 
  {
    Log.println("Connecting to MQTT...");
    mqttClient.connect();
  }
  else
  {
    mqttClient.publish(String("tele/" + (String)Settings.safename() + "/LWT").c_str(), 0, false, "Online");
  }
  
  mqttLoop.once(10, connectMqtt);
}

void ICACHE_RAM_ATTR tick();
void setupDevice();
void setup()
{
  Serial.begin(115200);
  Log.print("[MAIN] Reset reason: ");
  Log.println(ESP.getResetReason());


  setupSwitch();
  setupDevice();

  ticker.attach(0.6, tick);
  setupWifi();
  setupOTA();
  setupLog();  
  setupHttp();
  setupAlexa();
  setupMQTT();
  setupTime();
  ticker.detach();

  Log.println("[MAIN] System started.");
}

void loop()
{
  ArduinoOTA.handle();

#ifdef ALEXA
  fauxmo.handle();
#endif

  Alarm.delay(0);
}

void setupLog()
{
#ifdef WEBLOG  
  Serial.println("[MAIN] Setup log");
  Log.setup(&webEvents);
#endif  
}

void setupDevice()
{
  Device.setup(Settings);
}

void setupSwitch()
{
  Switch.onRestart([&]() {
    Device.restart();
  });
  Switch.onReset([&]() {
    Device.reset();
  });

  Switch.onTurnOn([&]() {
    mqttClient.publish(String("stat/" + (String)Settings.safename() + "/RESULT").c_str(), 2, true, "{\"POWER\": \"ON\"}");
    Device.turnOn();
  });
  Switch.onTurnOff([&]() {
    mqttClient.publish(String("stat/" + (String)Settings.safename() + "/RESULT").c_str(), 2, true, "{\"POWER\": \"OFF\"}");
    Device.turnOff();
  });

  Switch.onToggle([&]() {
    Device.toggle();
  });

  Switch.onChange([&](){
      String state = Switch.toJSON();
      webSocket.textAll(state);
  });  
}

void setupWifi()
{
  static AsyncWiFiManagerParameter custom_disp_name("disp_name", "Display Name", Settings.dispname().c_str(), 50);
  wifiManager.addParameter(&custom_disp_name);

  static AsyncWiFiManagerParameter custom_pair_addr("pair_addr", "Paired Device", Settings.pairaddr().c_str(), 50);
  wifiManager.addParameter(&custom_pair_addr);

  static AsyncWiFiManagerParameter custom_mqtt_host("mqtt_host", "MQTT Host or IP", Settings.mqtthost().c_str(), 50);
  wifiManager.addParameter(&custom_mqtt_host);

  static AsyncWiFiManagerParameter custom_mqtt_port("mqtt_port", "MQTT Port", Settings.mqttport().c_str(), 10);
  wifiManager.addParameter(&custom_mqtt_port);

  static AsyncWiFiManagerParameter custom_mqtt_user("mqtt_user", "MQTT User", Settings.mqttuser().c_str(), 50);
  wifiManager.addParameter(&custom_mqtt_user);

  static AsyncWiFiManagerParameter custom_mqtt_pwrd("mqtt_pwrd", "MQTT Password", Settings.mqttpwrd().c_str(), 50);
  wifiManager.addParameter(&custom_mqtt_pwrd);

  //set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
  wifiManager.setAPCallback([](AsyncWiFiManager *myWiFiManager)
  {
    Log.println("[MAIN] Entered config mode");
    Log.println(WiFi.softAPIP().toString());
    //if you used auto generated SSID, print it
    Log.println(myWiFiManager->getConfigPortalSSID());
    //entered config mode, make led toggle faster
    ticker.attach(0.2, tick);
  });
  
  //set config save notify callback
  wifiManager.setSaveConfigCallback([](){
    Settings.dispname(custom_disp_name.getValue());
    Settings.pairaddr(custom_pair_addr.getValue());
    Settings.mqtthost(custom_mqtt_host.getValue());
    Settings.mqttport(atoi(custom_mqtt_port.getValue()));
    Settings.mqttuser(custom_mqtt_user.getValue());
    Settings.mqttpwrd(custom_mqtt_pwrd.getValue());
    Settings.save();
  });

  wifiManager.setConfigPortalTimeout(300); // wait 5 minutes for Wifi config and then return

  if (!wifiManager.autoConnect(Settings.hostname().c_str()))
  {
    Log.println("[MAIN] failed to connect and hit timeout");
    ESP.reset();
  }

  //if you get here you have connected to the WiFi
  Log.println("[MAIN] connected to Wifi");
}

void setupOTA()
{
  Log.println("[OTA] Setup OTA");
  // OTA
  // An important note: make sure that your project setting of Flash size is at least double of size of the compiled program. Otherwise OTA fails on out-of-memory.
  ArduinoOTA.onStart([]() {
    Log.println("[OTA] OTA: Start");
  });
  ArduinoOTA.onEnd([]() {
    Log.println("[OTA] OTA: End");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Log.printf("OTA progress: %u%%\r", (progress / (total / 100)));
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
    Log.println(errormsg);
  });
  ArduinoOTA.setHostname(Settings.hostname().c_str());
  ArduinoOTA.begin();
}

void setupAlexa()
{
#ifndef ALEXA
  return;
#endif

  if (Settings.pairaddr().length() == 0 && Settings.dispname().length() > 0)
  {
    // Setup Alexa devices
    fauxmo.addDevice(Settings.dispname().c_str(), DEVICE_TYPE);
    Log.print("[MAIN] Added alexa device: ");
    Log.println(Settings.dispname());

    fauxmo.onSet([](unsigned char device_id, const char *device_name, bool state) {
      Log.printf("[MAIN] Set Device #%d (%s) state: %s\n", device_id, device_name, state ? "ON" : "OFF");
      state ? Switch.turnOn() : Switch.turnOff();
    });

    fauxmo.onGet([](unsigned char device_id, const char *device_name) {
      bool state = Switch.isOn();
      Log.printf("[MAIN] Get Device #%d (%s) state: %s\n", device_id, device_name, state ? "ON" : "OFF");
      return state;
    });
  }
  else
  {
    fauxmo.enable(false);
  }
}

void setupMQTT()
{
  Log.println("[MQTT] Setup MQTT");
  
  mqttClient.onConnect([](bool sessionPresent) {
    Log.println("Connected to MQTT.\r\n  Session present: " + String(sessionPresent));
    
    mqttClient.subscribe(String("cmnd/" + (String)Settings.safename() + "/POWER").c_str(), 0);
    
    mqttClient.publish(String("stat/" + (String)Settings.safename() + "/RESULT").c_str(), 0, false, String("{\"POWER\": \"" + (String)(Switch.isOn() ? "ON" : "OFF") + "\"}").c_str());
  });

  mqttClient.onDisconnect([](AsyncMqttClientDisconnectReason reason) {
    Log.println("Disconnected from MQTT.");
  });

  mqttClient.onMessage([](char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
    
    String message;
    for (int i = 0; i < len; i++) {
      message += (char)payload[i];
    }

    Log.println((String)"Command received." +
    "\r\n  topic: " + String(topic) +
    "\r\n  message: " + message +
    "\r\n  qos: " + String(properties.qos) +
    "\r\n  dup: " + String(properties.dup) + 
    "\r\n  retain: " + String(properties.retain));

    if (strcmp(topic, String("cmnd/" + (String)Settings.safename() + "/POWER").c_str()) == 0) {
      if (message == "ON") {
        Switch.turnOn();
      } else {
        Switch.turnOff();
      }
    }
  });

  if (Settings.setupMQTT(mqttClient)) {
    connectMqtt();
  }
}

void setupTime()
{
  // sync time
  Log.println("[MAIN] Setup time synchronization");
  setSyncProvider([]() { return NTP.getTime(); });
  setSyncInterval(3600);
}

void setupHttp()
{
  // Setup Web UI
  Log.println("[MAIN] Setup http server.");
  httpApi.setup(httpServer);
  httpServer.addHandler(&webSocket);
  httpServer.addHandler(&webEvents);
  httpServer.begin();
}

void tick()
{
  //toggle state
  int state = digitalRead(LED1_PIN); // get the current state of GPIO pin
  digitalWrite(LED1_PIN, !state);    // set pin to the opposite state
}
