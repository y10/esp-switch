#ifndef SWITCH_SONOFF_DEVICE_H
#define SWITCH_SONOFF_DEVICE_H

//#define ALEXA

// Generic ESP8266
//#define RESET_PIN 0
#define RELAY_PIN 12
#define LED1_PIN 13
//#define GPIO_PIN 14
#define GPIO_PIN 0

#include "switch-device.h"

ICACHE_RAM_ATTR void togleSwitch()
{
    detachInterrupt(digitalPinToInterrupt(GPIO_PIN));
    Switch.toggle();
    attachInterrupt(digitalPinToInterrupt(GPIO_PIN), togleSwitch, CHANGE);
}

extern SwitchDevice Device([](SwitchDevice* s){

    pinMode(LED1_PIN, OUTPUT);
    pinMode(RELAY_PIN, OUTPUT);

#ifdef RESET_PIN
    pinMode(RESET_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(RESET_PIN), [](){Switch.restart();}, CHANGE);
#endif  
#ifdef GPIO_PIN
    pinMode(GPIO_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(GPIO_PIN), togleSwitch, CHANGE);
#endif  

 });


#endif
