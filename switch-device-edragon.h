#ifndef SWITCH_EDRAGON_DEVICE_H
#define SWITCH_EDRAGON_DEVICE_H

#define RESET_PIN 2
#define RELAY_PIN 12
#define RELAY2_PIN 13
#define LED1_PIN 16

#include "switch-device.h"

extern SwitchDevice Device = SwitchDevice(RELAY_PIN, LED1_PIN, RESET_PIN);

#endif
