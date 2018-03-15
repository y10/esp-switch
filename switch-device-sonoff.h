#ifndef SWITCH_SONOFF_DEVICE_H
#define SWITCH_SONOFF_DEVICE_H

#define RESET_PIN 2
#define RELAY_PIN 12
#define LED1_PIN 13
#define GPIO_PIN 14

#include "switch.h"
#include "switch-device.h"

class SonoffSwitch : public SwitchDevice
{
private:
    unsigned long lastTimeChange = 0;

  public:
    SonoffSwitch()
        : SwitchDevice(RELAY_PIN, LED1_PIN, RESET_PIN)
    {
    }

    virtual void turnOn() override
    {
        lastTimeChange = millis();

        SwitchDevice::turnOn();
    }

    virtual void turnOff() override
    {
        lastTimeChange = millis();

        SwitchDevice::turnOff();
    }

    void toggle()
    {
        if ((millis() - lastTimeChange) > 500)
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
    }
};

extern SonoffSwitch Device = SonoffSwitch();

#endif
