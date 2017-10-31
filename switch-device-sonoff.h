#ifndef SWITCH_SONOFF_DEVICE_H
#define SWITCH_SONOFF_DEVICE_H

#define RELAY 12
#define LED 13
#define RST 2

#include "switch-device.h"

class SonoffSwitch : public SwitchDevice
{
  public:

    SonoffSwitch()
      : SwitchDevice(RELAY, LED, RST)
    {}

    virtual void setup(SwitchClass &api) override
    {
      SwitchDevice::setup(api);
      
      // turn lights off
      digitalWrite(LED, HIGH);
    }

    virtual void turnOn() override
    {
      digitalWrite(LED, LOW);
      digitalWrite(RELAY, HIGH);
    }

    virtual void turnOff() override
    {
      digitalWrite(LED, HIGH);
      digitalWrite(RELAY, LOW);
    }
};

extern SwitchDevice Device = (SwitchDevice)SonoffSwitch();

#endif
