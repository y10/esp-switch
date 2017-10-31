#ifndef SWITCH_DEVICE_H
#define SWITCH_DEVICE_H

#include "switch.h"

void resetDevice()
{
  Serial.println("[MAIN] Factory reset requested.");
  
  WiFi.disconnect(true);
  SPIFFS.format();
  system_restart();

  delay(5000);
}
  
class SwitchDevice
{
  protected:
  
    unsigned int _RELAY;
    unsigned int _LED;
    unsigned int _RST;
    
  public:

    SwitchDevice(int relay, int led, int rst) : _RELAY(relay), _LED(led), _RST(rst)
    {}
    
    virtual void turnOn() 
    {
      digitalWrite(_RELAY, HIGH);
    }

    virtual void turnOff() 
    {
      digitalWrite(_RELAY, LOW);
    }

    virtual void reset()
    {
      resetDevice();
    }

    virtual void restart()
    {
      Serial.println("[MAIN] Restarting...");
      system_restart();
    }
    
    virtual void setup(SwitchClass &api)
    {
      //set led pin as output
      pinMode(_LED, OUTPUT);

      //attach reset handler
      pinMode(_RST, INPUT_PULLUP);
      attachInterrupt(digitalPinToInterrupt(_RST), resetDevice, CHANGE);

      // set relay as outputs
      pinMode(_RELAY, OUTPUT);

      api.onTurnOn([&]() {
        turnOn();
      });
      api.onTurnOff([&]() {
        turnOff();
      });
      api.onRestart([&]() {
        restart();
      });
      api.onReset([&]() {
        reset();
      });

      //resetDevice();
    }
};

#endif
