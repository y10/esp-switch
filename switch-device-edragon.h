#ifndef SWITCH_EDRAGON_DEVICE_H
#define SWITCH_EDRAGON_DEVICE_H

#define ALEXA
//#define DEVICE_TYPE "controllee"
#define DEVICE_TYPE "lightswitch"

// ESP12 (NodMCU 0.9)
#define RESET_PIN 0
#define RELAY_PIN 12
#define RELAY2_PIN 13
#define LED1_PIN 16

#include "switch-device.h"

extern SwitchDevice Device([](SwitchDevice* s){

    pinMode(LED1_PIN, OUTPUT);
    pinMode(RELAY_PIN, OUTPUT);

    pinMode(RESET_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(RESET_PIN), [](){Switch.restart();}, CHANGE);
});

#endif
