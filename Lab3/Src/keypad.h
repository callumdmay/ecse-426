#ifndef KEYPAD
#define KEYPAD
#include "stdbool.h"

struct keypadState {
	char num_buffer[3];
	int roll_angle;
	int pitch_angle;
	bool operation_mode;
};

void initKeypad(void);
char scanKeypad(void);

void initKeypadState(struct keypadState *state);
void updateKeypadState(struct keypadState *state, char val);

#endif
