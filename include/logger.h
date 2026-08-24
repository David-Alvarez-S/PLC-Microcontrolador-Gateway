#ifndef LOGGER_H
#define LOGGER_H

#include "config_global.h"
#include <Arduino.h>
#ifdef DEBUG_MODE
#define log(x) (Serial.println(x))
#else
#define log(x)
#endif

#endif // LOGGER_H
