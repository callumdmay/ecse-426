#ifndef MAIN_H
#define MAIN_H
#include "keypad.h"

#define DEBOUNCE_THRESHOLD = 5;

void process_switch(struct keypadState *kpState);

#endif
