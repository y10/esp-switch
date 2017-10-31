#ifndef SWITCH_EDRAGON_DEVICE_H
#define SWITCH_EDRAGON_DEVICE_H

#define RELAY 12
#define RELAY_2 13
#define LED 16
#define RST 2

#include "switch-device.h"

class EdragonSwitch : public SwitchDevice
{

  public:

    EdragonSwitch()
    : SwitchDevice(RELAY, LED, RST) 
    {}
};

extern SwitchDevice Device = (SwitchDevice)EdragonSwitch();

#endif
