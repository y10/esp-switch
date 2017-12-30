#ifndef SWITCH_DEVICE_H
#define SWITCH_DEVICE_H

#include "switch.h"

void resetDevice()
{
  
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
      digitalWrite(_LED, LOW);
      digitalWrite(_RELAY, HIGH);
    }

    virtual void turnOff() 
    {
      digitalWrite(_LED, HIGH);
      digitalWrite(_RELAY, LOW);
    }

    virtual void reset()
    {
      Serial.println("[MAIN] Factory reset requested.");
      
      WiFi.disconnect(true);
      SPIFFS.format();
      system_restart();
    
      delay(5000);
    }

    virtual void restart()
    {
      Serial.println("[MAIN] Restarting...");
      system_restart();
    }
};

#endif
