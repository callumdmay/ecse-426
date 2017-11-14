#ifndef KEYPAD
#define KEYPAD
#include "stdbool.h"

enum display_state {ROLL, PITCH};
enum display_type {MEASURED, ENTERED};

struct keypadState {
	char num_buffer[3];
	int roll_angle;
	int pitch_angle;
	bool operation_mode;
	enum display_type disp_type;
	enum display_state disp_state;
};

#define DEBOUNCE_THRESHOLD 5
void initKeypad(void);
char scanKeypad(void);

void initKeypadState(struct keypadState *state);
void updateKeypadState(struct keypadState *state, char val);
void processKeypadInput(struct keypadState *kpState);

#endif
