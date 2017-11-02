#ifndef KEYPAD
#define KEYPAD
#include "stdbool.h"

enum display_state {ROLL, PITCH};

struct keypadState {
	char num_buffer[3];
	int roll_angle;
	int pitch_angle;
	bool operation_mode;
	enum display_state disp_state;
};

void initKeypad(void);
char scanKeypad(void);

void initKeypadState(struct keypadState *state);
void updateKeypadState(struct keypadState *state, char val);
void processKeypadInput(struct keypadState *kpState);

#endif
