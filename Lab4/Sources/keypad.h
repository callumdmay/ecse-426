#ifndef KEYPAD
#define KEYPAD
#include "stdbool.h"
#include "cmsis_os.h"

enum display_state {ROLL, PITCH};
enum display_type {MEASURED, ENTERED};

//This struct defines all states of the keypad
struct keypadState {
	char num_buffer[3];
	int roll_angle;
	int pitch_angle;
	bool operation_mode;
	enum display_type disp_type;
	enum display_state disp_state;
};

extern struct keypadState kpState;

//Create our keypad struct mutex
extern osMutexId keypad_mutex;

#define DEBOUNCE_THRESHOLD 3

void start_thread_keypad (void);
void initKeypad(void);

#endif
