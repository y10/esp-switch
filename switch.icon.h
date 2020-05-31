#pragma once

#ifndef NO_IMAGE
#include "data/icon.png.gz.h"
#include "data/apple-touch-icon.png.gz.h"
#else
const uint8_t SWITCH_ICON_PNG_GZ[] PROGMEM = {};
const uint8_t SWITCH_APPLE_TOUCH_ICON_PNG_GZ[] PROGMEM = {};
#endif