#ifndef SWITCH_DEVICE_H
#define SWITCH_DEVICE_H

#include <FS.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESP8266HTTPClient.h>

#include "switch.h"
#include "switch-settings.h"

class DeviceClass
{
public:

  virtual void turnOn() = 0;

  virtual void turnOff() = 0;

  virtual void toggle() = 0;
};

class LocalSwitch : public DeviceClass
{
protected:

  unsigned int _RELAY;
  unsigned int _LED;

public:
  LocalSwitch(int relay, int led) : 
  _RELAY(relay), 
  _LED(led)
  {
  }

  void turnOn() override
  {
    Serial.println("[Device] on");
    digitalWrite(_LED, HIGH);
    digitalWrite(_RELAY, HIGH);
  }

  void turnOff() override
  {
    Serial.println("[Device] off");
    digitalWrite(_LED, LOW);
    digitalWrite(_RELAY, LOW);
  }

  void toggle() override
  {
    if (Switch.isOn())
    {
      Switch.turnOff();
    }
    else
    {
      Switch.turnOn();
    }
  }
};

class RemoteSwitch : public DeviceClass
{
  String device_addr;

  DeviceClass * device;

public:
  RemoteSwitch(String addr, DeviceClass* impl) : device_addr(addr), device(impl)
  {
  }

  void turnOn() override
  {
    perform("on", [this](){
      device->turnOn();
    });
  }

  void turnOff() override
  {
    perform("off", [this](){
      device->turnOff();
    });
  }

  void toggle() override
  {
    perform("toggle", [this](){
      device->toggle();
    });
  }

private:
 
  void perform(const char *action, std::function<void()> onComplete)
  {
    AsyncClient * ac = new AsyncClient();

    ac->onError([](void *arg, AsyncClient *c, int8_t error) {
      delete c;
    }, 0);

    ac->onConnect([this, action, onComplete](void *arg, AsyncClient *c) {
      c->onError(NULL, NULL);
      Serial.println("Connected");
      this->send(action, c);
      //onComplete();
      c->close();
    }, 0);

    ac->onDisconnect([this](void *arg, AsyncClient *c) {
      c->onError(NULL, NULL);
      Serial.println("Disconnected");
      delete c;
    }, 0);

    if(!ac->connect(device_addr.c_str(), 80))
    {
      delete ac;
    }    
  }

  void send(const char *action, AsyncClient *client)
  {
    Serial.println("[" + device_addr + "] " + (String)action);
    String request = "GET /api/" + (String)action + " HTTP/1.1\r\nHost: " + device_addr + "\r\nConnection: close\r\n\r\n";
    client->write(request.c_str());
  }
};


class SwitchDevice : DeviceClass
{
private:
  std::function<void(SwitchDevice * s)> onSetup;

  DeviceClass * switch_impl = NULL;

  unsigned long lastTimeChange = 0;

public:
  SwitchDevice(std::function<void(SwitchDevice * s)> onSetupCallback) : onSetup(onSetupCallback), lastTimeChange(0)
  {
  }
  
  void turnOn()
  {
    lastTimeChange = millis();
    if (switch_impl) switch_impl->turnOn();
  }

  void turnOff()
  {
    lastTimeChange = millis();
    if (switch_impl) switch_impl->turnOff();
  }

  void toggle()
  {
    if ((millis() - lastTimeChange) > 50)
    {
      if (switch_impl) switch_impl->toggle();
    }
  }

  virtual void reset()
  {
    Serial.println("[MAIN] Factory reset requested.");

    WiFi.disconnect(true);
    SPIFFS.format();
    ESP.restart();

    delay(5000);
  }

  virtual void restart()
  {
    Serial.println("[MAIN] Restarting...");
    ESP.restart();
  }

  void setup(SwitchSettings& settings)
  {
    switch_impl = new LocalSwitch(RELAY_PIN, LED1_PIN);

    onSetup(this);

    Settings.load();

    if (Settings.pairaddr().length() > 0)
    {
      Serial.println("[MAIN] Pairing " + Settings.pairaddr() + " remote device");

      RemoteSwitch * remote_switch = new RemoteSwitch(Settings.pairaddr(), switch_impl);

      switch_impl = remote_switch;
    }
  }
};

#endif
