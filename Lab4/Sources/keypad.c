#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include "keypad.h"
#include "LED.h"

void initKeypadState(struct keypadState *state);
void updateKeypadState(struct keypadState *state, char val);
void processKeypadInput(struct keypadState *kpState);
void Thread_keypad (void const *argument);      // thread function
void initKeypad(void);

GPIO_InitTypeDef GPIO_InitDef_Row;
GPIO_InitTypeDef GPIO_InitDef_Col;

osThreadId tid_Thread_keypad;                              // thread id
osThreadDef(Thread_keypad, osPriorityNormal, 1, 0);

void start_thread_keypad (void) {
	tid_Thread_keypad = osThreadCreate(osThread(Thread_keypad), NULL);
}

void Thread_keypad (void const *argument) {
	initKeypad();
  while(1) {
    osDelay(230);
  }
}

void initKeypadState(struct keypadState *state) {
	state->roll_angle = -1;
	state->pitch_angle = -1;
	state->num_buffer[0] = state->num_buffer[1] = state->num_buffer[2] = '\0';
	state->operation_mode = false;
	state->disp_state = ROLL;
	state->disp_type = MEASURED;
}

//Initialize GPIO pins
void initKeypad(void) {
	__HAL_RCC_GPIOE_CLK_ENABLE();

	//Using E7 - E13
	GPIO_InitDef_Row.Pin = GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10;
	GPIO_InitDef_Row.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitDef_Row.Pull = GPIO_PULLDOWN;
	GPIO_InitDef_Row.Speed = GPIO_SPEED_HIGH;

	GPIO_InitDef_Col.Pin = GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13;
	GPIO_InitDef_Col.Mode = GPIO_MODE_INPUT;
	GPIO_InitDef_Col.Pull = GPIO_PULLDOWN;
	GPIO_InitDef_Col.Speed = GPIO_SPEED_HIGH;

	//init the structures
	HAL_GPIO_Init(GPIOE, &GPIO_InitDef_Row);
	HAL_GPIO_Init(GPIOE, &GPIO_InitDef_Col);
}

//Scan keypad for value
char scanKeypad(void) {
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10, GPIO_PIN_RESET);
	if (HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_11) == 1) {
		return '1';
	}
	else if (HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_12) == 1) {
		return '2';
	}
	else if (HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_13) == 1) {
		return '3';
	}

	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7|GPIO_PIN_9|GPIO_PIN_10, GPIO_PIN_RESET);
	if (HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_11) == 1) {
		return '4';
	}
	else if (HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_12) == 1) {
		return '5';
	}
	else if (HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_13) == 1) {
		return '6';
	}

	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_10, GPIO_PIN_RESET);
	if (HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_11) == 1) {
		return '7';
	}
	else if (HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_12) == 1) {
		return '8';
	}
	else if (HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_13) == 1) {
		return '9';
	}

	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_10, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9, GPIO_PIN_RESET);
	if (HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_11) == 1) {
		return '*';
	}
	else if (HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_12) == 1) {
		return '0';
	}
	else if (HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_13) == 1) {
		return '#';
	}

	return '\0';
}

//Update keypad state with input char
void updateKeypadState(struct keypadState *state, char val) {
	int i;
	int length = sizeof(state->num_buffer)/sizeof(char);
	bool has_buffer = false;

	switch (val) {
		case '*':
			for (i = 2; i >= 0; i--) {
				if (state->num_buffer[i] != '\0') {
					state->num_buffer[i] = '\0';
					return;
				}
			}
			break;
		case '#':

			if (state->operation_mode == false) {
				for (i = 0; i < length; i++) {
					if (state->num_buffer[i] != '\0') {
						has_buffer = true;
					}
				}

				if (!has_buffer) {
					return;
				}

				if (state->roll_angle == -1) {
					int buf_val;
					sscanf(state->num_buffer, "%d", &buf_val);
					if (buf_val >=0 && buf_val <= 180) {
						state->roll_angle = buf_val;
						state->num_buffer[0] = state->num_buffer[1] = state->num_buffer[2] = '\0';
					}
					return;
				}

				if (state->pitch_angle == -1) {
					int buf_val;
					sscanf(state->num_buffer, "%d", &buf_val);
					if (buf_val >=0 && buf_val <= 180) {
						state->pitch_angle = buf_val;
						state->num_buffer[0] = state->num_buffer[1] = state->num_buffer[2] = '\0';
					}
				}

				state->operation_mode = true;
			} else {
				if (state->disp_type == MEASURED) {
					state->disp_type = ENTERED;
				} else {
					state->disp_type = MEASURED;
				}
			}

			break;
		default:
			if(state->operation_mode == true) {
				if (val == '1') {
					state->disp_state = PITCH;
				}

				if (val == '2') {
					state->disp_state = ROLL;
				}
			} else {
				for (i = 0; i < length; i++) {
					if (state->num_buffer[i] == '\0') {
						state->num_buffer[i] = val;
						return;
					}
				}
			}
			break;
	}
}

//This function debounces the keypad input into a usable value
void processKeypadInput(struct keypadState *state) {
	static int debounce_counter = 0;
	static int debounce_down_counter = DEBOUNCE_THRESHOLD;
	static char last_char = '\0';

	char keypad_char = scanKeypad();
	if (keypad_char != '\0') {
		if (last_char == '\0') {
			debounce_counter = 0;
		}
		last_char = keypad_char;

		debounce_counter++;
		} else {
			if (debounce_counter > DEBOUNCE_THRESHOLD && debounce_down_counter > 0) {
				debounce_down_counter--;
			} else {
				if (debounce_counter >= 400 && last_char == '#' && state-> pitch_angle != -1 && state->roll_angle != -1) {
					state->operation_mode = true;
				} else if (debounce_counter >= 400 && last_char == '*' && state-> pitch_angle != -1 && state->roll_angle != -1) {
					state->operation_mode = false;
				} else if (debounce_counter >= 100 && last_char == '*') {
					if (state->operation_mode ==  true) {
						initKeypadState(state);
						//Turn the LED off
						int zero[2]={0};
						LEDSet(zero);
					}
				} else if (debounce_counter >= DEBOUNCE_THRESHOLD) {
					updateKeypadState(state, last_char);
				}
				debounce_down_counter = DEBOUNCE_THRESHOLD;
				last_char = '\0';
				debounce_counter = 0;
			}
	}
}
